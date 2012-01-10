#
CC= c++

INCLUDE = -I/usr/include/mysql

CFLAGS = $(INCLUDE) -O2 -Wall

OBJ = sql_cursor.o sql_closebaz.o sql_error.o sql_openbaz.o \
	sql_readkey.o sql_zap.o \
	sqlfiltr.o sql_nerror.o sql_flag.o sql_rows.o


HEADERS = libsql.h

LIB = libsql.a

all: $(LIB)

$(LIB): $(OBJ)
	ar cr $(LIB) $(OBJ)

$(OBJ): $(HEADERS)

install:

clean: 
	rm -f *.o ; rm -f *~ ; rm -f $(LIB)
