PROJECT=router
OCFAARCH_TOP_DIR=..
LIBS=-lxerces-c
include ../global.makeinfo

OBJFILESLIB=$(BUILDDIR)/router.lo


$(PROJECT): $(OBJFILESLIB)
	$(LIBTOOL) --mode=compile g++ ${LIBS} $(OCFALIBS)  -rpath $(INSTALL_SBIN) $(OBJFILESLIB) -o $(BUILDDIR)/$(PROJECT)

