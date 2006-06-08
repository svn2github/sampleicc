## Process this file with automake to produce Makefile.in

bin_PROGRAMS = iccApplyProfiles

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/IccProfLib -I/opt/local/include

LDADD = -L$(top_builddir)/IccProfLib -lSampleICC -L/opt/local/lib -ltiff

iccApplyProfiles_SOURCES = iccApplyProfiles.cpp TiffImg.cpp TiffImg.h
