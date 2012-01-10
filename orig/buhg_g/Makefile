#
BINDIR=/usr/local/bin

CC  = c++
INCLUDE = \
	-I../libbuh_g \
	-I/usr/include/mysql \
	-I../libdev_g \
	-I../libsql \
	-I../libutil_g

#Ключи для компиляции
CFLAGS = `pkg-config --cflags gtk+-2.0` $(INCLUDE) -O2 -Wall

LOADLIBES = -L/usr/lib \
	-L../libsql \
	-L../libbuh_g \
	-L../libutil_g \
	-lbuh_g  -lutil_g -lsql -lmysqlclient  `pkg-config --libs gtk+-2.0`

#Ключи для построения
LDFLAGS= -s

HEADERS = buhg_g.h l_saldo_sh.h \
 	../libbuh_g/libbuh_g.a \
	../libutil_g/libutil_g.a \
	../iceBw.h \
	l_mater.h ostatok.h rek_kartmat.h copdok.h rozkor.h dooc.h l_dokmat.h rasoskr.h muinv.h \
	dvtmcf1.h zagotkl.h dvtmcf2.h dvtmcf3.h rspvk.h go.h rasspdok.h glkni.h glkni_nbs.h pvbank.h \
	pvbanks.h dvtmcf3.h rasndsw.h prognozw.h rozn_oth.h doocsumw.h opssw.h \
	spizw.h specrasw.h l_uslgrup.h l_uslugi.h l_uslop.h l_uslpodr.h l_usldoks.h l_uslkart.h \
	usl_spismat.h dvuslf2w.h rasspdokuw.h rppvouw.h opss_uw.h l_uosop.h l_kasop.h \
	l_kasdoks.h kasothw.h l_xdkop.h dok4w.h l_xdkdoks.h l_tippl.h xdkgur.h l_prov.h l_saldo_shk.h \
	prov_poi.h buhkomw.h copdokus.h spis_p_kontrw.h buhhahw.h l_gruppod.h l_zarpodr.h l_zarnah.h \
	l_zarvidtab.h l_zarud.h l_zargni.h l_zarsocf.h vkartz.h zarp.h l_sptbn.h l_otrvr.h zar_ar.h \
	l_nahud.h l_kaskcn.h l_kaskcnsl.h kasothkcn.h imp_kr.h l_ukrgrup.h l_ukrzat.h zar_eks_ko.h \
	l_zar_dok.h l_zar_dok_zap.h rasvedw.h arz.h zar_kateg.h zar_otrsh.h zar_spisr.h zar_spdr.h \
	zar_sprav.h zar_srinu.h zar_srninu.h zar_otrvr.h zar_snu.h zar_ppn.h zar_dolgor.h zar_kart.h \
	zar_bpom.h zar_rtabel.h zar_f1df.h zar_f1dfr.h l_f1dfz.h zar_pensf.h zar_soc_nah.h zar_soc_vz.h \
	zar_podr.h zar_snahud.h buhsdkrw.h buhrpzw.h buhrpznpw.h buhvdzw.h buhgspw.h l_uospodr.h l_uosgrnu.h \
	l_uosgrbu.h l_uoshau.h l_uoshz.h l_uosmo.h uosvhd.h l_uosin.h uosprs.h uosdvosw.h uosvamotw.h \
	uosvamotbuw.h uosrpw.h uosvosiiw.h uosdpd.h uosinvedw.h uosvosw.h uosrbsw.h uos_spw.h l_ukrvk.h \
	l_ukrspdok.h ukrrpw.h ukrkontr.h ukrprik.h ukrshkz.h ukrspdokw.h ukrspdok1w.h l_uplpod.h l_uplmt.h l_uplout.h \
	l_uplavt.h l_uplsal.h l_uplgo.h l_uplost.h l_uplscnt.h l_uplspdok.h l_upldok_rekh.h upl_pvod.h \
	upl_pavt.h upl_rpavt.h upl_shvod.h upl_shkg.h upl_ovdt.h l_xrnngpn.h l_xrnngvn.h xrnn_poiw.h \
	rnn_d5w.h rnn_imp.h zapdokw.h imp_zardokw.h buhspkw.h buhpsdw.h ost_skl_shetw.h l_uosdm.h \
	l_dovers.h l_kasvedz.h l_zarsb.h l_doksmat.h zvb_hreshatik.h zvb_ibank2ua.h mumow.h glktmpw.h read_nast_indw.h \
	zarpensmw.h saldo.h admin_user.h zarpdw.h zarbesvw.h zarrnesvpw.h sprkvrt1w.h
	

