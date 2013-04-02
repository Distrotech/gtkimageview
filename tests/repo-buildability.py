import os
import os.path
import shutil
import sys
import tempfile

TMP_PATH = '/tmp/'
REPO_PATH = 'http://publicsvn.bjourne.webfactional.com/gtkimageview'
PROJ_NAME = 'gtkimageview'
TEST_COMMANDS = ['./autogen.sh --enable-gtk-doc',
                 'make check',

                 # And compile again with -O3 for good measure (#17)
                 'make clean',
                 'CFLAGS=-O3 ./configure',
                 'make check',
                 'python tests/alltests.py',

                 # Compile a third time without optimization to catch
                 # int to double comparision errors.
                 'make clean',
                 'CFLAGS=-O0 ./configure',
                 'make check',
                 'python tests/alltests.py',

                 # Try building the automake-disted file using waf.
                 './configure --enable-gtk-doc',
                 'make dist',
                 'tar xvzf gtkimageview-*.tar.gz',
                 'cd gtkimageview-*/ && ./waf configure --enable-gtk-doc',
                 'cd gtkimageview-*/ && ./waf build',

                 # Check that waf puts the HTML docs in the disted
                 # tarball.
                 './waf configure --enable-gtk-doc',
                 './waf build',
                 './waf dist',
                 'tar tf gtkimageview-*.tar.gz | egrep "gtkimageview-(.*)/docs/reference/html/index.html" >/dev/null',

                 'python tests/repo-buildability.py']
                 

def run_or_die(cmd):
    print 'RUNNING %s in %s.' % (cmd, os.getcwd())
    ret = os.system(cmd)
    if ret != 0:
        print '*** The command "%s" FAILED ***' % cmd
        sys.exit(1)

if __name__ == '__main__':
    s = 'This script tests the buildability of %s' % PROJ_NAME
    print s
    print '=' * len(s)
    print 'Do you want to continue? [Y/N]'
    if raw_input().lower() != 'y':
        sys.exit(1)
    
    dir = tempfile.mkdtemp(prefix = TMP_PATH)
    os.chdir(dir)
    run_or_die('svn co %s' % REPO_PATH)
    os.chdir(PROJ_NAME)
    for command in TEST_COMMANDS:
        run_or_die(command)
    shutil.rmtree(dir)
