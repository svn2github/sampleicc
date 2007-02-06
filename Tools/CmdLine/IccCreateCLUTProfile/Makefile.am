## Process this file with automake to produce Makefile.in

bin_PROGRAMS = \
	iccCreateCLUTProfile \
	iccFlattenAToBTag

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/IccProfLib \
	-I$(top_srcdir)/Tools/CmdLine/IccToolUtilsLib

LDADD = -L$(top_builddir)/IccProfLib -lSampleICC \
	-L$(top_builddir)/Tools/CmdLine/IccToolUtilsLib -lSampleICCToolUtils

iccCreateCLUTProfile_SOURCES = \
	BlackScaler.h BlackScaler.cpp \
	iccCreateCLUTProfile.cpp

iccFlattenAToBTag_SOURCES = \
	iccFlattenAToBTag.cpp