BIN = i_admin i_start i_xbu i_xmu i_xusl i_xkas i_xdk i_xzp i_xuos i_xukr i_xupl i_xrnn i_xud

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
	ostkarw.o ostuosw.o \
	pdokkow.o pdokmuw.o pdokpdw.o pdokukrw.o pdokuosw.o pdokuslw.o pdokzpw.o \
	plc_gk.o plc_gk_r.o poiinw.o poiinpdw.o poiprovw.o provvkp.o \
	pib_end_dokw.o pib_itlw.o pib_strw.o \
	pvbank_m.o pvbank_r.o pvbanks_m.o pvbanks_r.o pvbanks_kprovw.o \
	r_poiprov.o rbd.o rpshet_rsw.o rpshet_ssw.o rpsrsw.o rspvk_m.o rspvk_r.o\
	saldogkw.o saldo_m.o saldogk_r.o saldonbsw.o sapgorsw.o \
	ukrsib_strw.o ukrsib_endw.o ukrsib_startw.o\
	ukreksim_hapw.o ukreksim_endw.o ukreksim_zstrw.o ukreksim_hw.o\
	vibshet.o vregp.o \
	zvb_hreshatik_startw.o zvb_hreshatik_start_v.o zvb_hreshatik_strw.o zvb_hreshatik_endw.o \
	zvb_ibank2ua_endw.o zvb_ibank2ua_strw.o zvb_ibank2ua_start_v.o zvb_ibank2ua_startw.o \
	zvb_ukrgaz_endw.o zvb_ukrgaz_startw.o zvb_ukrgaz_strw.o

OBJ1 = i_xmu.o \
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
	oplmuw.o opssw.o opssw_m.o opssw_r.o ostdokw.o ostdok1w.o ostkarw.o ostkar1w.o \
	ost_skl_shetw.o ost_skl_shetw_m.o ost_skl_shetw_r.o \
	perecenpw.o perecenrw.o plc_mu.o plc_mu_r.o podtdokw.o podtdok1w.o podvdokw.o podzapm.o poigod_mu.o \
	prihod_m.o priv_k_kartw.o poimatdokw.o prcnw.o prdppw.o prihodw.o prognozw.o prognozw_m.o prognozw_r.o \
	prosprw.o provkolw.o provndsw.o provudvdokw.o \
	rasactmuw.o rasdokw.o rasdok1w.o rasdok2w.o rashapw.o rashodw.o \
	rasmedprw.o rasmothw.o rasnalnw.o rasndsw.o rasndsw_m.o rasndsw_r.o \
	rasoskr_m.o rasoskr_r.o rasoskrw.o raspropw.o rasspdokw.o rasspdok_r.o rasspdok_m.o readdokw.o \
	readkolkw.o rozkor_m.o rozkorw.o rozkor_r.o rozn_oth.o rozn_oth_m.o rozn_oth1_r.o rozn_oth2_r.o\
	rppvow.o rppvow_m.o rppvow_r.o\
	saldo_m.o saldomuw.o saldomuw_r.o sapkkivw.o sapkk1ivw.o sapvinw.o sklkar_m.o skoplmuw.o specrasw.o specrasw_r.o spis_p_kontrw.o \
	spis_p_kontrw_m.o spis_p_kontrw_r.o spizw.o spizw_m.o spizw_r.o sproznw.o sprozn_r.o sprozn_m.o sumdokw.o \
	sumdokwu.o sumzap1w.o sumzap1uw.o sumzapuosw.o\
	udkarw.o \
	vdndw.o vdnd1.o vdnd2.o vbrpw.o vib_kodop_mu.o vib_kodop_uos.o vib_kodop_usl.o \
	vkartm.o vmat_mc.o vmat_medp.o vmatw.o \
	zagotkl.o zagotkl_m.o zagotkl_r.o zapdokw.o zaphkorw.o zapkartm.o zapkarw.o zap_prihodw.o zapvdokw.o zapvkrw.o zapzkorw.o \
	zap_s_mudokw.o

