# Standard stuff.
#
OCFAARCH_TOP_DIR=../..
LIBS=
PROJECT=caseadmin
REMOVEITEM_EXEC=removeitem
#SUBDIR=.

include ../../global.makeinfo
INCFLAGS+= -I$(INCPQDIR) -I/usr/local/blah -I$(BOOSTINCDIR)
GCCFLAGS += $(GCCFLAGS_ADM)
#GCCFLAGS += $(GCCFLAGS_STRICT)

INSTALL_DIR=$(INSTALL_BIN)
#
# version number this is used to create some directoreis
#

#
OBJFILESLIB=$(BUILDDIR)/PrintConfig.lo
REMOVEITEMOBJFILES=$(BUILDDIR)/RemoveItem.lo
DBMETHODFILES=$(BUILDDIR)/DbMethods.lo
HIDEITEM_EXEC=hideitem
HIDEITEMOBJFILE=$(BUILDDIR)/HideItemMain.lo
DELETEHIDDENOBJFILES=$(BUILDDIR)/DeleteHidden.lo
DELETEHIDDEN_EXEC=deletehidden

PRINTCONFIG_EXEC=printconfig



install: all
	install -d $(INSTALL_DIR)
	install -d $(INSTALL_ETC)	
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PRINTCONFIG_EXEC) $(INSTALL_DIR)
	$(LIBTOOL) --mode=install cp scripts/*.sh $(INSTALL_DIR)
	$(LIBTOOL) --mode=install cp scripts/*.pl $(INSTALL_DIR)
	$(LIBTOOL) --mode=install cp scripts/*.py $(INSTALL_DIR)
	$(LIBTOOL) --mode=install cp scripts/go_o* $(INSTALL_DIR)
	$(LIBTOOL) --mode=install cp etc/template.conf* $(INSTALL_ETC)
	$(LIBTOOL) --mode=install cp etc/*.sql $(INSTALL_ETC)
	$(LIBTOOL) --mode=install cp etc/javamodules.example $(INSTALL_ETC)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(REMOVEITEM_EXEC) $(INSTALL_DIR)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(HIDEITEM_EXEC) $(INSTALL_DIR)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(DELETEHIDDEN_EXEC) $(INSTALL_DIR)

# the lib file contains the name of the library that should be created.
# the libs which should be linked with our object files
#
#The flags that are used when compiling.
#GCCFLAGS=-pedantic -ansi
#GCCFLAGS=-Wall -pedantic-errors -Wno-long-long

#
#Uncomment one of the folowing to select an architecture
$(PROJECT): printconfig removeitem hideitem deletehidden

printconfig: $(OBJFILESLIB) 
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(PRINTCONFIG_EXEC) $(OBJFILESLIB) $(OCFALIBS)


removeitem: $(REMOVEITEMOBJFILES) 	
	$(LIBTOOL) --tag=CPP --mode=link g++ -lpq $(LIBPQDIR) -o $(BUILDDIR)/$(REMOVEITEM_EXEC) $(REMOVEITEMOBJFILES) $(OCFALIBS)

dbmethods: $(DBMETHODFILES)

hideitem: $(HIDEITEMOBJFILE) dbmethods
	$(LIBTOOL) --tag=CPP --mode=link g++ -lpq $(LIBPQDIR) -o $(BUILDDIR)/$(HIDEITEM_EXEC) $(HIDEITEMOBJFILE) $(DBMETHODFILES) $(OCFALIBS)

deletehidden: $(DELETEHIDDENOBJFILES) dbmethods
	$(LIBTOOL) --tag=CPP --mode=link g++ -lpq $(LIBPQDIR) -o $(BUILDDIR)/$(DELETEHIDDEN_EXEC) $(DELETEHIDDENOBJFILES) $(DBMETHODFILES) $(OCFALIBS)
