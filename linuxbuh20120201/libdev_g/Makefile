#
CC= c++

#INCLUDE = -I/usr/include/mysql -I/home/sasa/iceb/libsql

#CFLAGS = `pkg-config --cflags gtk+-2.0` $(INCLUDE) 

INCLUDE = -I../libutil_g

CFLAGS = $(INCLUDE) -O2 -Wall

LOADLIBES = \
	-L../libutil_g \
	 -lutil_g 

LDFLAGS= -s


#HEADERS = iceb_d_libdev.h iceb_d_local.h

OBJ =  workid.o


all:	libdev_g.a $(BIN)

libdev_g.a: $(OBJ) 
	ar cr libdev_g.a $(OBJ)


$(OBJ): $(HEADERS)

install:

clean: 
	rm -f *.o ; rm -f *~ ;	rm -f libdev_g.a