OBJ2 = i_xusl.o \
	avtprouslw.o avtprousl1w.o\
	copdokuw.o copdoku_r.o copdoku_m.o \
	dvuslf2w.o dvuslf2w_m.o dvuslf2w_r.o \
	eks_ko_v.o \
	findkarw.o \
	impusldokw.o impusldok_r.o \
	l_kaskcn.o l_kaskcn_p.o l_kaskcn_v.o \
	l_kasopr.o l_kasopr_p.o l_kasopr_v.o \
	l_kasopp.o l_kasopp_p.o l_kasopp_v.o \
	l_mater1.o l_mater_p.o \
	l_opldok.o l_opldok_v.o \
	l_poiusldok.o l_prov_usl.o \
	l_spis_kas.o l_spis_kas_v.o\
	l_usldok.o l_usldoks.o l_usldoks_p.o l_uslgrup.o l_uslgrup_p.o l_uslgrup_v.o \
	l_uslkart.o l_uslkart_p.o \
	l_uslopp.o l_uslopp_p.o l_uslopp_v.o \
	l_uosopp.o l_uosopp_p.o l_uosopp_v.o \
	l_uosopr.o l_uosopr_p.o l_uosopr_v.o \
	l_uslopr.o l_uslopr_p.o l_uslopr_v.o l_uslugi.o l_uslugi_p.o l_uslugi_v.o\
	l_uslpodr.o l_uslpodr_p.o l_uslpodr_v.o\
	makkorw.o makkorrw.o menuvnpw.o\
	nomkrw.o nomkmw.o \
	opss_uw.o opss_uw_m.o opss_uw_r.o ostkarw.o ostdok1w.o \
	perecenuw.o podtdok1w.o podvdokw.o podudokw.o podzapusl.o poigod_usl.o poiusldokw.o prdppw.o \
	prilogw.o priv_k_kartw.o prosprusw.o \
	rasdokuw.o rasnalnuw.o rasspdokuw.o rasspdokuw_m.o rasspdokuw_r.o \
	readkolkw.o readkoluw.o rppvouw.o rppvouw_m.o rppvouw_r.o \
	saldo_kontr.o skopluslw.o sumzap1w.o sumzap1uw.o sumzapuosw.o \
	uddusl_r.o uduvdokw.o usldopnalw.o uslnastw.o usl_act_vrw.o usl_shet_fakw.o \
	usl_spismat.o usl_spismat_m.o usl_spismat_r.o\
	vdnuw.o vib_kodop_mu.o vib_kodop_usl.o vib_kodop_uos.o vuslw.o vuslw_v.o \
	zapkarw.o zaphkorw.o zappoduw.o zapuvdokw.o zapvdokw.o zapvkrw.o zap_prihodw.o zapzkorw.o zap_s_mudokw.o

OBJ3 = i_xkas.o \
	avtprkasw.o \
	imp_kasordw.o imp_kasordw_r.o itogw.o \
	kasnastw.o kasothw.o kasothw_m.o kasothw_r.o kasothw_r1.o \
	kasothkcn.o kasothkcn_m.o kasothkcn_r.o \
	l_kasdok.o \
	l_kasdoks.o l_kasdoks_p.o \
        l_kaskcn.o l_kaskcn_p.o l_kaskcn_v.o \
	l_kaskcnsl.o l_kaskcnsl_p.o l_kaskcnsl_v.o \
	l_kasopp.o l_kasopp_p.o l_kasopp_v.o \
	l_kasopr.o l_kasopr_p.o l_kasopr_v.o \
	l_kasvedz.o l_kasvedz_p.o l_kasvedz_v.o \
	l_poikasdok.o l_prov_kas.o \
	l_spis_kas.o l_spis_kas_v.o \
	pbkasw.o poikasdokw.o provpodkow.o provprkasw.o \
	saldokasw.o saldokasw_r.o \
	sspp1w.o sumkasorw.o sumprvedw.o \
	udkasdokw.o udpldw.o \
	vib_kodop_kas.o vkasord1.o vkasord2.o vvko_kontr.o \
	zaphkorw.o zapzkorw.o zarstfn53w.o zarstvedw.o

