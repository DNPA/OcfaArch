# Standard stuff.
#
OCFAARCH_TOP_DIR=..
LIBS=
PROJECT=router
include ../global.makeinfo
GCCFLAGS += $(GCCFLAGS_ROUTER)
#GCCFLAGS += $(GCCFLAGS_STRICT)
DISTDIR=/usr/local/digiwash
IMPLINCDIR=inc
OBJFILESLIB=$(BUILDDIR)/main.lo $(BUILDDIR)/router.lo $(BUILDDIR)/rulelist.lo $(BUILDDIR)/Rule.lo $(BUILDDIR)/LimitModule.lo $(BUILDDIR)/LimitScalar.lo $(BUILDDIR)/LimitNamespace.lo  $(BUILDDIR)/Table.lo $(BUILDDIR)/SetMetaFacet.lo
INCFLAGS += -I$(IMPLINCDIR) -I$(INTERFACEDIR) -I$(BOOSTINCDIR)
LIBS += -l$(BOOST_REGEX_LIB) 

$(PROJECT): $(OBJFILESLIB) 
	$(LIBTOOL) --tag=CPP --mode=link g++ -g -o $(BUILDDIR)/$(PROJECT) $(OBJFILESLIB) $(LIBS) $(OCFALIBS) -locfafacade -locfamodule -locfatreegraph
 
#$(BUILDDIR)/%.lo: $(SRCDIR)/%.cpp
#	if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR);fi
#	$(LIBTOOL) g++ -c $(GCCFLAGS) $(INCFLAGS) -o $@ $<

install: all installetc
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_DIR)/$(PROJECT)1
	chgrp ocfa $(INSTALL_DIR)/$(PROJECT)1
	chmod g+s $(INSTALL_DIR)/$(PROJECT)1
	ln -sf $(INSTALL_DIR)/$(PROJECT)1 $(INSTALL_DIR)/$(PROJECT)
	cp etc/rulelist.csv.snorkel $(INSTALL_ETC)
	cp etc/rulelist.csv.2.3.0 $(INSTALL_ETC)
	cp etc/rulelist.csv.bare $(INSTALL_ETC)
	cp scripts/dumplines.sh $(INSTALL_BIN)/
	cp scripts/rulelistfilter.pl $(INSTALL_BIN)/
	if [ ! -f $(INSTALL_ETC)/rulelist.csv ];then ln -s rulelist.csv.bare $(INSTALL_ETC)/rulelist.csv;fi

installetc:

uninstall:
	rm -r $(INSTALL_SBIN)/$(PROJECT)
	rm -r $(HTML_DOCDIR)
	

dpkg: all 
	install -d $(PWD)/build/debian/$(INSTALL_SBIN)
	libtool --mode=install install build/router $(PWD)/build/debian/$(INSTALL_SBIN)/
	install -d build/debian/DEBIAN
	cp pkg/debian/control build/debian/DEBIAN/
	install -d build/debian/usr/share/doc/ocfarouter
	cp ../LICENCE build/debian/usr/share/doc/ocfarouter/copyright
	echo "See ocfa.sourceforge.net for details" > build/debian/usr/share/doc/ocfarouter/changelog
	echo "See ocfa.sourceforge.net for details" >  build/debian/usr/share/doc/ocfarouter/changelog.Debian
	gzip --best build/debian/usr/share/doc/ocfarouter/*
	cd build;fakeroot dpkg-deb --build debian
	install -d ../debian
	mv build/debian.deb ../debian/ocfa-router_$(RELEASE)_amd64.deb
	rm -rf build/debian
	

