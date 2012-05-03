# Standard stuff.
#
OCFAARCH_TOP_DIR=..
LIBS=
PROJECT=fivesrouter
include ../global.makeinfo
GCCFLAGS += $(GCCFLAGS_ROUTER)
#GCCFLAGS += $(GCCFLAGS_STRICT)
DISTDIR=/usr/local/digiwash
IMPLINCDIR=inc
OBJFILESLIB=$(BUILDDIR)/main.lo $(BUILDDIR)/router.lo $(BUILDDIR)/rulelist.lo $(BUILDDIR)/Rule.lo $(BUILDDIR)/LimitModule.lo $(BUILDDIR)/LimitScalar.lo $(BUILDDIR)/LimitNamespace.lo  $(BUILDDIR)/Table.lo $(BUILDDIR)/SetMetaFacet.lo ${BUILDDIR}/Lexer.lo ${BUILDDIR}/Parser.lo ${BUILDDIR}/Expression.lo 
INCFLAGS += -I$(IMPLINCDIR) -I$(INTERFACEDIR) -I$(BOOSTINCDIR)
LIBS += -l$(BOOST_REGEX_LIB) 

$(PROJECT): $(OBJFILESLIB) 
	$(LIBTOOL) --tag=CPP --mode=link g++ -g -o $(BUILDDIR)/$(PROJECT) $(OBJFILESLIB) $(LIBS) $(OCFALIBS) -locfafacade -locfamodule -locfatreegraph
 
#$(BUILDDIR)/%.lo: $(SRCDIR)/%.cpp
#	if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR);fi
#	$(LIBTOOL) g++ -c $(GCCFLAGS) $(INCFLAGS) -o $@ $<

install: all installetc
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_DIR)
	chgrp ocfa $(INSTALL_DIR)/$(PROJECT)
	chmod g+s $(INSTALL_DIR)/$(PROJECT)
	cp etc/rulelist.fives.csv $(INSTALL_ETC)
	cp etc/rulelist.fives.csv.complete $(INSTALL_ETC)
	cp etc/rulelist.fives.csv.complete.2 $(INSTALL_ETC)

installetc:

uninstall:
	rm -r $(INSTALL_SBIN)/$(PROJECT)
	rm -r $(HTML_DOCDIR)
	

dpkg: all 
	install -d $(PWD)/build/debian/$(INSTALL_SBIN)
	libtool --mode=install install build/fivesrouter $(PWD)/build/debian/$(INSTALL_SBIN)/
	install -d build/debian/DEBIAN
	cp pkg/debian/control build/debian/DEBIAN/
	install -d build/debian/usr/share/doc/fivesrouter
	cp ../LICENCE build/debian/usr/share/doc/fivesrouter/copyright
	echo "See ocfa.sourceforge.net for details" > build/debian/usr/share/doc/fivesrouter/changelog
	echo "See ocfa.sourceforge.net for details" >  build/debian/usr/share/doc/fivesrouter/changelog.Debian
	gzip --best build/debian/usr/share/doc/fivesrouter/*
	cd build;fakeroot dpkg-deb --build debian
	install -d ../debian
	mv build/debian.deb ../debian/ocfa-fivesrouter_$(RELEASE)_amd64.deb
	rm -rf build/debian
	

