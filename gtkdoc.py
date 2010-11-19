# -*- coding: utf-8 -*-
'''
This module is a waf tool for building GTKDoc documentation.

Usage
=====
The tool must be checked for in the `configure` function:

.. python::

    def configure(conf):
        ...
        conf.check_tool('gtkdoc', tooldir = '.')
        ...

Then a gtkdoc task must be defined that takes the library to document
and extra SGML files as parameters:

.. python::

    (in a wscript_build)
    doctask = bld.new_task_gen(features = 'gtkdoc')
    doctask.library = libobj
    doctask.sgml_files = ['main.sgml',
                          'first_chapter.sgml',
                          'second_chapter.sgml']

`libobj` is a cc task for building a c library. The gtkdoc task uses
it to find the sources to document and also to set the correct CFLAGS
and LDFLAGS for gtkdoc_scangobj_task.

`sgml_files` is a list of SGML files used to create the non-generated
part of the documentation. The first item in that list is considered
the main SGML file and will be used to create the root page of the
documentation.

This tool also provides the --enable-gtk-doc configure option. When
turned on, gtk-doc 1.10 or higher will be checked for and all
documentation targets built. If the option is not specified, all
documentation targets are skipped.

Refer to the GTKDoc documentation for the documentation syntax. The
HTML manual is placed in build/$variant/$path/html.

:autor: `Björn Lindqvist <bjourne@gmail.com>`__
:todo: Find out how to not run gtkdoc_scangobj_task if the documented
    library does not use GObject.
'''
import Options
import Task
import TaskGen
import Utils
import os
from os.path import exists, isdir, join
import pproc
import shutil

def run_proc(cmd, cwd, env = None):
    '''
    Run the specified command with working directory cwd until
    completion and return its returncode. The command is run in a
    sub-shell.

    @param cmd: command to run.
    @param cwd: working directory for command.
    @param env: process environment to use.
    @return: return code of command.
    '''
    proc = pproc.Popen(cmd, shell = True, env = env, cwd = cwd)
    proc.communicate()
    return proc.returncode

def detect(conf):
    conf.env['BUILD_GTKDOC'] = Options.options.enable_gtk_doc
    if conf.env['BUILD_GTKDOC']:
        # I would like to be able to specify somehow that the gtkdoc tool
        # depends on compiler_cc but I don't know how.
        conf.check_cfg(package = 'gtk-doc',
                       mandatory = True,
                       atleast_version = '1.10')

def set_options(opt):
    docopts = opt.add_option_group(
        'GTKDoc Options',
        'These settings control GTKDoc documentation settings.')
    docopts.add_option(
        '--enable-gtk-doc',
        action = 'store_true',
        default = False,
        help = 'Build GTKDoc documentation [default: %default]')

def get_nodes_of_file(parent, file, env):
    '''
    Return a list of nodes for the file in the parent directory. If
    file is a regular file in the directory, then a list with one node
    representing the file is returned. If it is a directory, then one
    node for each file in that directory is returned.

    @param parent: a directory node.
    @param file: a file or directory name.
    @param env: a waf environment.
    @return: a list of nodes for the file.
    '''
    abspath = join(parent.abspath(env), file)
    if isdir(abspath):
        dirnode = parent.ensure_dir_node_from_path(file)
        dirfiles = Utils.listdir(abspath)
        return [dirnode.exclusive_build_node(f) for f in dirfiles]
    return [parent.exclusive_build_node(file)]

class GTKDocBaseTask(Task.Task):
    '''
    Base class for GTKDoc tasks. It is mostly used for the gen_files
    system so that the tasks outputs are set correctly in post_run.
    '''
    gen_files = []
    '''
    List of files the task builds. ${libname} expands to the name of
    the library being documented.
    '''
    run_command = ''
    quiet = True
    '''
    All GTKDoc tasks must be run quiet or waf complains about missing
    outputs.
    '''
    def scan(self):
        '''
        Rebuild is triggered when any of the inputs is changed. This
        should be waf default.
        '''
        return (self.inputs, [])

    def post_run(self):
        parent = self.inputs[0].parent
        d = {'libname' : self.libname}
        built_files = [Utils.subst_vars(f, d) for f in self.gen_files]
        for file in built_files:
            self.outputs.extend(get_nodes_of_file(parent, file, self.env))
        return Task.Task.post_run(self)

