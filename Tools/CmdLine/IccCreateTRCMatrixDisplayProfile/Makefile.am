## Process this file with automake to produce Makefile.in

bin_PROGRAMS = \
	iccCreateTRCMatrixDisplayProfile

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/IccProfLib \
	-I$(top_srcdir)/Tools/CmdLine/IccToolUtilsLib

LDADD = -L$(top_builddir)/IccProfLib -lSampleICC \
	-L$(top_builddir)/Tools/CmdLine/IccToolUtilsLib -lSampleICCToolUtils

iccCreateTRCMatrixDisplayProfile_SOURCES = \
	iccCreateTRCMatrixDisplayProfile.cpp
