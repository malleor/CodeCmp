#
# TODO:
#  - generate a friendly report (html?)
#

import re
import hashlib
import time
import copy
import json


symbolPat = r'[a-zA-Z_][a-zA-Z0-9_]*'
constantPat = r'-?[0-9]+([,.][0-9])?'

verbose = False

# finding matches
minSymbolOccurences = 3
minSymbolCorrespondenceLength = 3

# finding blocks
blockMinRepCount = 5
blockMaxGap = 1



class SymbolPrints:
	def __init__(self):
		self.name = ''
		self.positions = []
		self.lines = []
		self.prints = []
		
	def __str__(self):
		if verbose:
			return '\'%s\' - %d occurences, {positions: %s, lines: %s, prints: %s}' % \
				(self.name, len(self.positions), str(self.positions), str(self.lines), str(self.prints))
		else:
			return '\'%s\' - %d occurences, prints: %s' % \
				(self.name, len(self.positions), str(self.prints))

class CodePrints:
	def __init__(self):
		self.code = ''
		lineCount = 0
		self.symbols = {} # symbol name -> SymbolPrints object
		
	def __str__(self):
		s = '%d symbols: \n' % len(self.symbols)
		
		# sort for output
		si = self.symbols.items()
		si.sort(key = lambda x: len(x[1].positions), reverse = True)
		
		for sp in si[:10]:
			if sp[1].prints != []:
				s += '  ' + str(sp[1]) + '\n'
		return s
		
	@staticmethod
	def Parse(code):
		cp = CodePrints()
		cp.code = code
		
		# parse the code
		positionsPerSymbol = FindPrints(code)
		positions = positionsPerSymbol.values()
		
		# convert
		(lines, cp.lineCount) = ConvertPositionsToLines(code, positions)
		prints = ConvertLinesToLineDistances(lines)
		
		# fit in
		symbolList = positionsPerSymbol.keys()
		for symInd in range(len(positionsPerSymbol)):
			sp = SymbolPrints()
			sp.name = symbolList[symInd]
			sp.positions = positionsPerSymbol[symbolList[symInd]]
			sp.lines = lines[symInd]
			sp.prints = prints[symInd]
			cp.symbols[symbolList[symInd]] = sp
		
		return cp
	
	@staticmethod
	def ParseFile(path):
		filestr = open(path).read()
		prints = FindPrints(filestr)
		cp = CodePrints.Parse(filestr)
		return cp
		
class ComparisonResult:
	def __init__(self):
		self.source = None	# CodePrints object
		self.target = None	# CodePrints object
		self.matches = []	# a list of source line numbers per line
		self.blocks = []	# a list of tuples of form: 
							#  (src_start=int, dst_start=int, block_size=int, proof=[symbol=string, ...])
		
	def __str__(self):
		s = '[\n'
		i = 0
		for z in [[y[0] for y in x] for x in self.matches]:
			s += '  %4d: '%i + str(z) + ',\n'
			i += 1
		return s + ']'
		# return 'Matching stats: \n  max guesses per line: %d\n  avg guesses per line: %d' % \
			# (max([len(x) for x in self.matches]), sum([len(x) for x in self.matches])/len(self.matches))
			
	def Report(self, name):		
		# s = ''
		# for b in self.blocks:
			# if verbose:
				# s += '%4d lines from %d to %d, proof: '%(b[2], b[1], b[0]) + str(b[3]) + '\n'
			# else:
				# s += '%4d lines from %d to %d'%(b[2], b[1], b[0]) + '\n'
		# return s
		
		# form a json object with report data
		jsonObj = {
			'sourceCode': self.source.code, 
			'targetCode': self.target.code, 
			'matchingBlocks': []
			}
		for b in self.blocks:
			jsonObj['matchingBlocks'].append({
				'sourceStart': b[0], 
				'targetStart': b[1], 
				'blockSize': b[2], 
				'symbols': b[3]
				})
				
		# output the report from html template
		htmlTemplate = open('ReportTemplate.htm', 'r').read()
		htmlOutput = htmlTemplate + \
			'\n\n<script type="text/javascript">\n\tvar content=' + \
			json.dumps(jsonObj, ensure_ascii=False) + \
			';\n\twindow.loadContent(content);\n</script>'
		open(name+'.htm', 'w').write(htmlOutput)
		
		return 'Done. Report in \'%s\'.' % (name+'.htm')


# returns a dictionary of line-positions of symbols in the code
def FindPrints(code):
	prints = {}
	for r in re.finditer(symbolPat, code):
		span = r.span()
		match = code[span[0]:span[1]]
		pos = r.start()
		if match not in prints:
			prints[match] = [pos]
		else:
			prints[match].append(pos)
	return prints

	
