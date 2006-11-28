## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libSampleICC.la

libSampleICC_la_SOURCES = IccCmm.cpp IccIO.cpp IccProfile.cpp \
	IccMpeACS.cpp IccMpeBasic.cpp IccMpeFactory.cpp IccTagMPE.cpp \
	IccTagBasic.cpp IccTagFactory.cpp IccTagLut.cpp IccUtil.cpp \
	md5.cpp

libSampleICC_la_LDFLAGS = -version-info @LIBTOOL_VERSION@

libSampleICCincludedir = $(includedir)/SampleICC

libSampleICCinclude_HEADERS = IccCmm.h IccDefs.h IccIO.h \
	IccProfLibConf.h IccProfile.h \
	IccMpeACS.h IccMpeBasic.h IccMpeFactory.h IccTagMPE.h \
	IccTag.h IccTagBasic.h IccTagFactory.h IccTagLut.h \
	IccUtil.h MainPage.h \
	icProfileHeader.h \
	md5.h

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/SampleICC