OBJ4 = i_xdk.o \
	avtprpldw.o \
	b_aval.o b_brokbiznesw.o b_soft_revieww.o b_b2_bank_on_linew.o b_iBank_2_UAw.o b_pervinvw.o b_privatw.o b_tinyw.o \
	b_privatinvestw.o b_prominvest.o b_nadra.o b_skarbw.o b_ukrbiznesw.o b_ukrinbank.o b_ukrpromw.o b_ukreximw.o b_ukrsocw.o \
	dok_doverw.o \
	l_banks.o \
	l_poixdkdok.o \
	l_prov_xdk.o \
	l_sptbn1.o l_sptbn1_p.o \
	l_tippl.o l_tippl_p.o l_tippl_v.o \
	l_xdkdoks.o l_xdkdoks_p.o \
	l_xdkop.o l_xdkop_p.o l_xdkop_v.o \
	l_xdkpz.o l_xdkpz_v.o \
	nastdkw.o nomdokw.o \
	platpw.o podzapxdk.o poigod_xdk.o poixdkdokw.o provpdw.o \
	readkomw.o readpdokw.o \
	sumprpdw.o sumpzpdw.o \
	udpldw.o \
	vibshbnw.o vpld.o vzrkbzw.o vzrkfw.o \
	xdkdok.o xdkdok_v.o xdkgur.o xdkgur_r.o xdkgur_v.o xdksend.o xdksoznkom.o xdkudgod.o xdkudgod_r.o xdkvnp.o xdkvnp_rt.o\
	zappdokw.o

