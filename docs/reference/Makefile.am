# These are compile options that will be fed to gtkdoc-scan
GTKDOC_CFLAGS = -I$(top_srcdir) $(DEP_CFLAGS) 
GTKDOC_LIBS = $(top_builddir)/src/libgtkimageview.la

# The name of the module.
DOC_MODULE = gtkimageview

# The top-level SGML file.
DOC_MAIN_SGML_FILE = gtkimageview.sgml

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS = --rebuild-sections --rebuild-types

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR = ../..

# Used for dependencies.
HFILE_GLOB = $(top_srcdir)/src/*.h
CFILE_GLOB = $(top_srcdir)/src/*.c

# Images to copy into HTML directory.
HTML_IMAGES =		    \
	$(top_srcdir)/docs/reference/images/gtkimageview-in-window.png \
	$(top_srcdir)/docs/reference/images/ex-monitor-selection.png \
	$(top_srcdir)/docs/reference/images/ex-mini.png \
	$(top_srcdir)/docs/reference/images/big-zoom-sel-bug.png \
	$(top_srcdir)/docs/reference/images/navwin-open.png

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE)
content_files =		    \
	compiling.sgml	    \
	dirstructure.sgml   \
	releasehistory.sgml \
	sourcecode.sgml

# Extra options to supply to gtkdoc-mkdb.
MKDB_OPTIONS = --sgml-mode --output-format=xml

# Extra options to supply to gtkdoc-fixxref
FIXXREF_OPTIONS =

# include common portion ...
include $(top_srcdir)/gtk-doc.make
