## Process this file with automake to produce Makefile.in

SUBDIRS = \
	IccProfLib \
	Tools \
	Contrib

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sampleicc.pc

EXTRA_DIST = bootstrap \
	README.OSX \
	ICC_utils-Info.plist \
	SampleICC-Info.plist \
	SampleICC.xcodeproj

dist-hook:
	rm -fr `find $(distdir) -name .DS_Store -or -name .svn`
