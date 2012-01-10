#
CC= c++


CFLAGS = `pkg-config --cflags glib-2.0`  -O2 -Wall

#LOADLIBES =  `pkg-config --libs glib-2.0`

HEADERS = iceb_util.h iceb_u_spisok.h iceb_u_int.h iceb_u_double.h  iceb_u_str.h ../iceBw.h

OBJ = iceb_u_utf.o iceb_u_pole.o iceb_u_pole1.o iceb_u_pole2.o iceb_u_srav.o \
	iceb_u_spisok.o \
	iceb_u_proverka.o iceb_u_catgets.o iceb_u_strstrm.o \
	iceb_u_vremzap.o iceb_u_kszap.o iceb_u_polen.o \
	iceb_u_atof.o iceb_u_zagolov.o iceb_u_startfil.o iceb_u_poldan.o iceb_u_ps_minus.o \
	iceb_u_rsdat.o iceb_u_rstime.o iceb_u_sravmydat.o iceb_u_datzap.o iceb_u_str.o \
	iceb_u_dpm.o iceb_u_prnbr.o iceb_u_sqldata.o iceb_u_okrug.o iceb_u_print.o iceb_u_pstrz.o \
	iceb_u_period.o iceb_u_preobr.o iceb_u_rsdat1.o iceb_u_vrem.o \
	iceb_u_bstab.o iceb_u_poltekdat.o iceb_u_fgets.o iceb_u_vstavfil.o \
	iceb_u_double.o iceb_u_int.o iceb_u_vrem_tek.o iceb_u_dat_to_sec.o iceb_u_dat_to_sec_end.o \
	iceb_u_ud_lp.o iceb_u_sravtime.o iceb_u_prnb.o iceb_u_ud_simv.o iceb_u_prnb1.o iceb_u_ekrs.o \
	iceb_u_sozmas.o iceb_u_denrog.o iceb_u_slstr.o iceb_u_datetime_sec.o iceb_u_pole3.o iceb_u_datetime_pr.o \
	iceb_u_tolower_str.o iceb_u_srav_r.o iceb_u_toupper_str.o iceb_u_fplus.o iceb_u_fio.o iceb_u_kol_str.o \
	iceb_u_kol_str_l.o iceb_u_zvstr.o iceb_u_cpbstr.o iceb_u_poltekgod.o iceb_u_ustpeh.o iceb_u_strlen.o \
	iceb_u_adrsimv.o iceb_u_kolbait.o iceb_u_dattosql.o iceb_u_strncpy.o iceb_u_getlogin.o iceb_u_sqlfiltr.o \
	iceb_u_filtr_xml.o iceb_u_nofwords.o iceb_u_propis.o

all:	libutil_g.a

libutil_g.a: $(OBJ) iceb_util.h
	ar cr libutil_g.a $(OBJ)

$(OBJ): $(HEADERS)

install:

clean: 
	rm -f *.o ; rm -f *~ ;	rm -f libutil_g.a

