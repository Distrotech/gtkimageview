INCLUDES = $(DEP_CFLAGS)

BUILT_SOURCES =				\
	gtkimageview-marshal.c		\
	gtkimageview-marshal.h		\
	gtkimageview-typebuiltins.c	\
	gtkimageview-typebuiltins.h

# This is the names of the libraries being built.
lib_LTLIBRARIES = libgtkimageview.la

libgtkimageview_headers =	    \
	gdkpixbufdrawcache.h	    \
	gtkimageview.h		    \
	gtkanimview.h		    \
	gtkiimagecairoshaper.h	    \
	gtkiimagetool.h		    \
	gtkimagescrollwin.h	    \
	gtkimagetoolcairo.h	    \
	gtkimagetooldragger.h	    \
	gtkimagetoolpainter.h	    \
	gtkimagetoolselector.h	    \
	gtkimagenav.h		    \
	gtkzooms.h		    \
	cursors.h		    \
	mouse_handler.h		    \
	utils.h			    

libgtkimageview_la_SOURCES =        \
	cursors.c		    \
	gdkpixbufdrawcache.c	    \
	gtkanimview.c		    \
	gtkiimagecairoshaper.c	    \
	gtkiimagetool.c		    \
	gtkimagenav.c		    \
	gtkimagescrollwin.c	    \
	gtkimagetoolcairo.c	    \
	gtkimagetooldragger.c	    \
	gtkimagetoolpainter.c	    \
	gtkimagetoolselector.c	    \
	gtkimageview.c		    \
	gtkzooms.c		    \
	mouse_handler.c		    \
	utils.c			    \
	$(BUILT_SOURCES)	    \
	$(libgtkimageview_headers)

libgtkimageview_la_LIBADD = $(DEP_LIBS)
libgtkimageview_la_LDFLAGS = -no-undefined

libgtkimageview_includedir = $(includedir)/gtkimageview
libgtkimageview_include_HEADERS =   	\
	$(libgtkimageview_headers)  	\
	gtkimageview-typebuiltins.h

gtkimageview-marshal.h: gtkimageview-marshal.list $(GLIB_GENMARSHAL)
	$(GLIB_GENMARSHAL) $< --header --prefix=gtkimageview_marshal > $@
gtkimageview-marshal.c: gtkimageview-marshal.list $(GLIB_GENMARSHAL)
	echo "#include \"gtkimageview-marshal.h\"" > $@ && \
	$(GLIB_GENMARSHAL) $< --body --prefix=gtkimageview_marshal >> $@

# Note that two invocations of glib-mkenums is needed here. This is
# because some enums use the gtk prefix while others use gdk.
gtkimageview-typebuiltins.h:
	echo -e			\
	"#ifndef __GTKIMAGEVIEW_TYPEBUILTINS_H__\n"		    \
	"#define __GTKIMAGEVIEW_TYPEBUILTINS_H__\n"		    \
	"G_BEGIN_DECLS" > $@ &&	\
	$(GLIB_MKENUMS)		\
	--fprod "/* enumerations from \"@filename@\" */"	    \
	--vhead "GType @enum_name@_get_type (void) G_GNUC_CONST;\n" \
	--vhead "#define GDK_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
	gdk*.h >> $@ &&		\
	$(GLIB_MKENUMS)		\
	--fprod "/* enumerations from \"@filename@\" */"	    \
	--vhead "GType @enum_name@_get_type (void) G_GNUC_CONST;\n" \
	--vhead "#define GTK_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
	gtk*.h >> $@ &&		\
	echo -e			\
	"G_END_DECLS\n\n"	\
	"#endif" >> $@

gtkimageview-typebuiltins.c:
	$(GLIB_MKENUMS) \
	--fhead "#include \"gtkimageview.h\"\n"	    \
	--fhead "#include \"gtkimagetoolselector.h\"\n"  \
	--fhead "#include \"gtkimageview-typebuiltins.h\"\n"	    \
	--fprod "/* enumerations from \"@filename@\" */"	    \
	--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0)  {\n    static const G@Type@Value values[] = {" \
	--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
	--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (g_intern_static_string (\"@EnumName@\"), values);\n  }\n  return etype;\n}\n" \
	gtk*.h gdk*.h > $@

EXTRA_DIST = gtkimageview-marshal.list wscript_build

CLEANFILES = $(BUILT_SOURCES)
