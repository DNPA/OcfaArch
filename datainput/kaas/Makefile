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
PROJECT=kaas

#The buildFiles define the object files that should be build. When you want to 
# compile a new file, you should add it to this list. Because it is added here, Make will
# automatically start looking for a similar file in the $(SRC) directory. 
#
OBJFILESLIB=$(BUILDDIR)/kaas.lo

# the lib file contains the name of the library that should be created.

MODULEEXEC=$(PROJECT)


# the libs which should be linked with our object files
# 
LIBS = $(INSTALLIB)/libocfaevidence.la $(OCFALIBS)
#The flags that are used when compiling.
#GCCFLAGS=-pedantic -ansi
#GCCFLAGS=-Wall -pedantic-errors -Wno-long-long
#
#Uncomment one of the folowing to select an architecture

$(BUILDDIR)/$(MODULEEXEC): $(OBJFILESLIB) $(LIBS)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(MODULEEXEC) $(OBJFILESLIB) $(LIBS) -lPocoFoundation -lPocoNet -lPocoUtil -locfafacade -locfamodule -locfatreegraph


all: $(BUILDDIR)/$(MODULEEXEC)
 
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
	chgrp ocfa $(INSTALL_BIN)/$(MODULEEXEC)
	chmod g+s $(INSTALL_BIN)/$(MODULEEXEC)
uninstall:
	rm -r $(BINDIR)/$(MODULEEXEC)
dpkg:
	echo make dpkg not implemented for kaas
	exit 1
