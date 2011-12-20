# Standard stuff.
#
OCFAARCH_TOP_DIR=../..
include ../../global.makeinfo
GCCFLAGS += $(GCCFLAGS_LUCENE) -g
#GCCFLAGS += $(GCCFLAGS_STRICT)
#
# version number this is used to create some directoreis
#
VERSION=0.5

#The project is the name of the sub project of the washing machine.
#
PROJECT=lucene

#
BUILDDIR=build
HTMLDIR=html
LUCENE_CGI=lucene.cgi
LUCENE_EXEC=lucenesearch
QUERYTOOL_CGI= querytool.cgi
LUCENE_LIB=libocfaquerylucene.la
SHOWDICTIONARY_EXEC=showdictionary
#
#The buildFiles define the object files that should be build. When you want to 
# compile a new file, you should add it to this list. Because it is added here, Make will
# automatically start looking for a similar file in the $(SRC) directory. 
#


LUCENECGIOBJFILES=$(BUILDDIR)/IndexCgiFrontEnd.lo $(BUILDDIR)/IndexFrontEndCgiMain.lo
LUCENECOMMANDOBJFILES=$(BUILDDIR)/IndexCliFrontEnd.lo $(BUILDDIR)/IndexFrontEndCliMain.lo 
LUCENEOBJFILES=$(BUILDDIR)/DbMetaSearcher.lo $(BUILDDIR)/LuceneSearcher.lo $(BUILDDIR)/IndexResultGatherer.lo $(BUILDDIR)/MultiIndexSearcher.lo $(BUILDDIR)/IndexFrontEnd.lo $(BUILDDIR)/DocInfo.lo $(BUILDDIR)/MultiIndexDbSearcher.lo
QUERYTOOLOBJFILES=$(BUILDDIR)/QueryTool.lo $(BUILDDIR)/IndexResultGatherer.lo $(BUILDDIR)/DocInfo.lo
SHOWDICTIONARYOBJFILES=$(BUILDDIR)/ShowDictionary.lo
#Directory in which all test code is located.

# INCDIR contains the header files.
INCFLAGS+= -I$(INCPQDIR) -I/usr/local/lib -I/usr/local/lib64 -I/usr/lib -I/usr/lib64

# the libs which should be linked with our object files
# 
LIBS= -lcgicc $(LIBPQDIR) -lpq $(OCFALIBS) $(LIBCLUCENE)
#
#The flags that are used when compiling.
GCCFLAGS=-pedantic -ansi
#GCCFLAGS=-Wall -pedantic-errors -Wno-long-long -Woverloaded-virtual -Wundef -Wconversion -Wsign-compare -Wredundant-decls -W
#
#Uncomment one of the folowing to select an architecture
all: lucene.cgi lucene querytool library showdictionary

lucene.cgi:  $(LUCENECGIOBJFILES) $(LUCENEOBJFILES)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(LUCENE_CGI) $(LUCENECGIOBJFILES) $(LUCENEOBJFILES) $(LIBS)


lucene: $(LUCENECOMMANDOBJFILES)  $(LUCENEOBJFILES)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(LUCENE_EXEC)   -rpath $(INSTALLIB) $(LUCENEOBJFILES) $(LUCENECOMMANDOBJFILES) $(LIBS)

library: $(LUCENEOBJFILES)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(LUCENE_LIB) -rpath $(INSTALLIB) $(LUCENEOBJFILES) $(LIBS) $(OCFALIBS)

querytool: $(QUERYTOOLOBJFILES)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(QUERYTOOL_CGI) $(QUERYTOOLOBJFILES) $(LIBS)

showdictionary: $(SHOWDICTIONARYOBJFILES)
	$(LIBTOOL) --tag=CPP --mode=link g++ $(SHOWDICTIONARYOBJFILES) -o $(BUILDDIR)/$(SHOWDICTIONARY_EXEC) $(LIBS)

# This rules tells Make that in order to make an object file, it should
# compile the file with the corresponding name in the src directory.
#
$(BUILDDIR)/%.lo: $(SRCDIR)/%.cpp
	if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR);fi
	$(LIBTOOL) --tag=CPP --mode=compile g++ -g -c $(GCCFLAGS) $(INCFLAGS) -o $@ $<

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
	install -d $(INSTALL_BIN)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(LUCENE_CGI) $(INSTALL_CGI)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(LUCENE_EXEC) $(INSTALL_BIN)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(QUERYTOOL_CGI) $(INSTALL_CGI)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(SHOWDICTIONARY_EXEC) $(INSTALL_BIN)
	cp $(HTMLDIR)/*.jpg $(INSTALL_HTML)
	cp $(HTMLDIR)/*.html $(INSTALL_HTML)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(LUCENE_LIB) $(INSTALLIB)
	cp $(HTMLDIR)/*.js $(INSTALL_HTML)
	cp scripts/parkindex.pl $(INSTALL_BIN)
	cp scripts/viewnice.cgi $(INSTALL_CGI)

