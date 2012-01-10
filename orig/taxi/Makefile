#

BINDIR=/usr/local/bin

CC  = c++
INCLUDE = \
	-I../libbuh_g \
	-I/usr/include/mysql \
	-I../libsql \
	-I../libutil_g \
	-I../libdev_g

CFLAGS = `pkg-config --cflags gtk+-2.0` $(INCLUDE) -O2 -Wall

#Построение с библиотекой чтения карточек
#LOADLIBES = -L/usr/lib -L../libsql -L../libbuh_g -L../libutil_g -L../libdev_g -lbuh_g -ldev_g -lutil_g -lsql -lmysqlclient -ltorgdev `pkg-config --libs gtk+-2.0`

#Построение без библиотеки чтения карточек
LOADLIBES = -L/usr/lib -L../libsql -L../libbuh_g -L../libutil_g -L../libdev_g -lbuh_g -ldev_g -lutil_g -lsql -lmysqlclient `pkg-config --libs gtk+-2.0`

LDFLAGS= -s

BIN =  i_taxi i_sbtaxi sel_hrift i_rest i_rest_k i_sozbrest i_alter_rt i_rest_saldo i_taxi_saldo i_pros_vv

	

HEADERS = taxi.h lzaktaxi.h taxi_oth.h \
	../libbuh_g/libbuh_g.a \
	../libutil_g/libutil_g.a \
	i_rest.h l_srestdok.h ../iceBw.h rest_oth.h gr_klient.h klient.h l_tk_saldo.h l_taxisk.h l_vrint.h rpod.h \
	l_spis_opl.h l_restdok2.h l_klientr.h l_vod.h l_taxikz.h l_taxitel.h

OBJ = i_taxi.o \
	lzaktaxi.o lzaktaxi_p.o lzaktaxiv.o \
	l_taxikz.o l_taxikz_p.o l_taxikz_v.o\
	l_taxisk.o l_taxisk_p.o l_taxisk_v.o\
	l_taxitel.o l_taxitel_p.o l_taxitel_v.o\
	l_vod.o l_vod_p.o l_vod_v.o \
	rpod_l.o rpod_v.o rpod_p.o \
	taxi_oth.o taxi_oth_v.o taxinast.o \
	taxi_oth_r.o taxi_saldo.o prov_zak_time.o

OBJ1 = i_sbtaxi.o i_sbtaxi_r.o
OBJ2 = sel_hrift.o
OBJ3 = i_rest.o \
	kod_podr_v.o \
	read_card.o restdok_rshet.o restdok_uddok.o restnast.o rest_flag_kl.o rest_oth_r.o \
	rest_oth_v.o rest_sal_kl.o rest_sptov_m.o rpod_l.o rpod_p.o rpod_v.o \
	l_grkl.o l_grkl_v.o l_grkl_p.o \
	l_klientr.o l_klientr_bl.o l_klientr_v.o l_klientr_vg.o l_klientr_p.o l_klient_imp.o \
	l_restdok.o l_restdok_v.o l_restdok2.o l_restdok2_vgr.o l_restdok2_vmu.o l_restdok2_znm.o \
	l_spisok.o l_spisok_v.o l_sptur.o l_sptur_v.o l_srestdok.o l_srestdok_p.o \
	l_vhvi.o l_vhvi_r.o l_vhvi_p.o l_vmat.o l_vrint.o l_vrint_k.o l_vrint_v.o l_vrint_p.o \
	m_poird.o \
	osttov.o \
	sumapsh.o \
	t_msql_error.o \
	vip_shet.o vrshet_nomdok.o vrshet_v.o \
	zap_rest_dok.o zap_vrest_dok.o zap_v_kas.o

OBJ4 = i_sozbrest.o i_sozbrest_r.o

OBJ5 = i_alter_rt.o i_alter_rtr.o

OBJ6 = i_rest_k.o \
	kod_podr_v.o \
	l_grkl.o l_grkl_p.o l_grkl_v.o\
	l_klientr.o l_klientr_bl.o l_klientr_p.o l_klientr_v.o l_klientr_vg.o l_klient_imp.o \
	l_restdok1.o l_saldokl.o l_spis_opl.o l_spis_opl_p.o l_spis_opl_z.o\
	l_vhvi_pkr.o \
	otm_opl_dok.o otm_opl_doks.o \
	prov_vv.o prov_vv_r.o \
	ras_kl.o ras_sal_kas.o ras_sal_kl.o ras_sheks.o read_card.o rest_flag_kl.o restdok_rshet.o restnast_k.o \
	rpod_l.o rpod_p.o rpod_v.o\
	spdnr_r.o sumapsh.o \
	t_msql_error.o \
	vid_kar_kl.o vsnsk.o vsvks.o vv_kolih.o\
	zap_v_kas.o 
	
OBJ7 = i_rest_saldo.o \
	restdok_uddok.o restnast.o rest_saldo_r.o rest_saldo_del_r.o\
	t_msql_error.o \
	zap_v_kas.o

OBJ8 = i_taxi_saldo.o \
	rpod_l.o rpod_v.o rpod_p.o \
	l_taxisk.o l_taxisk_p.o l_taxisk_v.o\
	l_tk_saldo.o l_tk_saldo_v.o l_tk_saldo_p.o \
	taxinast.o taxi_saldo_r.o taxi_saldo.o taxi_ud_god.o

OBJ9 = i_pros_vv.o \
	l_grkl.o l_grkl_p.o l_grkl_v.o \
	l_klientr.o l_klientr_bl.o l_klientr_p.o l_klientr_v.o l_klientr_vg.o l_klient_imp.o \
	l_vhvi.o l_vhvi_p.o l_vhvi_r.o \
        l_spis_opl.o l_spis_opl_p.o l_spis_opl_z.o \
	rpod_l.o rpod_p.o rpod_v.o \
	ud_zap_vv.o


all:	$(BIN)


i_taxi: $(OBJ)
i_sbtaxi: $(OBJ1) 
sel_hrift: $(OBJ2) 
i_rest: $(OBJ3) 
i_sozbrest: $(OBJ4) 
i_alter_rt:$(OBJ5)
i_rest_k: $(OBJ6) 
i_rest_saldo: $(OBJ7)
i_taxi_saldo: $(OBJ8)
i_pros_vv: $(OBJ9)

#$(OBJ):	Makefile $(HEADERS)

$(OBJ): $(HEADERS) 
$(OBJ1): $(HEADERS)
$(OBJ2): $(HEADERS)
$(OBJ3): $(HEADERS)
$(OBJ4): $(HEADERS)
$(OBJ5): $(HEADERS)
$(OBJ6): $(HEADERS)
$(OBJ7): $(HEADERS)
$(OBJ8): $(HEADERS)
$(OBJ9): $(HEADERS)

install:	all
	install -m 0755 $(BIN) $(BINDIR)

clean: 
	rm -f i_taxi i_sbtaxi sel_hrift *.o *~ i_alter_rt i_sozbrest i_rest i_rest_k i_rest_saldo i_taxi_saldo i_pros_vv

