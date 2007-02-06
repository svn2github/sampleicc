## Process this file with automake to produce Makefile.in

SUBDIRS = deviceCodeGenerators

DIST_SUBDIRS = deviceCodeGenerators \
	examples

bin_PROGRAMS = \
	iccReconstructMeasurements

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/IccProfLib \
	-I$(top_srcdir)/Tools/CmdLine/IccToolUtilsLib

LDADD = -L$(top_builddir)/IccProfLib -lSampleICC \
	-L$(top_builddir)/Tools/CmdLine/IccToolUtilsLib -lSampleICCToolUtils

iccReconstructMeasurements_SOURCES = \
	MeasurementExtractor.h MeasurementExtractor.cpp \
	iccReconstructMeasurements.cpp