OBJ5 = i_xzp.o \
	alimw.o avtprozarw.o \
	arz.o arz_m.o arz_r.o \
	bolnw.o boln1w.o \
	clearzarw.o clearzarw_r.o\
	dopdominw.o \
	dolhnw.o dolhn1w.o \
	f1df_xml_konw.o f1df_xml_strw.o f1df_xml_zagw.o \
	fbezrw.o fbezr_sumnalw.o findnahw.o findnahmw.o find_podohw.o \
	hdisk.o hrvnks.o \
	impotvrw.o impotvrw_r.o \
	imp_kr.o imp_kr_m.o imp_kr_r.o imp_nu.o imp_nu_r.o imp_zardokw.o imp_zardokw_r.o imp_zar_kart.o imp_zar_kart_r.o \
	imp_zar_kartsh.o imp_zar_kartsh_r.o itogizw.o itogw.o \
	kategw.o kategshw.o knnfndsw.o koreshw.o kreditw.o \
	l_gruppod.o l_gruppod_p.o l_gruppod_v.o \
	l_f1df.o l_f1df_v.o l_f1dfz.o l_f1dfz_p.o l_f1dfz_v.o l_f1dfz1_v.o \
	l_kaskcn.o l_kaskcn_p.o l_kaskcn_v.o \
	l_kasopp.o l_kasopp_p.o l_kasopp_v.o \
	l_kasopr.o l_kasopr_p.o l_kasopr_v.o \
	l_nahud.o l_nahud_p.o l_nahud_vz.o \
	l_nahud_t.o l_nahud_t_v.o\
	l_otrvr.o l_otrvr_p.o l_otrvr_v.o l_otrvr_zk.o \
	l_otrvr_t.o l_otrvr_t_v.o \
	l_poizardok.o l_prov_zar.o \
	l_spis_kas.o l_spis_kas_v.o l_sptbn.o l_sptbn_p.o \
	l_ukrgrup.o l_ukrgrup_p.o l_ukrgrup_v.o \
	l_ukrzat.o l_ukrzat_p.o l_ukrzat_v.o \
	l_zaresv.o l_zaresv_v.o \
	l_zargni.o l_zargni_p.o l_zargni_v.o \
	l_zarkateg.o l_zarkateg_p.o l_zarkateg_v.o \
	l_zarkh.o l_zarkh_v.o \
	l_zarnah.o l_zarnah_p.o l_zarnah_v.o \
	l_zarpodr.o l_zarpodr_p.o l_zarpodr_v.o \
	l_zarsaldo.o l_zarsaldo_v.o \
	l_zarsb.o l_zarsb_p.o l_zarsb_v.o \
	l_zarsocf.o l_zarsocf_p.o l_zarsocf_v.o \
        l_zarsocot.o l_zarsocot_v.o \
	l_zarud.o l_zarud_p.o l_zarud_v.o \
        l_zarvidtab.o l_zarvidtab_p.o l_zarvidtab_v.o \
	l_zarzvan.o l_zarzvan_p.o l_zarzvan_v.o \
	l_zar_dok.o l_zar_dok_p.o l_zar_dok_hap.o l_zar_dok_zap.o l_zar_dok_zap_p.o l_zar_dok_zap_v.o\
	masinvw.o \
	matpom1w.o \
	nahudw.o nomdokw.o\
	okladw.o okladtrw.o otrshw.o \
	peh1w.o pensw.o pib_end_dokw.o pib_itlw.o pib_strw.o podoh1w.o podohrs1w.o podohrs2w.o poiflw.o poigod_zar.o poizardokw.o \
	prbperw.o prbper1w.o procentw.o profsw.o provblokzarpw.o provkodw.o provzzagw.o \
	rasf1df.o rasf1df_m.o rasnuptn1w.o rasshetzw.o rasvedw.o rasvedw_m.o rasvedw_r.o raszkartw.o ras_pensw.o \
	ras_podohw.o ras_podoh1w.o ras_socw.o read_nast_indw.o redkalw.o rnn_sap_xmlw.o rudnfvw.o \
	saldozrw.o saldozrw_r.o saldo_m.o \
	sapreestpw.o sosnsumw.o sprtabotpw.o sspp1w.o strfilw.o sumprvedw.o sumprzw.o \
	ukreksim_endw.o ukreksim_hapw.o ukreksim_hw.o ukreksim_zstrw.o \
	ukrsib_endw.o ukrsib_startw.o ukrsib_strw.o \
	vkartz.o vkartz1.o vvod_nahud.o vvod_podr.o vvod_tabnom.o vztbw.o \
	zarbanksw.o \
	zarbesvw.o zarbesvw_m.o zarbesvw_r.o \
	zardznw.o \
	zagrf1df.o \
	zaphkorw.o zapkrtbw.o zapmasw.o zaptabelw.o zapzagotw.o zapzardkw.o zapzarnw.o zapzarn1w.o zapzarpw.o zapzarpvw.o zapzkorw.o zaravprw.o zaravpr_oldw.o zaravpt_osw.o \
	zaresvw.o zardod13w.o zarindexw.o zarindexvw.o \
	zarkmdw.o zarkvrt_gkw.o zarlgotw.o zarnastw.o zarnvpw.o \
	zarpdw.o zarpdw_m.o zarpdw_r.o \
	zarpensmw.o zarpensmw_m.o zarpensm1w_r.o \
	zarpodprw.o zarprtnw.o zarpvw.o zarreadnw.o zarrnesvw.o zarrnesvpw.o zarsaldw.o zarsbslw.o zarsdf1dfw.o zarsf1dfw.o zarsocw.o zarsprprw.o zarsprvmnw.o zarsrbolw.o zarsruvw.o \
	zartarrozw.o zartarroz1w.o zarudkarw.o zarudnuw.o zaruozw.o zarvpnw.o zarstfn53w.o zarstvedw.o \
	zar_alim.o zar_alim_r.o \
	zar_ar.o zar_ar_r.o zar_ar_v.o zar_eks_ko.o zar_eks_ko_v.o \
	zar_bazno.o zar_bazno_r.o \
	zar_bpom.o zar_bpom_m.o zar_bpom_r.o \
	zar_dolgor.o zar_dolgor_m.o zar_dolgor_r.o \
	zar_dolgrab.o zar_dolgrab_r.o \
	zar_f1df.o zar_f1df_m.o zar_f1df_r.o \
	zar_f1pv.o zar_f1pv_r.o \
	zar_kart.o zar_kart_m.o zar_kart_r.o \
	zar_kateg.o zar_kateg_m.o zar_kateg_r.o \
	zar_krz.o zar_krz_r.o \
	zar_mg.o zar_mg_r.o \
	zar_mpslw.o \
	zar_otrsh.o zar_otrsh_m.o zar_otrsh_r.o \
	zar_otrvr.o zar_otrvr_m.o zar_otrvr_r.o \
	zar_pensf.o zar_pensf_m.o zar_pensf_r.o \
	zar_pi.o zar_pi_r.o \
	zar_podr.o zar_podr_m.o zar_podr1_r.o zar_podr2_r.o \
	zar_ppn.o zar_ppn_m.o zar_ppn_r.o \
	zar_rbh.o zar_rbh_r.o \
	zar_read_tnw.o zar_read_tn1w.o zar_rsocw.o zar_sm_shet.o \
	zar_rtabel.o zar_rtabel_m.o zar_rtabel_r.o \
	zar_snahud.o zar_snahud_m.o zar_snahud_r.o \
	zar_snu.o zar_snu_m.o zar_snu_r.o \
	zar_soc_nah.o zar_soc_nah_m.o zar_soc_nah_r.o \
	zar_soc_vz.o zar_soc_vz_m.o zar_soc_vz_r.o \
	zar_spdr.o zar_spdr_m.o zar_spdr_r.o \
	zar_spisr.o zar_spisr_m.o zar_spisr_r.o \
	zar_sprav.o zar_sprav_m.o zar_sprav_r.o \
	zar_spsoclgot.o zar_spsoclgot_r.o \
	zar_srinu.o zar_srinu_m.o zar_srinu_r.o \
	zar_srninu.o zar_srninu_m.o zar_srninu_r.o \
	zar_vedpz.o zar_vedpz_m.o zar_vedpz_r.o \
	zvaniew.o \
	zvb_alfabankw.o  zvb_alfabank_endw.o zvb_alfabank_startw.o  zvb_alfabank_start_v.o  zvb_alfabank_strw.o \
	zvb_kreditpromw.o \
	zvb_hreshatikw.o zvb_hreshatik_endw.o zvb_hreshatik_strw.o zvb_hreshatik_start_v.o zvb_hreshatik_startw.o \
	zvb_ibank2uaw.o zvb_ibank2ua_endw.o zvb_ibank2ua_strw.o zvb_ibank2ua_start_v.o zvb_ibank2ua_startw.o \
	zvb_nadraw.o zvb_pibw.o zvb_ukreksimw.o zvb_ukrpromw.o zvb_ukrpromw_m.o zvb_ukrpromw_r.o \
	zvb_ukrgazw.o zvb_ukrgaz_endw.o zvb_ukrgaz_startw.o zvb_ukrgaz_strw.o \
	zvb_promekonomw.o zvb_promekonom_endw.o zvb_promekonom_startw.o zvb_promekonom_strw.o \
	zvb_promekonomw.o zvb_promekonom_endw.o zvb_promekonom_startw.o zvb_promekonom_strw.o \
	zvb_ukrsibw.o zvb_ukrsocw.o
		
