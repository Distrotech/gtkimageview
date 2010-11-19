SUBDIRS = testlib

LDADD =						\
	$(top_builddir)/src/libgtkimageview.la	\
	$(DEP_LIBS)				\
	./testlib/libtest.la
INCLUDES = $(DEP_CFLAGS) -I$(top_srcdir) -I.

noinst_PROGRAMS =	     \
	ex-abssize	     \
	ex-alignment	     \
	ex-anim		     \
	ex-blurpart	     \
	ex-cairo-drawing     \
	ex-mini		     \
	ex-monitor-selection \
	ex-pixbuf-changes    \
	ex-rotate	     \
	interactive	     

check_PROGRAMS =	     \
	test-anim-view	     \
	test-attributes	     \
	test-fitting	     \
	test-gdk-pixbuf-draw-cache  \
	test-gdk-utils	     \
	test-gtk-signals     \
	test-image-nav	     \
	test-keybindings     \
	test-memory	     \
	test-scrollwin	     \
	test-signals	     \
	test-size-allocation \
	test-tool-dragger    \
	test-tool-selector   \
	test-viewport	     \
	test-zoom-in-out     

EXTRA_DIST = gnome_logo.jpg wscript_build

