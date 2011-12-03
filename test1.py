def foo(x, s1, s_2, s3_, _y):
	a = 1
	if x > 0:
		a += 1
	return a

def bar():
	return 2


def baz():
	def qux():
		print 4
	
	print foo(bar(), 2, 3, 4)