OBJ6 = i_start.o

OBJ7 = i_xuos.o \
	amortw.o amort23w.o amortbyw.o avtprouosw.o \
	bsiz23w.o bsiz23pw.o bsizpw.o bsizw.o \
	hrasactw.o \
	imp_uosdokw.o imp_uosdokw_r.o \
	in_d12_itw.o in_d12_startw.o \
	koncinvw.o krasactw.o \
	l_opldok.o l_opldok_v.o \
	l_poiuosdok.o \
	l_prov_uos.o \
	l_uosamort.o l_uosamort_v.o l_uosamortbu.o l_uosamortbu_v.o \
	l_uosdm.o l_uosdm_p.o l_uosdm_v.o \
	l_uosdok.o l_uosdok_v.o \
	l_uosgrbu.o l_uosgrbu_p.o l_uosgrbu_v.o \
	l_uosgrnu.o l_uosgrnu_p.o l_uosgrnu_v.o \
	l_uoshau.o l_uoshau_p.o l_uoshau_v.o \
	l_uoshz.o l_uoshz_p.o l_uoshz_v.o \
	l_uosin.o l_uosin_p.o \
	l_uosindm.o l_uosindm_v.o\
	l_uosls.o l_uosls_v.o \
	l_uosmo.o l_uosmo_p.o l_uosmo_v.o \
	l_uosopp.o l_uosopp_p.o l_uosopp_v.o \
	l_uosopr.o l_uosopr_p.o l_uosopr_v.o \
	l_uospodr.o l_uospodr_p.o l_uospodr_v.o \
        l_uosspdok.o l_uosspdok_p.o \
	menuvnpw.o \
	opluosw.o ostuosw.o \
	poduosdw.o poigod_uos.o poiinpdw.o poiinw.o poiuosdokw.o provuinw.o prpuosw.o pudokw.o puzapw.o\
	rasactuosw.o rasdokuosw.o rasdopuosw.o rasmes.o rasnalnuosw.o rasnasw.o \
	sapkk1ivw.o sapkkivw.o skopluosw.o \
	smenakofw.o smenakofw_m.o sumzapuosw.o \
	strms.o strpod.o \
	udamortw.o udinnw.o uduosdokw.o uduoszdw.o \
	uosdpd.o uosdpd_m.o uosdpd_r.o \
	uosdvosw.o uosdvosw_m.o uosdvosw_r.o\
	uosrbsw.o uosrbsw_m.o uosrbsw_r.o \
	uosrpw.o uosrpw_m.o uosrpw_r.o \
	uosactw.o uosgetinn.o uosgetlsw.o uosgetnd.o uoskart1.o uoskart2.o uosnastw.o uosprhd.o uosprovnd.o uosprovdok.o \
	uosinvedw.o uosinvedw_m.o uosinvedw_r.o \
	uoskarw.o \
	uosprovarw.o uosprovgrw.o uosprs_m.o uosprs_r.o uosprs.o \
	uosremw.o uosrsdw.o uosrsd1w.o \
	uossasw.o uossosw.o \
	uosvamotw.o uosvamotw_m.o uosvamotw_r.o\
	uosvamotbuw.o uosvamotbuw_m.o uosvamotbuw_r.o\
	uosvosiiw.o uosvosiiw_m.o uosvosiiw_r.o \
	uosvosw.o uosvosw_m.o uosvosw_r.o \
	uosvhd.o uosvhd1.o uosvhd2.o uosvhdvp.o uosvhdzv.o \
	uoszaphd.o uoszvdok.o \
	uos_spw.o uos_spw_m.o uos_spw_r.o \
	vib_kodop_uos.o

