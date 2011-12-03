import re

flatWSPat = r'[ \t]'
flatNoWSPat = r'[^ \t]'
symPat = r'[a-zA-Z_][a-zA-Z0-9_]*'
numPat = r'-?[0-9]+([,.][0-9])?'
funArgPat = r'%s*%s%s*' % (flatWSPat, symPat, flatWSPat);
funArgsPat = r'(%s(,%s*%s)*)|(%s)' % (funArgPat, flatWSPat, funArgPat, flatWSPat)
funBodyPat = r'(%s.*?\n)+'
funPat = r'def%s+(?P<name>%s)\((?P<args>%s)?\):%s*?\n' % (flatWSPat, symPat, funArgsPat, flatWSPat)

# print 'symbol pattern:\n%s\n' % symPat
# print 'numeral pattern:\n%s\n' % numPat
# print 'function argument pattern:\n%s\n' % funArgPat
# print 'function arguments pattern:\n%s\n' % funArgsPat
# print 'function body pattern:\n%s\n' % (funBodyPat % ' ')
# print 'function pattern:\n%s\n' % funPat

funDefFinder = re.compile(funPat, re.M)
flatNoWSPatFinder = re.compile(flatNoWSPat)
	
def parseBlock(str, start, end):
	while True:
		symIt = funDefFinder.search(str)
		if symIt == None:
			break
	
		# find the body
		wsb = symIt.end()
		bm = flatNoWSPatFinder.search(str[wsb:])
		if bm == None:
			body = None
			bs = (wsb, wsb)
		else:
			wse = wsb+bm.start()
			#print '  body pattern: \'%s\'' % (funBodyPat % str[wsb:wse])
			funBodyFinder = re.compile(funBodyPat % str[wsb:wse], re.M)
			bs = funBodyFinder.match(str[wsb:]).span()
			bs = (bs[0]+wsb, bs[1]+wsb)
			body = str[bs[0]:bs[1]]
		
		# present result
		print 'found function:'
		print '  name:', symIt.group('name')
		print '  arguments:', symIt.group('args')
		print '  body: \'\n', body, '\n\''
		
		str = str[bs[1]:]

def performTest(test):
	print 'Input file: \'%s\'.' % test
	
	# load file
	filestr = open(test).read()
	
	# parse the string
	parseBlock(filestr, 0, -1)

performTest('test1.py')