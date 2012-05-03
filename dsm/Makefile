# Standard stuff.
#
OCFAARCH_TOP_DIR=..
LIBS=
PROJECT=dsm
#SUBDIR=.
include ../global.makeinfo
GCCFLAGS += $(GCCFLAGS_DSM)
#GCCFLAGS += $(GCCFLAGS_STRICT)
#
# version number this is used to create some directoreis
#

#
OBJFILESLIB=$(BUILDDIR)/Dsm.lo
OBJFILESLIB2 = $(BUILDDIR)/MetaFilter.lo $(BUILDDIR)/Dsm2.lo

INCFLAGS+= -I$(INCPQDIR)

# the lib file contains the name of the library that should be created.
# the libs which should be linked with our object files
#
#The flags that are used when compiling.
#GCCFLAGS=-pedantic -ansi
#GCCFLAGS=-Wall -pedantic-errors -Wno-long-long

$(PROJECT): $(OBJFILESLIB) 
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/dsm1 $(OBJFILESLIB) $(OCFALIBS) $(LIBPQDIR) -lpq -locfafacade -locfamodule -locfatreegraph

dsm2:	$(OBJFILESLIB2)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/dsm2 $(OBJFILESLIB2) $(OCFALIBS) -lpq -locfafacade -locfamodule -locfatreegraph

all:	dsm dsm2

install: all installetc
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/dsm1 $(INSTALL_DIR)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/dsm2 $(INSTALL_DIR)
	ln -sf $(INSTALL_DIR)/dsm2 $(INSTALL_DIR)/dsm
	chgrp ocfa $(INSTALL_DIR)/$(PROJECT)
	chmod g+s $(INSTALL_DIR)/$(PROJECT)

set1:	
	ln -sf $(INSTALL_DIR)/dsm1 $(INSTALL_DIR)/dsm

set2:	
	ln -sf $(INSTALL_DIR)/dsm2 $(INSTALL_DIR)/dsm

installetc:
	install -d $(INSTALL_ETC)
	cp etc/dsm.conf $(INSTALL_ETC)

uninstall:
	rm -r $(INSTALL_SBIN)/$(PROJECT)
	rm -r $(HTML_DOCDIR)
		

dpkg: all 
	install -d $(PWD)/build/debian/$(INSTALL_SBIN)
	libtool --mode=install install build/dsm1 $(PWD)/build/debian/$(INSTALL_SBIN)/
	libtool --mode=install install build/dsm2 $(PWD)/build/debian/$(INSTALL_SBIN)/
	ln -s dsm2 $(PWD)/build/debian/$(INSTALL_SBIN)/dsm
	install -d build/debian/DEBIAN
	cp pkg/debian/control build/debian/DEBIAN/
	install -d build/debian/usr/share/doc/ocfadsm
	cp ../LICENCE build/debian/usr/share/doc/ocfadsm/copyright
	echo "See ocfa.sourceforge.net for details" > build/debian/usr/share/doc/ocfadsm/changelog
	echo "See ocfa.sourceforge.net for details" >  build/debian/usr/share/doc/ocfadsm/changelog.Debian
	gzip --best build/debian/usr/share/doc/ocfadsm/*
	cd build;fakeroot dpkg-deb --build debian
	install -d ../debian
	mv build/debian.deb ../debian/ocfa-dsm_$(RELEASE)_amd64.deb
	rm -rf build/debian
	