OBJ8 = i_xukr.o \
	avtprukrw.o \
	l_gruppod.o l_gruppod_p.o l_gruppod_v.o \
	l_poiukrdok.o \
	l_prov_ukr.o \
	l_ukrdok.o l_ukrdok_v.o l_ukrdok_pb.o l_ukrdok_vn.o \
	l_ukrgrup.o l_ukrgrup_p.o l_ukrgrup_v.o \
	l_ukrspdok.o l_ukrspdok_p.o \
	l_ukrvk.o l_ukrvk_p.o l_ukrvk_v.o \
	l_ukrzat.o l_ukrzat_p.o l_ukrzat_v.o \
	poiukrdokw.o \
	prosprkrw.o \
	ukrkontr.o ukrkontr_m.o ukrkontr_r.o \
	ukrkuw.o \
	ukrnastw.o \
	ukrprik.o ukrprik_m.o ukrprik_r.o \
	ukrrpw.o ukrrpw_m.o ukrrpw_r.o \
	ukrshkz.o ukrshkz_m.o ukrshkz_r.o \
	ukrspdokw.o ukrspdokw_m.o ukrspdokw_r.o \
	ukrspdok1w.o ukrspdok1w_m.o ukrspdok1w_r.o \
	ukrvhd.o

OBJ9 = i_xupl.o \
	dvmatw.o \
	l_kartmat.o \
	l_poiupldok.o \
	l_uplavt.o l_uplavt_p.o l_uplavt_v.o \
	l_upldok.o l_upldok_pb.o l_upldok_v.o l_upldok_vs1.o l_upldok_vzp.o l_upldok_vzr.o\
	l_uplgo.o l_uplgo_p.o l_uplgo_v.o \
	l_uplmt.o l_uplmt_p.o l_uplmt_v.o \
	l_uplost.o l_uplost_p.o l_uplost_v.o \
	l_uplout.o l_uplout_p.o l_uplout_v.o \
	l_uplpod.o l_uplpod_p.o l_uplpod_v.o \
	l_uplsal.o l_uplsal_p.o l_uplsal_v.o \
	l_uplscnt.o l_uplscnt_p.o l_uplscnt_v.o\
	l_uplspdok.o l_uplspdok_p.o \
	l_uplst.o l_uplst_v.o \
	l_mater.o l_mater_dv.o l_mater_v.o l_mater_p.o l_mater_zkm.o l_mater_unz.o \
	l_zkartmat.o l_zkartmat_p.o l_zkartmat_v.o\
	poiupldokw.o \
	nomkmw.o \
	ostdokw.o ostkarw.o ostkar1w.o ostdok1w.o \
	ostvaw.o \
	podvdokw.o prdppw.o prosprw.o \
	provpstw.o \
	readkolkw.o\
	saldo_m.o saldouplw.o saldouplw_r.o \
	uplnastw.o uplnomdokw.o \
	upl_ovdt.o upl_ovdt_m.o upl_ovdt_r.o \
	upl_pavt.o upl_pavt_m.o upl_pavt_r.o \
	upl_rpavt.o upl_rpavt_m.o upl_rpavt_r.o \
	upl_plc_r.o \
	upl_ptw.o \
	upl_pvod.o upl_pvod_m.o upl_pvod_r.o \
	upl_shkg.o upl_shkg_m.o upl_shkg_r.o \
	upl_shvod.o upl_shvod_m.o upl_shvod_r.o \
	upl_zapvdokw.o \
	zapkarw.o zapkartm.o zapvkrw.o 
	
