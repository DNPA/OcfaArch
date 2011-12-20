 #
#The project is the name of the sub project of the washing machine.
#
PROJECT=fsmod

# Standard stuff.
#
OCFAARCH_TOP_DIR=..
include ../global.makeinfo

OBJFILESLIB=$(BUILDDIR)/fsmod.lo 


EXEC=fsmod

$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS)

test:	$(BUILDDIR)/test.lo
	$(LIBTOOL) --mode=link g++ -o $(BUILDDIR)/test $(BUILDDIR)/test.lo

install: all
	install -d $(INSTALL_SBIN)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)

