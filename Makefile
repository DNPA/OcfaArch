OCFAARCH_TOP_DIR=.
include configure.makeinfo
SHELL := /bin/bash
SUBTARGETS= ppq anycastrelay router fivesrouter dsm datainput/kickstart admin $(EXTRAARCH)
all:
	if [ $(CONF_OK) ]; then \
		echo Configuration seems ok; \
	else \
		echo Can not build with bad configuration;\
		exit 1;\
	fi;
	returnValue=0;\
	for dir in ppq $(SUBTARGETS);\
	do \
	$(MAKE) -C $$dir all ;\
	if [ $$? != 0 ]; then \
	        returnValue=1 ;\
	        echo $$dir has failed;\
		exit 2;\
	fi ; \
	done; \
	exit $$returnValue

dpkg:
	if [ `dpkg --print-architecture` != "amd64" ]; then \
		echo -n "Build debian packages only on amd64 architecture, not on ";\
		dpkg --print-architecture;\
		exit 1;\
	fi;
	for dir in $(SUBTARGETS);\
	do \
	$(MAKE) -C $$dir dpkg ;\
	if [ $$? != 0 ]; then \
		returnValue=1 ;\
		echo $$dir has failed;\
		exit 2;\
	fi ; \
	done; \
	exit $$returnValue

install: all
	returnValue=0;\
	for dir in $(SUBTARGETS);\
	do \
	$(MAKE) -C $$dir install ;\
	if [ $$? != 0 ]; then \
	        returnValue=1 ;\
		exit 1;\
	fi ; \
	done; \
	inst/updateusers
	install -d $(VARDIR)/queues
	chown ocfa:ocfa $(VARDIR)/queues
	chmod 775 $(VARDIR)/queues
	install -d $(VARDIR)/httpd
	echo "NameVirtualHost *:80" > $(VARDIR)/httpd/baseconfig.conf
	chown ocfa:ocfa $(VARDIR)
	chmod 770 $(VARDIR)
	chown root:ocfa $(VARDIR)
	chown -R root:ocfa $(VARDIR)/httpd
	chmod -R 770 $(VARDIR)/httpd
	inst/apacheconf
	inst/unsafe $(DISTDIR)
	if [ ! -d $(DISTDIR)/.gnupg ]; then \
	    mkdir $(DISTDIR)/.gnupg; \
	    chown ocfa:ocfa $(DISTDIR)/.gnupg; \
        fi;
	exit $$returnValue
	if [ -d /usr/local/digiwash2.0 ]; then \
	    mv /usr/local/digiwash2.0 /usr/local/digiwash2.0-original; \
	    ln -s /usr/local/digiwash /usr/local/digiwash2.0; \
	fi;
	
clean: 
	returnValue=0;\
	for dir in ppq $(SUBTARGETS);\
	do \
	$(MAKE) -C $$dir clean ;\
	if [ $$? != 0 ]; then \
	        returnValue=1 ;\
		exit 1;\
	fi ; \
	done; \
	exit $$returnValue

rootinstall:
	echo make rootinstall is depricated, use make install instead
	exit 1