OBJ10 = i_xrnn.o \
	end_nn_xmlw.o \
	l_ukrvk.o l_ukrvk_p.o l_ukrvk_v.o \
	l_uosopp.o l_uosopp_p.o l_uosopp_v.o \
	l_uosopr.o l_uosopr_p.o l_uosopr_v.o \
	l_uslopp.o l_uslopp_p.o l_uslopp_v.o \
	l_uslopr.o l_uslopr_p.o l_uslopr_v.o \
	l_xrnngpn.o l_xrnngpn_p.o l_xrnngpn_v.o\
	l_xrnngvn.o l_xrnngvn_p.o l_xrnngvn_v.o \
	l_xrnnp.o l_xrnnp_p.o l_xrnnp_v.o \
	l_xrnnv.o l_xrnnv_p.o l_xrnnv_v.o \
	open_fil_nn_xmlw.o \
	rasrnn_pw.o rasrnn_vw.o rnn_vsw.o \
	ree_nastw.o \
	xml_nn_matw.o xml_nn_ukrw.o xml_nn_uosw.o xml_nn_uslw.o \
	rnnovdw.o rnnrd5w.o rnnvvd.o rnnvdw.o rnn_sap_xmlw.o \
	rnn_imp.o rnn_imp_m.o rnn_imp_ovd.o rnn_imp_r1.o rnn_imp_r2.o \
	rpvnn_psw.o rpvnn_piw.o rpvnn_ps_pw.o rpvnn_viw.o \
	str_nn_xmlw.o sumzap1w.o sumzap1uw.o sumzapuosw.o \
	xrnn_kopd.o xrnn_kord.o \
	xrnn_rasp.o xrnn_rasp_m.o xrnn_rasp_r.o \
	xrnn_uzzp.o xrnn_uzzp_m.o \
	zag_nn_xmlw.o zapvree1w.o zapvreew.o


OBJ12 = i_admin.o \
	admin_alx.o admin_cp_alxw.o admin_del_alxw.o admin_l.o admin_makebaz_r.o admin_makebaz_m.o admin_dostup.o admin_user.o admin_user_p.o admin_bases.o \
	admin_makebaz.o admin_newuserw.o \
	admin_icebuser.o admin_icebuser_v.o admin_icebuser_v1.o \
	admin_provbaz.o \
	docinw.o \
	prov_uduserw.o

OBJ13 = i_xud.o \
	l_dover.o l_dover_v.o l_dovers.o l_dovers_p.o \
	l_poixuddok.o \
	l_sptbn1.o l_sptbn1_p.o \
	nomdokw.o \
	poixuddokw.o \
	udvhdw.o \
	xudrasdokw.o	

all: $(BIN)

i_xbu: $(OBJ)
i_xusl: $(OBJ2)
i_xmu: $(OBJ1)
i_xkas: $(OBJ3)
i_xdk: $(OBJ4)
i_xzp: $(OBJ5)
i_start:$(OBJ6)
i_xuos: $(OBJ7)
i_xukr: $(OBJ8)
i_xupl: $(OBJ9)
i_xrnn: $(OBJ10)
i_admin: $(OBJ12)
i_xud:$(OBJ13)

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
$(OBJ10): $(HEADERS) 
$(OBJ12): $(HEADERS) 
$(OBJ13): $(HEADERS) 

install:	all
	install -m 0755 $(BIN) $(BINDIR)

clean: 
	rm -f i_admin i_start i_xbu i_xmu i_xusl i_xkas i_xdk i_xzp i_xuos i_xukr i_xupl i_xrnn i_xud; rm -f *.o ; rm -f *~





