# Standard stuff.
#
OCFAARCH_TOP_DIR=../..
include ../../global.makeinfo
GCCFLAGS += $(GCCFLAGS_SHOWTREE)
#GCCFLAGS += $(GCCFLAGS_STRICT)
#
# version number this is used to create some directoreis
#

#
#The project is the name of the sub project of the washing machine.
#
PROJECT=showtree

#
# The Build Dir defines the directory in which all newly created stuff should be put.
#
BUILDDIR=build
#
#The buildFiles define the object files that should be build. When you want to 
# compile a new file, you should add it to this list. Because it is added here, Make will
# automatically start looking for a similar file in the $(SRC) directory. 
#

SHOWTREEOBJFILESLIB=$(BUILDDIR)/ShowTree.lo 
SHOWEVIDENCEOBJFILESLIB=$(BUILDDIR)/ShowEvidence.lo $(BUILDDIR)/ShowEvidenceCGIMain.lo
SHOWDATAOBJFILESLIB=$(BUILDDIR)/ShowEvidence.lo $(BUILDDIR)/ShowEvidenceCGIData.lo
SHOWDATACOMMANDLIB=$(BUILDDIR)/ShowEvidence.lo $(BUILDDIR)/ShowDataMain.lo
SHOWFILEPATHCOMMANDLIB=$(BUILDDIR)/ShowEvidence.lo $(BUILDDIR)/ShowDataFilePathMain.lo
SHOWMETAIDFILES=$(BUILDDIR)/ShowEvidence.lo $(BUILDDIR)/ShowMetaWithIdCGIMain.lo
SHOWDATAIDFILES=$(BUILDDIR)/ShowEvidence.lo $(BUILDDIR)/ShowDataWithIdCGIMain.lo

#Directory in which all test code is located.

# INCDIR contains the header files.
INCFLAGS+=  -I$(INCPQDIR)

SHOWTREEEXEC=showtree.cgi
SHOWEVIDENCEEXEC=evidence.cgi
SHOWDATACGI=view.cgi
SHOWDATACOMMAND=showevidence
SHOWDATAFILEPATHCOMMAND=showdatapath
SHOWMETAIDCGI=evidenceid.cgi
SHOWDATAIDCGI=showdataid.cgi
# the libs which should be linked with our object files
# 
LIBS=$(INSTALLIB)/libocfamisc.la -lcgicc -L/usr/local/pgsql/lib -lpq $(INSTALLIB)/libocfastore.la
 #
#The flags that are used when compiling.
#GCCFLAGS=-pedantic -ansi
#GCCFLAGS=-Wall -pedantic-errors -Wno-long-long -Woverloaded-virtual -Wundef -Wconversion -Wsign-compare -Wredundant-decls -W
#
#Uncomment one of the folowing to select an architecture
all: showtree.cgi showevidence.cgi showdata.cgi showdata.command showfilepath.command showdataid.cgi showmetaid.cgi showdataid.cgi

showtree.cgi:  $(SHOWTREEOBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++  -g -o  $(BUILDDIR)/$(SHOWTREEEXEC) $(SHOWTREEOBJFILESLIB) $(LIBS)


showevidence.cgi: $(SHOWEVIDENCEOBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++  -g -o  $(BUILDDIR)/$(SHOWEVIDENCEEXEC) $(SHOWEVIDENCEOBJFILESLIB) $(LIBS)

showmetaid.cgi: $(SHOWMETAIDFILES)
	$(LIBTOOL) --tag=CPP --mode=link g++  -g -o  $(BUILDDIR)/$(SHOWMETAIDCGI) $(SHOWMETAIDFILES) $(LIBS)


showdataid.cgi:  $(BUILDDIR)/ShowDataWithIdCGIMain.lo
	echo showdataidfiels = $(SHOWDATAIDFILES)
	$(LIBTOOL) --tag=CPP --mode=link g++  -g -o  $(BUILDDIR)/$(SHOWDATAIDCGI) $(SHOWDATAIDFILES) $(LIBS)


# This rules tells Make that in order to make an object file, it should
# compile the file with the corresponding name in the src directory.
#
$(BUILDDIR)/%.lo: $(SRCDIR)/%.cpp
	if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR);fi
	$(LIBTOOL) --tag=CPP --mode=compile g++ -g -c $(GCCFLAGS) $(INCFLAGS) -o $@ $<

showdata.cgi: $(SHOWDATAOBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++  -g  -o $(BUILDDIR)/$(SHOWDATACGI) $(SHOWDATAOBJFILESLIB) $(LIBS)	

 
showdata.command: $(SHOWDATACOMMANDLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(SHOWDATACOMMAND) $(SHOWDATACOMMANDLIB) $(LIBS)	

showfilepath.command: $(SHOWFILEPATHCOMMANDLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(SHOWDATAFILEPATHCOMMAND) $(SHOWFILEPATHCOMMANDLIB) $(LIBS)

# Target that creates the executable for the tests.
#
# Target that automagically creates documentation in html and man pages for the source
# in this project.
#
doc: all
	$(DOXYGEN)


install: all
	install -d $(INSTALL_CGI) 
	install -d $(INSTALL_HTML)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(SHOWTREEEXEC) $(INSTALL_CGI)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(SHOWEVIDENCEEXEC) $(INSTALL_CGI)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(SHOWDATACGI) $(INSTALL_CGI)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(SHOWDATACOMMAND) $(INSTALL_BIN)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(SHOWDATAFILEPATHCOMMAND) $(INSTALL_BIN)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(SHOWMETAIDCGI) $(INSTALL_CGI)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(SHOWDATAIDCGI) $(INSTALL_CGI)
	cp scripts/preview.cgi $(INSTALL_CGI)
	cp html/index.html $(INSTALL_HTML)
	cp html/index.cgi $(INSTALL_HTML)
	cp html/test.cgi $(INSTALL_HTML)
	cp html/index.xsl $(INSTALL_HTML)
	cp html/error.xsl $(INSTALL_HTML)
	cp html/ocfa.xsl $(INSTALL_HTML)
	cp html/ppqcontent.xsl $(INSTALL_HTML)
	cp html/ppqoverview.xsl $(INSTALL_HTML)
	cp html/ocfa.css $(INSTALL_HTML)
	cp -R html/icons $(INSTALL_HTML)
	cp -R html/img $(INSTALL_HTML)

uninstall:
	#rm -r $(INSTALLLIB)
	#rm -r $(HTML_DOCDIR)

