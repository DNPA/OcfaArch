OCFAARCH_TOP_DIR=../

#PROJECT=anycast

#
# The main  installation directory in which all stuff will be put.
# shared library files will be put in subdir lib, executables in subdir lib
# and executables that should be executed from other executables in libexec
# according to GNU standards this should be called prefix 
#

#
#Installation dir of shared directories.
#
include ../global.makeinfo
#GCCFLAGS += -Werror
GCCFLAGS += $(GCCFLAGS_ANYCAST)
#GCCFLAGS += $(GCCFLAGS_STRICT)
#
#The project is the name of the sub project of the washing machine.
#
PROJECT=anycastrelay

#
# place where the html doc for this project is put when installing.
HTML_DOCDIR=$(HTML_PREFIX)/$(PROJECT)

#
# place where html doc is put when created.
LOCAL_HTML=doc/generated/html

#
# defines the location of doxygen
DOXYGEN=/usr/local/bin/doxygen

#
#The buildFiles define the object files that should be build. When you want to 
# compile a new file, you should add it to this list. Because it is added here, Make will
# automatically start looking for a similar file in the $(SRC) directory. 
#
OBJFILESLIB=$(BUILDDIR)/main.lo $(BUILDDIR)/anycast.lo $(BUILDDIR)/msgidbroker.lo $(BUILDDIR)/MonitorMessageFactory.lo

# INCDIR contains the header files.
# $(ACEINCDIR) should be in makeinfo-dir
INCFLAGS+=-I../ppq/inc  -I$(ACEINCDIR)
PPQLIB= ../ppq/build/libPersistentQueue.la

# the lib file contains the name of the library that should be created.

MODULEEXEC=$(PROJECT)

# the libs which should be linked with our object files
# 
LIBS=$(OCFALIBS) $(PPQLIB)
#
#The flags that are used when compiling.
#GCCFLAGS=-pedantic -ansi
#GCCFLAGS=-Wall -pedantic-errors -Wno-long-long
#
#Uncomment one of the folowing to select an architecture
all: $(OBJFILESLIB) 
	echo hoi! gcc flags are $(GCCFLAGS)
	$(LIBTOOL) --tag=CPP --mode=link g++ $(LIBS) $(GCCFLAGS) -o $(BUILDDIR)/$(MODULEEXEC)  $(OBJFILESLIB) 
 
# This rules tells Make that in order to make an object file, it should
# compile the file with the corresponding name in the src directory.
#
$(BUILDDIR)/%.lo: $(SRCDIR)/%.cpp
	if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR);fi
	$(LIBTOOL) --tag=CPP --mode=compile g++ -g -c $(GCCFLAGS) $(INCFLAGS) -I$(BOOSTINCDIR) -o $@ $<


#	$(LIBTOOL) --mode=compile g++ -g $(LIBS) -L/usr/local/BerkeleyDB.4.1/lib/ -o $(BUILDDIR)/digest ../../../ipc/build/libocfa.la# $(BUILDDIR)/digest.lo
#
# Target that creates the executable for the tests.
#
# Target that automagically creates documentation in html and man pages for the source
# in this project.

doc: all
	$(DOXYGEN)


install: all
	install -d $(INSTALL_SBIN)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(MODULEEXEC) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(MODULEEXEC)
	chmod g+s $(INSTALL_SBIN)/$(MODULEEXEC)

uninstall:
	rm -r $(SBINDIR)/$(MODULEEXEC)
	rm -r $(HTML_DOCDIR)



dpkg: all 
	install -d $(PWD)/build/debian/$(INSTALL_SBIN)
	libtool --mode=install install build/anycastrelay $(PWD)/build/debian/$(INSTALL_SBIN)/
	install -d build/debian/DEBIAN
	cp pkg/debian/control build/debian/DEBIAN/
	install -d build/debian/usr/share/doc/anycastrelay
	cp ../LICENCE build/debian/usr/share/doc/anycastrelay/copyright
	echo "See ocfa.sourceforge.net for details" > build/debian/usr/share/doc/anycastrelay/changelog
	echo "See ocfa.sourceforge.net for details" >  build/debian/usr/share/doc/anycastrelay/changelog.Debian
	gzip --best build/debian/usr/share/doc/anycastrelay/*
	cd build;fakeroot dpkg-deb --build debian
	install -d ../debian
	mv build/debian.deb ../debian/ocfa-anycastrelay_$(RELEASE)_amd64.deb
	rm -rf build/debian
	

