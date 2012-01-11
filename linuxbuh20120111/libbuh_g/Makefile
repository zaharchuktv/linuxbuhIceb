#
CC= c++

INCLUDE = -I..//libutil_g -I/usr/include/mysql -I../libdev_g -I../libsql

#CFLAGS = `pkg-config --cflags gtk+-2.0` $(INCLUDE) -O2 -Wall
CFLAGS = `pkg-config --cflags gtk+-2.0` $(INCLUDE) -O2 -Wall

#	-L../libdev_g \
#	-lmysqlclient -ldev_g -lsql `pkg-config --libs gtk+-2.0`
LOADLIBES =  -L../libsql \
	-lmysqlclient -lsql `pkg-config --libs gtk+-2.0`

HEADERS = iceb_libbuh.h iceb_l_kontr.h iceb_rsn.h iceb_menu_vibknop.h ../libutil_g/libutil_g.a \
	../iceBw.h iceb_l_gkontr.h iceb_l_opmup.h iceb_l_opmur.h iceb_l_ei.h iceb_l_sklad.h \
	iceb_l_forop.h iceb_lock_tables.h iceb_sql_flag.h iceb_kassa.h iceb_clock.h iceb_read_card_enter.h \
	iceb_get_dev.h iceb_l_plansh.h iceb_l_grupmat.h iceb_gdite.h iceb_l_subbal.h iceb_razuz.h iceb_salorok.h \
	iceb_fioruk.h iceb_avp.h iceb_tmptab.h iceb_rnfxml.h
	

