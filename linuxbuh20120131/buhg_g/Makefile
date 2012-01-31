subdirs=obshie i_admin i_start i_xbu i_xmu

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
