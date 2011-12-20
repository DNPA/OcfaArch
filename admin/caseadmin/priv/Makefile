include ../../../configure.makeinfo
all: restartapache restarttomcat

restartapache: restartapache.c
	gcc -o restartapache restartapache.c

restarttomcat: restarttomcat.c
	gcc -o restarttomcat restarttomcat.c

install: all
	cp restartapache $(INSTALL_BIN)
	cp restarttomcat $(INSTALL_BIN)
	chown root:ocfa $(INSTALL_BIN)/restartapache
	chown root:ocfa $(INSTALL_BIN)/restarttomcat
	chmod 4750 $(INSTALL_BIN)/restartapache
	chmod 4750 $(INSTALL_BIN)/restarttomcat
