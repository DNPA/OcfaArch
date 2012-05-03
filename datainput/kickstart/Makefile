# Standard stuff.
#
SHELL=/bin/sh
INSTALL=install
OCFAARCH_TOP_DIR=../..
include ../../global.makeinfo
GCCFLAGS += $(GCCFLAGS_KICK)
INCFLAGS+= -I$(INCPQDIR)
#GCCFLAGS += $(GCCFLAGS_STRICT)
#
# version number this is used to create some directories
#
VERSION=0.5
#The project is the name of the sub project of the washing machine.
#
PROJECT=kickstart
PROJECT2=eekickstart
PROJECT3=cp-kickstart-ll
PROJECT4=kickstart2
PROJECT5=kicktree

#The buildFiles define the object files that should be build. When you want to 
# compile a new file, you should add it to this list. Because it is added here, Make will
# automatically start looking for a similar file in the $(SRC) directory. 
#
OBJFILESLIB=$(BUILDDIR)/kickstart.lo 
OBJFILESLIB2=$(BUILDDIR)/eekickstart.lo
OBJFILESLIB3=$(BUILDDIR)/cp-kickstart-ll.lo
OBJFILESLIB4=$(BUILDDIR)/kickstart2.lo
OBJFILESLIB5=$(BUILDDIR)/kicktree.lo

# the lib file contains the name of the library that should be created.

MODULEEXEC=$(PROJECT)
MODULEEXEC2=$(PROJECT2)
MODULEEXEC3=$(PROJECT3)
MODULEEXEC4=$(PROJECT4)
MODULEEXEC5=$(PROJECT5)


# the libs which should be linked with our object files
# 
LIBS= $(INSTALLIB)/libocfafs.la $(INSTALLIB)/libocfaevidence.la $(INSTALLIB)/libUnixFileSystem.la $(OCFALIBS)
#The flags that are used when compiling.
#GCCFLAGS=-pedantic -ansi
#GCCFLAGS=-Wall -pedantic-errors -Wno-long-long
#
#Uncomment one of the folowing to select an architecture
all: $(OBJFILESLIB) $(OBJFILESLIB2) $(OBJFILESLIB3) $(OBJFILESLIB5) 
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(MODULEEXEC) $(OBJFILESLIB) $(LIBS) -locfafacade -locfamodule -locfatreegraph
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(MODULEEXEC2) $(OBJFILESLIB2) $(LIBS) -locfafacade -locfamodule -locfatreegraph
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(MODULEEXEC3) $(OBJFILESLIB3) $(LIBS) -lpq -locfafacade -locfamodule -locfatreegraph
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(MODULEEXEC5) $(OBJFILESLIB5) $(LIBS) -locfafacade -locfamodule -locfatreegraph
 
#	$(LIBTOOL) --mode=link g++ -g $(LIBS) -L/usr/local/BerkeleyDB.4.1/lib/ -o $(BUILDDIR)/digest ../../../ipc/build/libocfa.la# $(BUILDDIR)/digest.lo
#
# Target that creates the executable for the tests.
#
# Target that automagically creates documentation in html and man pages for the source
# in this project.
#

install: all
	install -d $(INSTALL_BIN)
	$(LIBTOOL)  --mode=install cp $(BUILDDIR)/$(MODULEEXEC) $(INSTALL_BIN)
	$(LIBTOOL)  --mode=install cp $(BUILDDIR)/$(MODULEEXEC2) $(INSTALL_BIN)
	$(LIBTOOL)  --mode=install cp $(BUILDDIR)/$(MODULEEXEC3) $(INSTALL_SBIN)
	$(LIBTOOL)  --mode=install cp $(BUILDDIR)/$(MODULEEXEC5) $(INSTALL_BIN)
	cp scripts/cp-kickstart $(INSTALL_BIN)
	chgrp ocfa $(INSTALL_BIN)/$(MODULEEXEC)
	chgrp ocfa $(INSTALL_BIN)/$(MODULEEXEC2)
	chgrp ocfa $(INSTALL_SBIN)/$(MODULEEXEC3)
	chgrp ocfa $(INSTALL_BIN)/$(MODULEEXEC5)
	chmod g+s $(INSTALL_BIN)/$(MODULEEXEC)
	chmod g+s $(INSTALL_BIN)/$(MODULEEXEC2)
	chmod g+s $(INSTALL_SBIN)/$(MODULEEXEC3)
	chmod g+s $(INSTALL_BIN)/$(MODULEEXEC5)	

uninstall:
	rm -r $(BINDIR)/$(MODULEEXEC)
	rm -r $(BINDIR)/$(MODULEEXEC2)
	rm -r $(BINDIR)/$(MODULEEXEC3)
	rm -r $(BINDIR)/$(MODULEEXEC5)
	rm -r $(HTML_DOCDIR)


dpkg:  all
	install -d $(PWD)/build/debian/$(INSTALL_BIN)
	libtool --mode=install install build/kicktree $(PWD)/build/debian/$(INSTALL_BIN)/
	install -d build/debian/DEBIAN
	cp pkg/debian/control build/debian/DEBIAN/
	install -d build/debian/usr/share/doc/kicktree
	cp ../../LICENCE build/debian/usr/share/doc/kicktree/copyright
	echo "See ocfa.sourceforge.net for details" > build/debian/usr/share/doc/kicktree/changelog
	echo "See ocfa.sourceforge.net for details" >  build/debian/usr/share/doc/kicktree/changelog.Debian
	gzip --best build/debian/usr/share/doc/kicktree/*
	cd build;fakeroot dpkg-deb --build debian
	install -d ../debian
	mv build/debian.deb ../../debian/ocfa-kicktree_$(RELEASE)_amd64.deb
	rm -rf build/debian
	

