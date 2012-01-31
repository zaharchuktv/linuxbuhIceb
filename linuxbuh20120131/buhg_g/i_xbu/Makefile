#
BINDIR=/usr/bin

CC  = c++
INCLUDE = \
	-I../../libbuh_g \
	-I/usr/include/mysql \
	-I../../libdev_g \
	-I../../libsql \
	-I../../libutil_g

#Ключи для компиляции
CFLAGS = `pkg-config --cflags gtk+-2.0` $(INCLUDE) -O2 -Wall

LOADLIBES = -L/usr/lib \
	-L../../libsql \
	-L../../libbuh_g \
	-L../../libutil_g \
	-lbuh_g  -lutil_g -lsql -lmysqlclient  `pkg-config --libs gtk+-2.0`

#Ключи для построения
LDFLAGS= -s

HEADERS = ../headers/buhg_g.h ../../libbuh_g/libbuh_g.a \
	../../libutil_g/libutil_g.a \
	../../iceBw.h
#	l_saldo_sh.h l_mater.h ostatok.h rek_kartmat.h copdok.h rozkor.h dooc.h l_dokmat.h rasoskr.h muinv.h \
#	dvtmcf1.h zagotkl.h dvtmcf2.h dvtmcf3.h rspvk.h go.h rasspdok.h glkni.h glkni_nbs.h pvbank.h \
#	pvbanks.h dvtmcf3.h rasndsw.h prognozw.h rozn_oth.h doocsumw.h opssw.h \
#	spizw.h specrasw.h l_uslgrup.h l_uslugi.h l_uslop.h l_uslpodr.h l_usldoks.h l_uslkart.h \
#	usl_spismat.h dvuslf2w.h rasspdokuw.h rppvouw.h opss_uw.h l_uosop.h l_kasop.h \
#	l_kasdoks.h kasothw.h l_xdkop.h dok4w.h l_xdkdoks.h l_tippl.h xdkgur.h l_prov.h l_saldo_shk.h \
#	prov_poi.h buhkomw.h copdokus.h spis_p_kontrw.h buhhahw.h l_gruppod.h l_zarpodr.h l_zarnah.h \
#	l_zarvidtab.h l_zarud.h l_zargni.h l_zarsocf.h vkartz.h zarp.h l_sptbn.h l_otrvr.h zar_ar.h \
#	l_nahud.h l_kaskcn.h l_kaskcnsl.h kasothkcn.h imp_kr.h l_ukrgrup.h l_ukrzat.h zar_eks_ko.h \
#	l_zar_dok.h l_zar_dok_zap.h rasvedw.h arz.h zar_kateg.h zar_otrsh.h zar_spisr.h zar_spdr.h \
#	zar_sprav.h zar_srinu.h zar_srninu.h zar_otrvr.h zar_snu.h zar_ppn.h zar_dolgor.h zar_kart.h \
#	zar_bpom.h zar_rtabel.h zar_f1df.h zar_f1dfr.h l_f1dfz.h zar_pensf.h zar_soc_nah.h zar_soc_vz.h \
#	zar_podr.h zar_snahud.h buhsdkrw.h buhrpzw.h buhrpznpw.h buhvdzw.h buhgspw.h l_uospodr.h l_uosgrnu.h \
#	l_uosgrbu.h l_uoshau.h l_uoshz.h l_uosmo.h uosvhd.h l_uosin.h uosprs.h uosdvosw.h uosvamotw.h \
#	uosvamotbuw.h uosrpw.h uosvosiiw.h uosdpd.h uosinvedw.h uosvosw.h uosrbsw.h uos_spw.h l_ukrvk.h \
#	l_ukrspdok.h ukrrpw.h ukrkontr.h ukrprik.h ukrshkz.h ukrspdokw.h ukrspdok1w.h l_uplpod.h l_uplmt.h l_uplout.h \
#	l_uplavt.h l_uplsal.h l_uplgo.h l_uplost.h l_uplscnt.h l_uplspdok.h l_upldok_rekh.h upl_pvod.h \
#	upl_pavt.h upl_rpavt.h upl_shvod.h upl_shkg.h upl_ovdt.h l_xrnngpn.h l_xrnngvn.h xrnn_poiw.h \
#	rnn_d5w.h rnn_imp.h zapdokw.h imp_zardokw.h buhspkw.h buhpsdw.h ost_skl_shetw.h l_uosdm.h \
#	l_dovers.h l_kasvedz.h l_zarsb.h l_doksmat.h zvb_hreshatik.h zvb_ibank2ua.h mumow.h glktmpw.h read_nast_indw.h \
#	zarpensmw.h saldo.h admin_user.h zarpdw.h zarbesvw.h zarrnesvpw.h sprkvrt1w.h
#	

