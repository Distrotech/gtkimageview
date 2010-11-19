INCLUDES = $(DEP_CFLAGS)

noinst_LTLIBRARIES = libtest.la
noinst_HEADERS = testlib.h

libtest_la_SOURCES = testlib.c

libtest_la_LIBADD = $(DEP_LIBS)
libtest_la_LDFLAGS = -no-undefined

EXTRA_DIST = wscript_build
