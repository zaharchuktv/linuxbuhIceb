subdirs=libsql libutil_g libdev_g libbuh_g buhg_g taxi locale


all:
	@for dir in $(subdirs) ; do \
		cd $$dir ; make all || exit 1 ; \
		cd .. ; \
	done

clean:
	@for dir in $(subdirs) ; do \
		cd $$dir ; make clean || exit 1 ; \
		cd .. ; \
	done ; rm -f *~

install:
	@for dir in $(subdirs) ; do \
		cd $$dir ; make install || exit 1 ; \
		cd .. ; \
	done 
