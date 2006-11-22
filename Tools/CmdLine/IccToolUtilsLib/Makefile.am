## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libSampleICCToolUtils.la

libSampleICCToolUtils_la_SOURCES = CAT.cpp CLUT.cpp CLUTStuffer.cpp \
	IccToolException.cpp

libSampleICCToolUtils_la_LDFLAGS = -version-info @LIBTOOL_VERSION@

libSampleICCToolUtilsincludedir = $(includedir)/SampleICC

libSampleICCToolUtilsinclude_HEADERS = CAT.h CLUT.h CLUTStuffer.h IccToolException.h

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/IccProfLib \
	-I$(top_srcdir)/Tools/CmdLine/IccToolUtilsLib

LDADD = -L$(top_builddir)/IccProfLib -lSampleICC

