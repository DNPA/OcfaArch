
OCFAARCH_TOP_DIR=..
include ../global.makeinfo
GCCFLAGS += $(GCCFLAGS_PPQ)
#GCCFLAGS += $(GCCFLAGS_STRICT)
#
OBJFILES=$(BUILDDIR)/PpqRoot.lo  $(BUILDDIR)/PersistentFifo.lo $(BUILDDIR)/PersistentPriorityQueue.lo $(BUILDDIR)/ReadFile.lo $(BUILDDIR)/StatusFile.lo $(BUILDDIR)/WriteFile.lo

#GCCFLAGS=-D_FILE_OFFSET_BITS=64 -g3

$(BUILDDIR)/%.lo: $(SRCDIR)/%.cpp $(INCDIR)/%.hpp
	if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR);fi
	echo INCFlags are $(INCFLAGS)
	$(LIBTOOL) --tag=CPP --mode=compile g++ -ggdb -c $(GCCFLAGS) $(INCFLAGS) -I$(BOOSTINCDIR) -o $@ $<


#
# same rule as above, except this one will not look at the header file.
#
$(BUILDDIR)/%.lo: $(SRCDIR)/%.cpp 
	if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR);fi
	$(LIBTOOL) --tag=CPP --mode=compile g++ -ggdb -c $(GCCFLAGS) $(INCFLAGS) -I$(BOOSTINCDIR) -o $@ $<

all: $(OBJFILES)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/libPersistentQueue.la $(OBJFILES)
	$(LIBTOOL) --tag=CPP --mode=compile g++ -c src/overview.cpp -Iinc -I$(BOOSTINCDIR) -o $(BUILDDIR)/overview.lo -D_FILE_OFFSET_BITS=64 -g3
	$(LIBTOOL) --tag=CPP --mode=link g++ $(BUILDDIR)/overview.lo $(BUILDDIR)/libPersistentQueue.la -o $(BUILDDIR)/ppqoverview
	$(LIBTOOL) --tag=CPP --mode=compile g++ -c src/PpqAnalyzer.cpp -Iinc -I$(BOOSTINCDIR) -o $(BUILDDIR)/PpqAnalyzer.lo -D_FILE_OFFSET_BITS=64 -g3
	$(LIBTOOL) --tag=CPP --mode=link g++ $(BUILDDIR)/PpqAnalyzer.lo -o $(BUILDDIR)/ppqanalyzer 
	$(LIBTOOL) --tag=CPP --mode=compile g++ -c src/hasdata.cpp -Iinc -I$(BOOSTINCDIR) -o $(BUILDDIR)/ppqhasdata.lo -D_FILE_OFFSET_BITS=64 -g3
	$(LIBTOOL) --tag=CPP --mode=link g++ $(BUILDDIR)/ppqhasdata.lo $(BUILDDIR)/libPersistentQueue.la -o $(BUILDDIR)/ppqhasdata
	$(LIBTOOL) --tag=CPP --mode=compile g++ -c src/TestRoot.cpp -Iinc -I$(BOOSTINCDIR) -o $(BUILDDIR)/TestRoot.lo -D_FILE_OFFSET_BITS=64 -g3
	$(LIBTOOL) --tag=CPP --mode=link g++ $(BUILDDIR)/TestRoot.lo $(BUILDDIR)/libPersistentQueue.la -o $(BUILDDIR)/TestRoot

install: all installetc
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	if [ ! -d $(INSTALL_CGI) ]; then mkdir $(INSTALL_CGI);fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/ppqoverview $(INSTALL_CGI)/
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/ppqhasdata $(INSTALL_SBIN)/
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/ppqanalyzer $(INSTALL_BIN)/
	cp scripts/ppqcontent $(INSTALL_CGI)/

installetc:
	
uninstall:
	rm -r $(INSTALL_SBIN)/$(PROJECT)
	rm -r $(HTML_DOCDIR)

	

dpkg: all
	install -d $(PWD)/build/debian/$(INSTALL_CGI)
	install -d $(PWD)/build/debian/$(INSTALL_SBIN)
	libtool --mode=install install build/ppqoverview $(PWD)/build/debian$(INSTALL_CGI)/
	libtool --mode=install install build/TestRoot $(PWD)/build/debian/$(INSTALL_SBIN)/
	cp scripts/ppqcontent $(PWD)/build/debian/$(INSTALL_CGI)/
	install -d build/debian/DEBIAN
	cp pkg/debian/control build/debian/DEBIAN/
	install -d build/debian/usr/share/doc/ppqtools
	cp ../LICENCE build/debian/usr/share/doc/ppqtools/copyright
	echo "See ocfa.sourceforge.net for details" > build/debian/usr/share/doc/ppqtools/changelog
	echo "See ocfa.sourceforge.net for details" >  build/debian/usr/share/doc/ppqtools/changelog.Debian
	gzip --best build/debian/usr/share/doc/ppqtools/*
	cd build;fakeroot dpkg-deb --build debian
	install -d ../debian
	mv build/debian.deb ../debian/ocfa-ppqtools_$(RELEASE)_amd64.deb
	rm -rf build/debian
	

