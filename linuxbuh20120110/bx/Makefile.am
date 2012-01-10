# $Id: Makefile.am,v 5.2 2005-08-26 08:29:43 sasa Exp $
#

CONFDIR=$(DESTDIR)@CONFIG_PATH@/bx
#------------------------------------------------------------

all:    ;

install:
	@if [ \! -d $(CONFDIR) ]; then \
	   $(INSTALL) -d $(CONFDIR) ;\
	   $(INSTALL_DATA) *.alx $(CONFDIR) ;\
	fi

uninstall:
	rm -rf $(CONFDIR)

clean:
	rm -f *~
