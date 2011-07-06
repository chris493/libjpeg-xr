#
#  Automake Makefile for the JPEG library
#
#  This file is written by Bob Friesenhahn, bfriesen@simple.dallas.tx.us
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.9 dist-zip dist-bzip2 foreign 

# Sources to build library
LIBSOURCES = 	jxrapi.c jxrparsefile.c jxrparsedir.c jxrparseimg.c \
			jdatasrc.c jdmaster.c jerror.c jutils.c jmemmgr.c \
			@MEMORYMGR@.c

# System dependent sources
SYSDEPSOURCES = jmemansi.c jmemname.c jmemnobs.c jmemdos.c jmemmac.c

# Headers which are installed to support the library
INSTINCLUDES  = jerror.h jmorecfg.h jpegxrlib.h

# Headers which are not installed
OTHERINCLUDES = cderror.h cdjpeg-xr.h jinclude.h \
        jmemsys.h jpegint.h jversion.h

# Manual pages (Automake uses 'MANS' for itself)
DISTMANS= djpeg-xr.1

# Other documentation files
DOCS= README

# Makefiles for various systems
MKFILES= configure

# Configuration files
CONFIGFILES= jconfig.h.in

# Miscellaneous support files
OTHERFILES= jconfig.doc ckconfig.c

# Test support files
TESTFILES= testorig.jpg testimg.ppm testimg.bmp

# libtool libraries to build
lib_LTLIBRARIES = libjpeg-xr.la

# Library sources for libjpeg-xr.la
libjpeg_xr_la_SOURCES = $(LIBSOURCES)

# LDFLAGS for libjpeg-xr.la
libjpeg_xr_la_LDFLAGS = -no-undefined \
	-version-info $(JPEGXR_LIB_VERSION)

# Conditionally-build library sources for libjpeg-xr.la
EXTRA_libjpeg_xr_la_SOURCES = $(SYSDEPSOURCES)

# Executables to build
bin_PROGRAMS = djpeg-xr

# Executable sources & libs
djpeg_xr_SOURCES = djpeg-xr.c cdjpeg-xr.c 
djpeg_xr_LDADD   = libjpeg-xr.la $(PNGLIBS)

# Manual pages to install
man_MANS = $(DISTMANS)

# Headers to install
include_HEADERS = $(INSTINCLUDES)

# Other distributed headers
noinst_HEADERS = $(OTHERINCLUDES)

# Other distributed files
EXTRA_DIST =  $(DOCS) $(DISTMANS) $(MKFILES) $(CONFIGFILES) $(SYSDEPSOURCES) \
        $(OTHERFILES) $(TESTFILES)

# Files to be cleaned
CLEANFILES = testout.ppm testout.bmp testout.jpg testoutp.ppm

# Install jconfig_api.h
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(includedir)
	$(INSTALL_HEADER) jconfig_api.h $(DESTDIR)$(includedir)/jconfig.h

# Uninstall jconfig_api.h
uninstall-local:
	rm -f $(DESTDIR)$(includedir)/jconfig.h

# Non-Automake subdirectories to distribute
DISTDIRS = m4 config
dist-hook:
	( \
	  builddir=`pwd` ; \
	  cd $(srcdir) && \
	  ( \
	    for dir in $(DISTDIRS) ; do \
	      find $$dir -depth -print | egrep -v '(~$$)|(/CVS)|(/\.#)|(/\.deps)' \
	        | cpio -pdum $$builddir/$(distdir) 2> /dev/null ; \
	    done \
	  ) \
	)

# Run tests
test: check-local
check-local:
	$(RM) testout*
	./djpeg-xr -dct int -ppm -outfile testout.ppm  $(srcdir)/testorig.jpg
	./djpeg-xr -dct int -bmp -colors 256 -outfile testout.bmp  $(srcdir)/testorig.jpg
	cmp $(srcdir)/testimg.ppm testout.ppm
	cmp $(srcdir)/testimg.bmp testout.bmp