OBJ = iceb_msql_error.o iceb_eropbaz.o iceb_vparol.o iceb_infosys.o iceb_readkey.o \
	iceb_get_text.o iceb_get_text_str.o iceb_menu_danet.o iceb_vihod.o iceb_podpis.o iceb_mydoscopy.o \
	iceb_key_release.o iceb_rabfil.o iceb_ponp.o iceb_get_new_kod.o \
	iceb_menu_vvod1.o iceb_parol.o iceb_sql_zapis.o iceb_er_op_fil.o iceb_pbar.o iceb_menu_soob.o \
	iceb_dikont.o iceb_vizred.o iceb_rsdatp.o \
	iceb_zagacts.o iceb_konact.o iceb_zagsh.o iceb_salorksl.o iceb_printw.o iceb_printw_vr.o \
	iceb_prsh.o iceb_prsh1.o iceb_spks.o iceb_spks_v.o  iceb_spks_r.o \
	iceb_l_kontr.o iceb_l_kontrp.o iceb_l_kontrv.o iceb_l_kontrsh.o iceb_f_redfil.o \
	iceb_l_gkontr.o iceb_l_gkontr_p.o iceb_l_gkontr_v.o \
	iceb_l_plansh.o iceb_l_plansh_v.o iceb_l_plansh_p.o iceb_l_plansh_vksb.o \
	iceb_zapprov.o iceb_menu_vibknop.o iceb_menu_mv.o iceb_vibrek.o \
	iceb_udprov.o iceb_lock_tables.o iceb_klient_pvu.o iceb_klient_kor.o \
	iceb_sql_readkey.o iceb_printcod.o iceb_rsn.o iceb_l_rsfr.o iceb_l_spisokm.o \
	iceb_rmenuf.o iceb_smenabaz.o iceb_l_printcap.o iceb_runsystem.o iceb_str_poisk.o \
	iceb_l_grupmat.o iceb_l_grupmat_p.o iceb_l_grupmat_v.o iceb_print.o iceb_imafn.o \
	iceb_l_ei.o iceb_l_ei_p.o iceb_l_ei_v.o \
	iceb_l_forop.o iceb_l_forop_p.o iceb_l_forop_v.o \
	iceb_l_sklad.o iceb_l_sklad_p.o iceb_l_sklad_v.o \
	iceb_l_opmup.o iceb_l_opmup_p.o iceb_l_opmup_v.o \
	iceb_l_opmur.o iceb_l_opmur_p.o iceb_l_opmur_v.o \
	iceb_rsdatp_str.o iceb_refresh.o iceb_kodkon.o iceb_rstimep_str.o iceb_mesc.o \
	iceb_prcn.o iceb_udprgr.o iceb_provinnom.o iceb_invnomer.o \
	iceb_nomnak.o iceb_00_skl.o iceb_00_kontr.o iceb_get_menu.o iceb_sql_flag.o \
	iceb_vprov.o iceb_provsh.o iceb_zapmpr.o iceb_zapmpr1.o iceb_sumvpr.o iceb_raspprov.o \
	iceb_pehf.o iceb_kasord1.o iceb_kasord2.o iceb_nalndoh.o iceb_razuz.o iceb_razuz1.o \
	iceb_razuz_kod.o iceb_getnkas.o iceb_errorkas.o iceb_p_kol_strok_list.o iceb_file_selection.o \
	iceb_nastsys.o iceb_kolstrok.o iceb_mous_klav.o iceb_exit.o iceb_rab_kas.o iceb_phzp.o \
	iceb_mariq_vs.o iceb_mariq_zvs.o iceb_mariq_slvi.o iceb_smenabaz_m.o iceb_calendar.o iceb_calendar1.o\
	iceb_start.o iceb_read_card.o iceb_read_card_enter.o iceb_connect_dserver.o iceb_close_dserver.o \
	iceb_perzap.o iceb_get_dev.o iceb_nomnalnak.o iceb_provnomnnak.o iceb_rasnaln1.o iceb_rasnaln2.o \
	iceb_snanomer.o iceb_gdite.o iceb_prosf.o iceb_menu_mes_god.o iceb_spis_komp.o \
	iceb_l_subbal.o iceb_l_subbal_p.o iceb_l_subbal_v.o iceb_menu_import.o iceb_start_rf.o \
	iceb_pm_nds.o iceb_prov_iz.o iceb_prospp.o iceb_pm_vibor.o \
	iceb_salorok.o iceb_salorok_m.o iceb_salorok_r.o iceb_akt_sv.o iceb_spkw.o \
	iceb_pbp.o iceb_pbpds.o iceb_pbsh.o iceb_delfil.o iceb_fioruk.o iceb_mpods.o iceb_pvkdd.o \
	iceb_uionp.o iceb_pvglkni.o iceb_make_kat.o iceb_fsystem.o iceb_avp.o iceb_poi_kontr.o \
	iceb_ustpeh.o iceb_sortkk.o iceb_cp.o iceb_cat.o iceb_poldan.o iceb_poldan_vkl.o iceb_perecod.o iceb_pnnnp.o \
	iceb_vperecod.o iceb_tmptab.o iceb_tokoi8u.o iceb_vb.o iceb_progforlst.o iceb_dir_select.o iceb_menu_start.o \
	iceb_about.o iceb_l_smenabaz.o iceb_l_smenabaz_p.o iceb_l_smenabaz_v.o iceb_optimbazw.o iceb_optimbazw_r.o \
	iceb_font_selection.o iceb_pm_pr.o iceb_l_blok.o iceb_l_blok_v.o iceb_getuid.o iceb_prr.o \
	iceb_getnps.o iceb_pblok.o iceb_getfioop.o iceb_prn.o iceb_alxin.o iceb_alxout.o iceb_kszap.o \
	iceb_print_operation.o iceb_insfil.o iceb_pnds.o iceb_alxzag.o iceb_kikz.o iceb_fplus.o iceb_sozmas.o iceb_vixod.o \
	iceb_nalnaks.o iceb_nalnaks_v.o iceb_nalnake.o iceb_nalnak_str.o iceb_vkk00.o iceb_l_kontdog.o iceb_l_kontdog_v.o \
	iceb_getuslp.o iceb_getuslp_m.o iceb_rnfxml.o iceb_openxml.o iceb_menu_vibkat.o iceb_copfil.o iceb_get_pnk.o
	
	

all: libbuh_g.a

libbuh_g.a: $(OBJ)
	ar cr libbuh_g.a $(OBJ)

$(OBJ): $(HEADERS)

install:

clean: 
	rm -f *.o ; rm -f *~ ; rm -f libbuh_g.a