# converts positions in the string into line numbers
# line positions are insensitive to symbol name changes
def ConvertPositionsToLines(code, positions):
	# lazy calc line ends
	h = hashlib.md5()
	h.update(code)
	md5 = h.digest()
	lineNumbers = [0]
	if md5 not in ConvertPositionsToLines.lineNumbersStatic:
		t = time.clock()
		for ch in code[0:-1]:
			if ch == '\n':
				lineNumbers.append(lineNumbers[-1]+1)
			else:
				lineNumbers.append(lineNumbers[-1])
		if verbose:
			print '(%d lines parsed in %.1fms)' % (lineNumbers[-1]+1, (time.clock()-t)*1000)
		
		ConvertPositionsToLines.lineNumbersStatic[md5] = lineNumbers
	else:
		lineNumbers = ConvertPositionsToLines.lineNumbersStatic[md5]
		
	# convert
	t = time.clock()
	lines = []
	for x in positions:
		lines.append([lineNumbers[y] for y in x])
	if verbose:
		print '(converted in %.1fms)' % ((time.clock()-t)*1000)
	
	return (lines, lineNumbers[-1]+1)
	
ConvertPositionsToLines.lineNumbersStatic = {}


def ConvertLinesToLineDistances(lines):	
	distances = []
	for x in lines:
		distances.append([y[1]-y[0] for y in zip(x, x[1:])])
	return distances
	

def CompareCodePrints(cp1, cp2):	
	# init output
	linesMapping = [[] for i in range(cp2.lineCount)]
	
	# for each combination of symbols...
	for s1 in cp1.symbols:
		l1 = cp1.symbols[s1].prints
		if len(l1) < minSymbolOccurences: 
			continue
		
		for s2 in cp2.symbols:
			l2 = cp2.symbols[s2].prints
			if len(l2) < minSymbolOccurences: 
				continue
			
			if verbose:
				print 'Comparing symbols \'%s\' and \'%s\'.' % (str(s1), str(s2))
			
			# ...find coverage regions
			r1 = range(len(l1))
			matches = [[] for i in r1]
			for mStart1 in r1[:-1]:
				mStart2 = -1
				while True:
					mStart2 = l2.index(l1[mStart1], mStart2+1) if l1[mStart1] in l2[mStart2+1:] else None
					if mStart2 is None: 
						break
					
					# control duplicate mathes
					if mStart2 in matches[mStart1]:
						continue
						
					# find matching subsequence
					mLen = 1
					while mStart1+mLen < len(l1) and \
						  mStart2+mLen < len(l2) and \
						  l1[mStart1+mLen] == l2[mStart2+mLen]:
						mLen += 1
						
					# minimal coverage constraint
					if mLen < minSymbolCorrespondenceLength:
						continue;
					
					# note down the match
					for i in range(mLen):
						try:
							matches[mStart1+i].append(mStart2+i)
						except:
							print '  i =', i
							print '  mStart1 =', mStart1
							print '  len(matches) =', len(matches)
							print '  matches[mStart1:] =', matches[mStart1:]
							return
					for i in range(mLen):
						sourceLine = cp1.symbols[s1].lines[mStart1+i]
						targetLine = cp2.symbols[s2].lines[mStart2+i]
						linesMapping[targetLine].append((sourceLine,s1))
						
					if verbose:
						print '  Found subsequence:', l1[mStart1:mStart1+mLen]
				
	# create the results container
	cr = ComparisonResult()
	cr.source = cp1
	cr.target = cp2
	cr.matches = [sorted(x, key = lambda y: y[0]) for x in linesMapping]
	
	# form matches in blocks
	uniqueLineNumbers = []
	for l in cr.matches:
		uniqueLineNumbers.append(list(set([y[0] for y in l])))
	for targetLineNumber in range(len(uniqueLineNumbers)):
		sourceLineNumbers = uniqueLineNumbers[targetLineNumber]
		for sourceLineNumber in sourceLineNumbers:
			# find the block
			wondIt = 1
			lastFoundOffset = 0
			proof = []
			while targetLineNumber+wondIt < len(uniqueLineNumbers):
				if sourceLineNumber+wondIt in uniqueLineNumbers[targetLineNumber+wondIt]:
					lastFoundOffset = wondIt
					proof += [x[1] for x in cr.matches[targetLineNumber+wondIt] if x[0] == sourceLineNumber+wondIt]
				elif wondIt-lastFoundOffset > blockMaxGap:
					break
				wondIt += 1
				
			# save the block
			if len(proof) >= blockMinRepCount:
				cr.blocks.append((sourceLineNumber, targetLineNumber, wondIt, proof))
				
	return cr

	
if __name__ == '__main__':
	cp1 = CodePrints.ParseFile('real_tests\pu\Unit2.h')
	cp2 = CodePrints.ParseFile('real_tests\cz\Unit2.h')
	
	cr = CompareCodePrints(cp1, cp2)
	print cr.Report('Unit2Test')
	