class gtkdoc_scan_task(GTKDocBaseTask):
    gen_files = ['${libname}-decl.txt',
                 '${libname}-decl-list.txt',
                 '${libname}-overrides.txt',
                 '${libname}-sections.txt',
                 '${libname}.types']
    after = 'cc_link'

    def run(self):
        parent_node = self.inputs[0].parent
        srcdir = parent_node.abspath()
        dstdir = parent_node.abspath(self.env)
        fmt = ('gtkdoc-scan ' +
               '--module=%s ' +
               '--source-dir=%s ' +
               '--rebuild-types ' +
               '--rebuild-sections')
        return run_proc(fmt % (self.libname, srcdir), dstdir)

class gtkdoc_scangobj_task(GTKDocBaseTask):
    gen_files = ['${libname}.args',
                 '${libname}.hierarchy',
                 '${libname}.interfaces',
                 '${libname}.prerequisites',
                 '${libname}.signals']
    after = 'gtkdoc_scan_task'

    def run(self):
        parent_node = self.inputs[0].parent
        dstdir = parent_node.abspath(self.env)

        # gtkdoc compiles a scanner program that does some
        # introspective magic stuff on GObjects. To be able to do
        # that, the gtkdoc-scangobj program has to be supplied correct
        # flags in the environment variables CFLAGS, LDFLAGS and RUN.
        cflags = 'CCFLAGS', 'CPPFLAGS', '_CCINCFLAGS', '_CCDEFFLAGS'
        cflag_values = ' '.join(self.env.get_flat(flag) for flag in cflags)

        ldflags = 'LINKFLAGS',
        ldflags_values = ' '.join(self.env.get_flat(flag) for flag in ldflags)

        # For some reason this linkflag must be removed, or the
        # compiled scanner will segfault.
        ldflags_values = ldflags_values.replace('-shared', '')

        # Have to link with the library itself. By putting the current
        # directory first, link will always be done with the local so
        # and not an installed variant.
        ldflags_values = '-L. ' + ldflags_values + ' -l' + self.libname

        proc_env = os.environ.copy()
        proc_env['CFLAGS'] = cflag_values
        proc_env['LDFLAGS'] = ldflags_values
        # It is neccesarry to set the link path, otherwise the scanner
        # won't find the library it is trying to introspect.
        proc_env['RUN'] = 'LD_LIBRARY_PATH=. sh -c'

        fmt = 'gtkdoc-scangobj --module=%s'
        return run_proc(fmt % self.libname, dstdir, proc_env)

class gtkdoc_mktmpl_task(GTKDocBaseTask):
    after = 'gtkdoc_scangobj_task'
    gen_files = ['${libname}-unused.txt', 'tmpl.stamp', 'tmpl']

    def run(self):
        parent_node = self.inputs[0].parent
        dstdir = parent_node.abspath(self.env)
        fmt = 'gtkdoc-mktmpl --module=%s'
        return run_proc(fmt % self.libname, dstdir)

class gtkdoc_mkdb_task(GTKDocBaseTask):
    '''
    Creates the XML files.
    '''
    after = 'gtkdoc_mktmpl_task'
    gen_files = ['sgml.stamp',
                 '${libname}-undeclared.txt',
                 '${libname}-undocumented.txt',
                 'xml']

    def run(self):
        parent = self.inputs[0].parent
        srcdir = parent.abspath()
        dstdir = parent.abspath(self.env)
        fmt = ('gtkdoc-mkdb ' +
               '--module=%s ' +
               '--source-dir=%s ' +
               '--output-format=xml ' +
               '--expand-content-files="" ' +
               '--main-sgml-file=%s ' +
               '--sgml-mode')
        return run_proc(fmt % (self.libname,
                               srcdir,
                               self.sgml_nodes[0].abspath()),
                        dstdir)

