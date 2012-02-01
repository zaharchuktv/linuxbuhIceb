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

HEADERS = ../headers/buhg_g.h ../../libbuh_g/libbuh_g.a ../../libutil_g/libutil_g.a ../../iceBw.h 
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

all: $(OBJ)

$(OBJ): $(HEADERS) 

install:

clean: 
	rm -f *.o ; rm -f *~