BIN = i_xbu

OBJ = i_xbu.o \
	balans_f1w.o balans_f2w.o \
	buhdenshetw.o \
	buhgspw.o buhgspw_m.o buhgspw_r.o \
	buhhahw.o buhhahw_m.o buhhahw_r.o \
	buhmesshetw.o \
	buhnast_g.o buhkomw.o buhkomw_m.o buhkomw_r.o \
	buhpsdw.o buhpsdw_m.o buhpsdw_r.o \
	buhrpzw.o buhrpzw_m.o buhrpzw_r.o \
	buhrpznpw.o buhrpznpw_m.o buhrpznpw_r.o \
	buhsdkrw.o buhsdkrw_m.o buhsdkrw_r.o \
	buhspkw.o buhspkw_m.o buhspkw_r.o \
	buhvdzw.o buhvdzw_m.o buhvdzw_r.o \
        buh_rhw.o \
	clprov_m.o clprov_r.o \
	forzappoi.o \
	glkniw.o glkni_m.o glkniw_r.o glkni_nbs_m.o glkni_nbs_r.o gosrsw_r.o gosssw_r.o go_m.o\
	go_srk.o go_srv_nbs.o go_ss_nbs.o \
	hdisk.o hrvnks.o \
	l_prov.o l_prov_kr.o l_prov_krnbs.o\
	l_saldo_sh.o l_saldo_sh_p.o l_saldo_sh_v.o \
	l_saldo_shk.o l_saldo_shk_p.o  l_saldo_shk_v.o \
	m_plc_gk.o m_poiprov.o m_vdd.o m_vprov_bs.o m_vprov_nbs.o m_vprov_bs_gr.o m_vprov_nbs_gr.o \
	../obshie/ostkarw.o ostuosw.o \
	pdokkow.o pdokmuw.o pdokpdw.o pdokukrw.o pdokuosw.o pdokuslw.o pdokzpw.o \
	plc_gk.o plc_gk_r.o poiinw.o poiinpdw.o poiprovw.o provvkp.o \
	pib_end_dokw.o pib_itlw.o pib_strw.o \
	pvbank_m.o pvbank_r.o pvbanks_m.o pvbanks_r.o pvbanks_kprovw.o \
	r_poiprov.o rbd.o rpshet_rsw.o rpshet_ssw.o rpsrsw.o rspvk_m.o rspvk_r.o\
	saldogkw.o ../obshie/saldo_m.o saldogk_r.o saldonbsw.o sapgorsw.o \
	ukrsib_strw.o ukrsib_endw.o ukrsib_startw.o\
	ukreksim_hapw.o ukreksim_endw.o ukreksim_zstrw.o ukreksim_hw.o\
	vibshet.o vregp.o \
	zvb_hreshatik_startw.o zvb_hreshatik_start_v.o zvb_hreshatik_strw.o zvb_hreshatik_endw.o \
	zvb_ibank2ua_endw.o zvb_ibank2ua_strw.o zvb_ibank2ua_start_v.o zvb_ibank2ua_startw.o \
	zvb_ukrgaz_endw.o zvb_ukrgaz_startw.o zvb_ukrgaz_strw.o

all: $(BIN)

i_xbu: $(OBJ)

$(OBJ): $(HEADERS) 

install:	all
	install -m 0755 $(BIN) $(BINDIR)

clean: 
	rm -f i_xbu; rm -f *.o ; rm -f *~





