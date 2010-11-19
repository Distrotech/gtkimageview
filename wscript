#! /usr/bin/env python
# encoding: utf-8
import Logs
import Options
import os
import os.path
import pproc
import shutil
import tarfile

VERSION='1.6.4'
APPNAME='gtkimageview'

srcdir = '.'
blddir = 'build'

def copy_files(files, todir):
    for file in files:
        if os.path.isdir(file):
            continue
        dstdir = os.path.dirname(os.path.join(todir, file))
        if not os.path.exists(dstdir):
            os.mkdir(dstdir)
        shutil.copy2(file, dstdir)

def missing_files(files):
    return [f for f in files if not os.path.exists(f)]

def run_proc_logged(cmd):
    proc = pproc.Popen(cmd, shell = True)
    proc.communicate()
    if proc.returncode != 0:
        Logs.error('Running %s failed' % cmd)
    return proc.returncode

def dist():
    tmp_dir = APPNAME + '-' + VERSION
    archive = tmp_dir + '.tar.gz'

    if os.path.exists(tmp_dir):
        shutil.rmtree(tmp_dir)
    if os.path.exists(archive):
        os.remove(archive)
    os.mkdir(tmp_dir)

    Logs.info('Getting file list from Subversion...')
    proc = pproc.Popen('svn list -R', shell = True, stdout = pproc.PIPE)
    svn_files = proc.communicate()[0].split('\n')
    if proc.returncode != 0:
        Logs.error('Failed creating dist.')
        return

    Logs.info('Copying Subversion files...')
    # Filter out non-files.
    svn_files = [file for file in svn_files if os.path.isfile(file)]
    copy_files(svn_files, tmp_dir)

    # The junk that autotools needs.
    extra_dist = ['aclocal.m4',
                  'config.guess',
                  'config.sub',
                  'configure',
                  'configure.in',
                  'depcomp',
                  'gtk-doc.make',
                  'install-sh',
                  'ltmain.sh',
                  'missing',
                  'Makefile.in',
                  'docs/Makefile.in',
                  'docs/reference/Makefile.in',
                  'src/Makefile.in',
                  'src/gtkimageview-marshal.c',
                  'src/gtkimageview-marshal.h',
                  'src/gtkimageview-typebuiltins.c',
                  'src/gtkimageview-typebuiltins.h',
                  'tests/Makefile.in',
                  'tests/testlib/Makefile.in']

    # If any file is missing, autogen.sh needs to be run to regenerate
    # them.
    missing = missing_files(extra_dist)
    if missing:
        Logs.info('Regenerating autotools files (%s missing)...' %
                  ' '.join(missing))
        if run_proc_logged('./autogen.sh') != 0:
            return

    # If any file still is missing, then make has to be run
    # unfortunately, to regenerate it.
    missing = missing_files(extra_dist)
    if missing:
        Logs.info('Recompiling project (%s missing)...' %
                  ' '.join(missing))
        if run_proc_logged('make') != 0:
            return

    Logs.info('Copying non-versioned files...')
    copy_files(extra_dist, tmp_dir)

    Logs.info('Copying HTML documentation...')
    if not os.path.exists('build/default/src/html/index.html'):
        Logs.error('HTML docs not found. Please build docs before disting.')
        return
    shutil.copytree('build/default/src/html',
                    os.path.join(tmp_dir, 'docs/reference/html'))

    # Create the archive.
    tar = tarfile.open(archive, 'w:gz')
    tar.add(tmp_dir)
    tar.close()
    Logs.info('Your archive is ready -> %s' % archive)
    shutil.rmtree(tmp_dir)

def set_options(opt):
    opt.tool_options('compiler_cc')
    opt.tool_options('gnu_dirs')
    opt.tool_options('gtkdoc')
    buildopts = opt.add_option_group(
        'Build Configuration',
        'These settings control which extra targets to build. They only make sense with the build command.')
    buildopts.add_option('--demos',
                         action = 'store_true',
                         default = False,
                         help = 'Build demo programs')
    buildopts.add_option('--tests',
                         action = 'store_true',
                         default = False,
                         help = 'Build unit test programs')

def configure(conf):
    conf.check_tool('compiler_cc')
    conf.check_tool('gnome')
    conf.check_tool('misc')
    conf.check_cfg(package = 'gtk+-2.0',
                   uselib_store = 'GTK',
                   atleast_version = '2.18.0',
                   args = '--cflags --libs',
                   mandatory = True)
    conf.check_tool('gtkdoc', tooldir = '.')
    # Waf doesn't set the -g and -O2 flags automatically so add them
    # here.
    if not conf.env['CCFLAGS']:
        conf.env['CCFLAGS'] = ['-g', '-O2']
    flags = ['-std=c99', '-Wall', '-Werror', '-Wmissing-prototypes']
    conf.env.append_value('CCFLAGS', flags)

def build(bld):
    bld.env['PACKAGE_NAME'] = APPNAME
    bld.env['PACKAGE_VERSION'] = VERSION
    bld.add_subdirs('src')
    bld.add_subdirs('tests')

    # Build the gtkimageview.pc file.
    bld.new_task_gen('subst',
                     source = 'gtkimageview.pc.in',
                     install_path = '${LIBDIR}/pkgconfig')
