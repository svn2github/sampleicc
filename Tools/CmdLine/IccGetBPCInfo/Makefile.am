## Process this file with automake to produce Makefile.in

bin_PROGRAMS = \
	iccGetBPCInfo

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/IccProfLib

LDADD = -L$(top_builddir)/IccProfLib -lSampleICC

iccGetBPCInfo_SOURCES = \
	iccGetBPCInfo.cpp
