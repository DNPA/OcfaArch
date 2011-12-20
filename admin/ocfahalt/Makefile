# Standard stuff.
#
OCFAARCH_TOP_DIR=../..
LIBS=
PROJECT=ocfahalt
SUBDIR=.
include ../../global.makeinfo
GCCFLAGS += $(GCCFLAGS_HALT)
#GCCFLAGS += $(GCCFLAGS_STRICT)
INSTALL_DIR=$(INSTALL_BIN)
#
# version number this is used to create some directoreis
#

#
OBJFILESLIB=$(BUILDDIR)/OcfaHalt.lo




# the lib file contains the name of the library that should be created.
# the libs which should be linked with our object files
#
#The flags that are used when compiling.
#GCCFLAGS=-pedantic -ansi
#GCCFLAGS=-Wall -pedantic-errors -Wno-long-long

#
#Uncomment one of the folowing to select an architecture
$(PROJECT): $(OBJFILESLIB) 
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(PROJECT) $(OBJFILESLIB) $(OCFALIBS)

install: all installetc
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_DIR)

installetc:

uninstall:
	rm -r $(INSTALL_SBIN)/$(PROJECT)
	rm -r $(HTML_DOCDIR)
	
