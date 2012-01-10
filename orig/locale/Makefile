

all:	iceBw.mo

#iceBw.mo:	iceBw.ua.msg
#	gencat $@ $< 
iceBw.mo:	iceBw_uk.po
	msgfmt -vo $@ $< 
	
install:	all
	install -m 0644 iceBw.mo /usr/share/locale/ru/LC_MESSAGES

clean:
	rm -f *~ iceBw.mo
	