class gtkdoc_mkhtml_task(GTKDocBaseTask):
    '''
    Builds the HTML using gtkdoc-mkhtml.
    '''
    after = 'gtkdoc_mkdb_task'
    gen_files = ['html.stamp', 'html']

    def run(self):
        parent = self.inputs[0].parent
        parent_abs = parent.abspath(self.env)
        srcdir = parent.abspath()

        # gtkdoc-mkhtml must be run in the html directory. It behaves
        # differently from gtkdoc-mkdb, gtkdoc-mktmpl etc.
        dstdir = join(parent_abs, 'html')
        if not exists(dstdir):
            os.mkdir(dstdir)

        # The non-generated SGML files has to be copied to the build
        # directory. Otherwise paths will be incorrect.
        for node in self.sgml_nodes:
            shutil.copy(node.abspath(), parent_abs)
            # List it as a generated file.
            self.gen_files.append(node.name)

        # Copy the image files. They don't need to be added to the
        # gen_files list because they are in the html directory.
        for node in self.image_nodes:
            shutil.copy(node.abspath(), dstdir)

        fmt = 'gtkdoc-mkhtml %s %s'
        cmd = fmt % (self.libname, join(parent_abs, self.sgml_nodes[0].name))
        return run_proc(cmd, dstdir)

    def install(self):
        parent = self.inputs[0].parent
        parent_abs = parent.abspath(self.env)
        htmldir = join(parent_abs, 'html')
        dstdir = '${DATADIR}/gtk-doc/html/%s' % self.libname
        install_files = [join(htmldir, f) for f in os.listdir(htmldir)]
        self.generator.bld.install_files(dstdir, install_files)

class gtkdoc_fixxref_task(GTKDocBaseTask):
    after = 'gtkdoc_mkhtml_task'

    def run(self):
        fmt = ('gtkdoc-fixxref ' +
               '--module-dir=html ' +
               '--html-dir=html')
        rundir = self.inputs[0].parent.abspath(self.env)
        return run_proc(fmt, rundir)

@TaskGen.feature('gtkdoc')
def process_gtkdoc(self):
    if not hasattr(self, 'sgml_files') or not self.sgml_files:
        raise Utils.WafError('No SGML sources specified.')
    if not hasattr(self, 'library'):
        raise Utils.WafError('Library to document not specified.')

    env = self.library.env.copy()
    if not env['BUILD_GTKDOC']:
        return
    
    if 'cshlib' in self.library.features:
        env.append_value('LIB', self.library.target)

    sgml_nodes = []
    for f in self.sgml_files:
        node = self.path.find_resource(f)
        if not node:
            raise Utils.WafError('SGML file %s not found' % f)
        sgml_nodes.append(node)

    image_nodes = []
    for f in getattr(self, 'image_files', []):
        node = self.path.find_resource(f)
        if not node:
            raise Utils.WafError('Image file %s not found' % f)
        image_nodes.append(node)
        
    sources = [self.path.find_resource(f) for f in self.library.source]

    tasknames = ['gtkdoc_scan',
                'gtkdoc_scangobj',
                'gtkdoc_mktmpl',
                'gtkdoc_mkdb',
                'gtkdoc_mkhtml',
                'gtkdoc_fixxref']
    tasks = {}
    for taskname in tasknames:
        task = self.create_task(taskname, env)
        task.set_inputs(sources)
        task.libname = self.library.target
        tasks[taskname] = task
    # These task depend on the SGML files.
    for taskname in 'gtkdoc_mkdb', 'gtkdoc_mkhtml':
        tasks[taskname].sgml_nodes = sgml_nodes
        tasks[taskname].set_inputs(sgml_nodes + sources)

    # The HTML task depends on the image files.
    tasks['gtkdoc_mkhtml'].set_inputs(sgml_nodes + sources + image_nodes)
    tasks['gtkdoc_mkhtml'].image_nodes = image_nodes
