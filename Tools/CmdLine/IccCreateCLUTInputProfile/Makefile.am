## Process this file with automake to produce Makefile.in

bin_PROGRAMS = \
	iccCreateCLUTInputProfile \
	iccFlattenAToBTag

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/IccProfLib

LDADD = -L$(top_builddir)/IccProfLib -lSampleICC

iccCreateCLUTInputProfile_SOURCES = \
	iccCreateCLUTInputProfile.cpp \
	CAT.h CAT.cpp \
	CLUT.h CLUT.cpp

iccFlattenAToBTag_SOURCES = \
	iccFlattenAToBTag.cpp
