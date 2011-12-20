# Standard stuff.
#
OCFAARCH_TOP_DIR=../..
include ../../global.makeinfo
GCCFLAGS += $(GCCFLAGS_DB)
#GCCFLAGS += $(GCCFLAGS_STRICT)
#
# version number this is used to create some directoreis
#

#
#The project is the name of the sub project of the washing machine.
#
PROJECT=db

#
# The Build Dir defines the directory in which all newly created stuff should be put.
#
BUILDDIR=build
#
#The buildFiles define the object files that should be build. When you want to 
# compile a new file, you should add it to this list. Because it is added here, Make will
# automatically start looking for a similar file in the $(SRC) directory. 
#

DBFILESLIB=$(BUILDDIR)/db.lo 

# INCDIR contains the header files.
INCFLAGS+=  -I$(INCPQDIR)

DBFILE=db.cgi

# the libs which should be linked with our object files
# 
LIBS=$(INSTALLIB)/libocfamisc.la -lcgicc -L/usr/local/pgsql/lib -lpq
 #
#The flags that are used when compiling.
#GCCFLAGS=-pedantic -ansi
#GCCFLAGS=-Wall -pedantic-errors -Wno-long-long -Woverloaded-virtual -Wundef -Wconversion -Wsign-compare -Wredundant-decls -W
#
#Uncomment one of the folowing to select an architecture
all:

$(DBFILE):  $(DBFILESLIB)
	$(LIBTOOL) --mode=link g++  -g -o  $(BUILDDIR)/$(DBFILE) $(DBFILESLIB) $(LIBS)

$(BUILDDIR)/%.lo: $(SRCDIR)/%.cpp
	if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR);fi
	$(LIBTOOL) --mode=compile g++ -g -c $(GCCFLAGS) $(INCFLAGS) -o $@ $<

install: all
	cp mkindex/meta.html $(INSTALL_HTML)/
	chgrp ocfa $(INSTALL_HTML)/meta.html
	chmod 664 $(INSTALL_HTML)/meta.html
	cp mkindex/s1.pl $(INSTALL_SBIN)/
	cp mkindex/makemetaindex.pl $(INSTALL_BIN)/
	cp mkindex/calenderview.pl $(INSTALL_SBIN)/
	cp mkindex/makeoverviews.pl $(INSTALL_BIN)/
	if [ ! -d $(INSTALL_HTML)/cal ]; then mkdir $(INSTALL_HTML)/cal;fi
	chgrp ocfa $(INSTALL_HTML)/cal
	chmod 775 $(INSTALL_HTML)/cal
	cp cgi-bin/meta.cgi $(INSTALL_CGI)
	#install -d $(INSTALL_CGI) 
	#$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(DBFILE) $(INSTALL_CGI)

uninstall:
	#rm -r $(INSTALLLIB)
	#rm -r $(HTML_DOCDIR)

