import re


flatWSPat = r'[ \t]'
flatNoWSPat = r'[^ \t]'
indentPat = r'^[ \t]*'
symPat = r'[a-zA-Z_][a-zA-Z0-9_]*'
numPat = r'-?[0-9]+([,.][0-9])?'
funArgPat = r'%s*%s%s*' % (flatWSPat, symPat, flatWSPat);
funArgsPat = r'(%s(,%s*%s)*)|(%s)' % (funArgPat, flatWSPat, funArgPat, flatWSPat)
scopePat = r'(%s.*?\n)+'
funPat = r'def%s+(?P<name>%s)\((?P<args>%s)?\):%s*?\n' % (flatWSPat, symPat, funArgsPat, flatWSPat)

# print 'symbol pattern:\n%s\n' % symPat
# print 'numeral pattern:\n%s\n' % numPat
# print 'function argument pattern:\n%s\n' % funArgPat
# print 'function arguments pattern:\n%s\n' % funArgsPat
# print 'function pattern:\n%s\n' % funPat
# print 'scope pattern:\n%s\n' % (scopePat % ' ')

funDefFinder = re.compile(funPat, re.M)
flatNoWSPatFinder = re.compile(flatNoWSPat)
indentFinder = re.compile(indentPat)


### A scope object contains an ordered collection of 
### function definitions, statements, expressions, etc. 
class Scope:
	def __init__(self):
		self.elements = []
	def __str__(self):
		if len(self.elements) == 0:
			return '<empty>\n'
			
		s = ''
		for e in self.elements:
			s = s + str(e) + '\n'
		return 'Scope elements: \n' + s

### A function definition found in the source.
### Function definitions are scope elements. 
class Func:
	def __init__(self, name='', args='', body=Scope()):
		self.name = name
		self.args = args
		self.body = body
	def __str__(self):
		return 'function definition:\n  name: %s\n  arguments: %s\n  body: \'\n%s\n\'' % \
			(self.name, str(self.args), str(self.body))
		
	
def parseBlock(blockStr):
	sc = Scope()
	
	# find block's indent
	indSpan = indentFinder.search(blockStr).span()
	ind = blockStr[indSpan[0]:indSpan[1]]
	
	# search for block's elements
	while True:
		# search for enother element in the scope
		symIt = funDefFinder.search(blockStr)
		if symIt == None:
			break
	
		# find the function's body
		wsb = symIt.end()
		bm = flatNoWSPatFinder.search(blockStr[wsb:])
		if bm == None:
			body = None
			bs = (wsb, wsb)
		else:
			wse = wsb+bm.start()
			#print '  body pattern: \'%s\'' % (scopePat % blockStr[wsb:wse])
			funBodyFinder = re.compile(scopePat % blockStr[wsb:wse], re.M)
			bs = funBodyFinder.match(blockStr[wsb:]).span()
			bs = (bs[0]+wsb, bs[1]+wsb)
			#print '\n\n', blockStr[bs[0]:bs[1]], '\n\n'
			body = parseBlock(blockStr[bs[0]:bs[1]])
			#print body
			
		# save in the object
		f = Func(symIt.group('name'), symIt.group('args'), body)
		sc.elements.append(f)
		
		blockStr = blockStr[bs[1]:]
		
	return sc

def performTest(test):
	print 'Input file: \'%s\'.' % test
	
	# load file
	filestr = open(test).read()
	
	# parse the string
	print parseBlock(filestr)


if __name__ == '__main__':
	performTest('test1.py')
