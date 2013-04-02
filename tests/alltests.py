import os
import sys

red = "\033[31m"
reset = "\033[0m"
bright = "\033[1;32m"

dir = sys.path[0]
if len(sys.argv) >= 2:
    dir = sys.argv[1]

is_test = lambda x: x.startswith('test-') and not os.path.splitext(x)[1]
tests = [f for f in os.listdir(dir) if is_test(f)]
if not tests:
    print 'No tests found in "%s".' % dir
    print 'Maybe you want to run "make check" build them?'
    sys.exit()
for test in tests:
    print bright + '==', test, '==' + reset
    code = os.system(os.path.join(dir, test))
    if code:
        print (red + '*** Exited with return code ' +
               str(code) + ' ***' + reset)
