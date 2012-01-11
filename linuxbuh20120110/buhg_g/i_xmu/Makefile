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

BIN = i_xmu

OBJ = i_xmu.o \
	avtpromuw.o avtpromu1w.o \
	cennikdw.o cennikmw.o cennikw.o copdokw.o copdok_m.o copdok_r.o\
	doocw.o dooc_m.o dooc_r.o doocsumw.o doocsumw_m.o doocsumw_r.o \
	dvmatw.o dvtmcf1w.o dvtmcf1_m.o dvtmcf1_r.o dvtmcf2w.o dvtmcf2_m.o dvtmcf2_r.o \
	dvtmcf3w.o dvtmcf3_m.o dvtmcf3_r.o \
	eks_ko_v.o \
	findkarw.o \
	hrasactw.o \
	impmatdokw.o impmatdok_r.o \
	in_d12_startw.o in_d12_itw.o \
	koncinvw.o krasactw.o kriostw.o kriostw_m.o kriostw_r.o kzvmu1.o kzvmu2.o \
	l_dokmat.o l_doksmat.o l_doksmat_p.o \
	l_kartmat.o l_kartmatv.o \
	l_kaskcn.o l_kaskcn_p.o l_kaskcn_v.o\
	l_kasopr.o l_kasopr_p.o l_kasopr_v.o \
	l_kasopp.o l_kasopp_p.o l_kasopp_v.o \
	l_mater.o l_mater_dv.o l_mater_p.o l_mater_v.o l_mater_unz.o l_mater_zkm.o l_matusl.o l_matusl_v.o\
	l_nalog.o l_nalog_v.o \
	l_opldok.o l_opldok_v.o \
	l_poimatdok.o l_prov_mu.o \
	l_spec.o l_spec1.o l_spec_v.o\
	l_zkartmat.o l_zkartmat_p.o l_zkartmat_v.o \
	l_spis_kas.o l_spis_kas_v.o \
	l_uslgrup.o l_uslgrup_v.o l_uslgrup_p.o \
	l_uslopp.o l_uslopp_p.o l_uslopp_v.o \
	l_uosopp.o l_uosopp_p.o l_uosopp_v.o \
	l_uslopr.o l_uslopr_p.o l_uslopr_v.o \
        l_uslugi.o l_uslugi_v.o l_uslugi_p.o \
	l_uosopr.o l_uosopr_p.o l_uosopr_v.o \
	makkorw.o makkorrw.o matnastw.o matsporw.o matspow.o matstatw.o matstatrw.o matuddokw.o matuddokw_r.o \
	matudw.o matuddw.o menuvnpw.o mudmcw.o mudmcw_m.o mudmcw_r.o muinvw.o muinv_m.o muinv_r.o \
	mumow.o mumow_m.o mumow_r.o \
	nomkmw.o nomkrw.o \
	oplmuw.o opssw.o opssw_m.o opssw_r.o ostdokw.o ostdok1w.o ../obshie/ostkarw.o ostkar1w.o \
	ost_skl_shetw.o ost_skl_shetw_m.o ost_skl_shetw_r.o \
	perecenpw.o perecenrw.o plc_mu.o plc_mu_r.o podtdokw.o podtdok1w.o podvdokw.o podzapm.o poigod_mu.o \
	prihod_m.o priv_k_kartw.o poimatdokw.o prcnw.o prdppw.o prihodw.o prognozw.o prognozw_m.o prognozw_r.o \
	prosprw.o provkolw.o provndsw.o provudvdokw.o \
	rasactmuw.o rasdokw.o rasdok1w.o rasdok2w.o rashapw.o rashodw.o \
	rasmedprw.o rasmothw.o rasnalnw.o rasndsw.o rasndsw_m.o rasndsw_r.o \
	rasoskr_m.o rasoskr_r.o rasoskrw.o raspropw.o rasspdokw.o rasspdok_r.o rasspdok_m.o readdokw.o \
	readkolkw.o rozkor_m.o rozkorw.o rozkor_r.o rozn_oth.o rozn_oth_m.o rozn_oth1_r.o rozn_oth2_r.o\
	rppvow.o rppvow_m.o rppvow_r.o\
	../obshie/saldo_m.o saldomuw.o saldomuw_r.o sapkkivw.o sapkk1ivw.o sapvinw.o sklkar_m.o skoplmuw.o specrasw.o specrasw_r.o spis_p_kontrw.o \
	spis_p_kontrw_m.o spis_p_kontrw_r.o spizw.o spizw_m.o spizw_r.o sproznw.o sprozn_r.o sprozn_m.o sumdokw.o \
	sumdokwu.o sumzap1w.o sumzap1uw.o sumzapuosw.o\
	udkarw.o \
	vdndw.o vdnd1.o vdnd2.o vbrpw.o vib_kodop_mu.o vib_kodop_uos.o vib_kodop_usl.o \
	vkartm.o vmat_mc.o vmat_medp.o vmatw.o \
	zagotkl.o zagotkl_m.o zagotkl_r.o zapdokw.o zaphkorw.o zapkartm.o zapkarw.o zap_prihodw.o zapvdokw.o zapvkrw.o zapzkorw.o \
	zap_s_mudokw.o

all: $(BIN)

i_xmu: $(OBJ)

$(OBJ): $(HEADERS) 

install:	all
	install -m 0755 $(BIN) $(BINDIR)

clean: 
	rm -f i_xmu; rm -f *.o ; rm -f *~





