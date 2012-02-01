# $Id: Makefile.am,v 5.2 2005-08-26 08:29:45 sasa Exp $
#

CONFDIR=$(DESTDIR)@CONFIG_PATH@/doc
#------------------------------------------------------------

all:    ;

install:        $(CONFDIR)
	$(INSTALL_DATA) *.txt $(CONFDIR)

$(CONFDIR):
	$(INSTALL) -d $(CONFDIR)

uninstall:
	rm -rf $(CONFDIR)

clean:
	rm -f *~
