/*$Id: xrnn_rasp_r.c,v 1.21 2011-08-29 07:13:44 sasa Exp $*/
/*18.02.2010	05.05.2008	Белых А.И.	xrnn_rasp_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "xrnn_poiw.h"
#include "rnn_d5w.h"
#include <pwd.h>

class xrnn_rasp_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class xrnn_poiw *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  xrnn_rasp_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   xrnn_rasp_r_key_press(GtkWidget *widget,GdkEventKey *event,class xrnn_rasp_r_data *data);
gint xrnn_rasp_r1(class xrnn_rasp_r_data *data);
void  xrnn_rasp_r_v_knopka(GtkWidget *widget,class xrnn_rasp_r_data *data);

int rasrnn_pw(class xrnn_poiw *data,int kol_strok,int period_type,FILE *ff,FILE *ff_prot,FILE *ff_xml,FILE *ff_dot,FILE *ff_xml_dot,
FILE *ff_xml_rozkor,
FILE *ff_lst_rozkor,
class rnn_d5w *dod5,char *imaf_dbf,char *imaf_dbf_d,GtkWidget *bar,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);

int rasrnn_vw(class xrnn_poiw *data,int kol_strok,int period_type,FILE *ff,FILE *ff_prot,FILE *ff_xml,
FILE *ff_dot,FILE *ff_xml_dot,
FILE *ff_xml_rozkor,
FILE *ff_lst_rozkor,
class rnn_d5w *dod5,char *imaf_dbf,char *imaf_dbf_d,GtkWidget *bar,GtkWidget *view,
GtkTextBuffer *buffer,GtkWidget *wpredok);

int rnnrd5w(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,char *imaf_svod,char *imaf_svod_naim,
char *imaf_xml_dot,char *imaf_svod_dot,double proc_dot,class rnn_d5w *dod5,GtkWidget *wpredok);

extern class iceb_rnfxml_data rek_zag_nn;
extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
class iceb_u_str kat_for_nal_nak;

int xrnn_rasp_r(class xrnn_poiw *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class xrnn_rasp_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать реестр налоговых накладных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(xrnn_rasp_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Распечатать реестр налоговых накладных"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),imabaz,organ);
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),450,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
//PangoFontDescription *font_pango=pango_font_description_from_string("Nimbus Mono L, Bold 12");
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.view),font_pango);
pango_font_description_free(font_pango);

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(xrnn_rasp_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)xrnn_rasp_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xrnn_rasp_r_v_knopka(GtkWidget *widget,class xrnn_rasp_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   xrnn_rasp_r_key_press(GtkWidget *widget,GdkEventKey *event,class xrnn_rasp_r_data *data)
{
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}

/***********************************/
/*Шапка xml файла*/
/**************************/

int rasrnn_sap_xml(short dn,short mn,short gn,short dk,short mk,short gk,
char *imaf_xml,
char *imaf_xml_dot,
char *imaf_xml_rozkor,
int *period_type,double proc_dot,
FILE **ff_xml,
FILE **ff_xml_dot,
FILE **ff_xml_rozkor,
GtkWidget *wpredok)
{
if(iceb_openxml(dn,mn,gn,dk,mk,gk,imaf_xml,"J12","015","05",1,period_type,&rek_zag_nn,ff_xml,wpredok) != 0)  /*Основной реестр*/
 return(1);

rnn_sap_xmlw(mn,gn,mk,"J12","015",5,1,"J1201505.xsd",*period_type,&rek_zag_nn,*ff_xml); /*Основной реестр*/



fprintf(*ff_xml," <DECLARBODY>\n");
fprintf(*ff_xml,"   <HZY>%d</HZY>\n",gk);
fprintf(*ff_xml,"   <HZ>1</HZ>\n"); /*Звітний*/
fprintf(*ff_xml,"   <HNP>1</HNP>\n"); /*Номер порції реєстру*/
fprintf(*ff_xml,"   <HZKV></HZKV>\n");
fprintf(*ff_xml,"   <HZM>%d</HZM>\n",mk);
fprintf(*ff_xml,"   <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr));
fprintf(*ff_xml,"   <HLOC>%s</HLOC>\n",iceb_u_filtr_xml(rek_zag_nn.adres));
//fprintf(*ff_xml,"   <HTIN>%s</HTIN>\n",rek_zag_nn.innn);
fprintf(*ff_xml,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn);
fprintf(*ff_xml,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds);
fprintf(*ff_xml,"   <H01G01D>%02d%02d%04d</H01G01D>\n",dn,mn,gn);
fprintf(*ff_xml,"   <H01G02I>%04d</H01G02I>\n",gk); //архивный год
fprintf(*ff_xml,"   <H02G01D>%02d%02d%04d</H02G01D>\n",dk,mk,gk);
fprintf(*ff_xml,"   <H02G02S>1</H02G02S>\n"); //номер документа
fprintf(*ff_xml,"   <H03G02I>1</H03G02I>\n"); // количество страниц
fprintf(*ff_xml,"   <H03G01S>9</H03G01S>\n"); // Время хранения документа в годах

/*Расчёт корректировки сумм НДС к налоговой декларации с налога на добавленную стоимость додаток 1*/
if(iceb_openxml(dn,mn,gn,dk,mk,gk,imaf_xml_rozkor,"J02","005","09",1,period_type,&rek_zag_nn,ff_xml_rozkor,wpredok) != 0)  /*Основной реестр*/
 return(1);

rnn_sap_xmlw(mn,gn,mk,"J02","005",9,1,"J0200509.xsd",*period_type,&rek_zag_nn,*ff_xml_rozkor); /*Основной реестр*/

fprintf(*ff_xml_rozkor," <DECLARBODY>\n");
fprintf(*ff_xml_rozkor,"   <HZ>1</HZ>\n"); /*Звітний*/
fprintf(*ff_xml_rozkor,"   <HZY>%d</HZY>\n",gk);
fprintf(*ff_xml_rozkor,"   <HZM>%d</HZM>\n",mk);
fprintf(*ff_xml_rozkor,"   <HZYP xsi:nil=\"true\"></HZYP>\n");
fprintf(*ff_xml_rozkor,"   <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr));
fprintf(*ff_xml_rozkor,"   <HTINJ>%s</HTINJ>\n",rek_zag_nn.innn);
fprintf(*ff_xml_rozkor,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds);
fprintf(*ff_xml_rozkor,"   <HDDGVSD xsi:nil=\"true\"></HDDGVSD>\n");
fprintf(*ff_xml_rozkor,"   <HNDGVSD xsi:nil=\"true\"></HNDGVSD>\n");
//fprintf(*ff_xml_rozkor,"   <HNSPDV>01834282</HNSPDV>

if(proc_dot != 0.)
 {
  /*отличается от основного только порядковым номером документа*/
  if(iceb_openxml(dn,mn,gn,dk,mk,gk,imaf_xml_dot,"J12","015","05",2,period_type,&rek_zag_nn,ff_xml_dot,wpredok) != 0) /*Дополнительный*/
   return(1);
  rnn_sap_xmlw(mn,gn,mk,"J12","015",5,2,"J1201505.xsd",*period_type,&rek_zag_nn,*ff_xml_dot); /*Дополнительный*/

  fprintf(*ff_xml_dot," <DECLARBODY>\n");
  fprintf(*ff_xml_dot,"   <HZY>%d</HZY>\n",gk);
  fprintf(*ff_xml_dot,"   <HZ>1</HZ>\n"); /*Звітний*/
  fprintf(*ff_xml_dot,"   <HNP>1</HNP>\n"); /*Номер порції реєстру*/
  fprintf(*ff_xml_dot,"   <HZKV></HZKV>\n");
  fprintf(*ff_xml_dot,"   <HZM>%d</HZM>\n",mk);
  fprintf(*ff_xml_dot,"   <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr));
  fprintf(*ff_xml_dot,"   <HLOC>%s</HLOC>\n",iceb_u_filtr_xml(rek_zag_nn.adres));
  //fprintf(*ff_xml_dot,"   <HTIN>%s</HTIN>\n",rek_zag_nn.innn);
  fprintf(*ff_xml_dot,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn);
  fprintf(*ff_xml_dot,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds);
  fprintf(*ff_xml_dot,"   <H01G01D>%02d%02d%04d</H01G01D>\n",dn,mn,gn);
  fprintf(*ff_xml_dot,"   <H01G02I>%04d</H01G02I>\n",gk); //архивный год
  fprintf(*ff_xml_dot,"   <H02G01D>%02d%02d%04d</H02G01D>\n",dk,mk,gk);
  fprintf(*ff_xml_dot,"   <H02G02S>1</H02G02S>\n"); //номер документа
  fprintf(*ff_xml_dot,"   <H03G02I>1</H03G02I>\n"); // количество страниц
  fprintf(*ff_xml_dot,"   <H03G01S>9</H03G01S>\n"); // Время хранения документа в годах
 }



char bros[512];
memset(bros,'\0',sizeof(bros));
if(iceb_poldan("Код группы возвратная тара",bros,"matnast.alx",wpredok) == 0)
  rek_zag_nn.kod_gr_voz_tar=atoi(bros);




return(0);

}


/******************************************/
/******************************************/

gint xrnn_rasp_r1(class xrnn_rasp_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);


short dn,mn,gn;
short dk,mk,gk;
iceb_u_rsdat(&dn,&mn,&gn,data->rk->datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->rk->datak.ravno(),1);

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

iceb_rnfxml(&rek_zag_nn,data->window); /*чтение настроек для создания xml файлов*/




if(iceb_make_kat(&kat_for_nal_nak,"nal_nak_xml",data->window) != 0)
 {
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
 }

//Чистим каталог от файлов
iceb_delfil(kat_for_nal_nak.ravno(),"xml",data->window);

FILE *ff_xml;
char imaf_xml[56];

FILE *ff_lst_rozkor=NULL;
char imaf_lst_rozkor[56]; /*розрахунок коригування сумм ПДВ*/
sprintf(imaf_lst_rozkor,"rk%d.lst",getpid());

FILE *ff_xml_rozkor=NULL;
char imaf_xml_rozkor[56]; 

/************
  из списка выданных накладных делается Коригування податковых зобов'язань
  из списка полученных накладных делается Коригування податкового кредиту 
*************/

FILE *ff_xml_dot=NULL;
char imaf_xml_dot[56];

memset(imaf_xml_dot,'\0',sizeof(imaf_xml_dot));

int period_type=0;
if(rasrnn_sap_xml(dn,mn,gn,dk,mk,gk,imaf_xml,imaf_xml_dot,imaf_xml_rozkor,&period_type,data->rk->proc_dot.ravno_atof(),
&ff_xml,&ff_xml_dot,&ff_xml_rozkor,data->window) != 0)
 return(1);

if((ff_lst_rozkor=fopen(imaf_lst_rozkor,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_lst_rozkor,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov("Розрахунок коригування сум ПДВ до податкової\nдекларації з податку на додану вартість",dn,mn,gn,dk,mk,gk,organ,ff_lst_rozkor);


FILE *ff;
char imaf[112];
sprintf(imaf,"rnn%d.lst",getpid());

if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff_dot=NULL;
char imaf_dot[112];
memset(imaf_dot,'\0',sizeof(imaf_dot));

if(data->rk->proc_dot.ravno_atof() != 0.)
 {
  sprintf(imaf_dot,"rnnd%d.lst",getpid());

  if((ff_dot=fopen(imaf_dot,"w")) == NULL)
   {
    iceb_er_op_fil(imaf_dot,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

 }
 
FILE *ff_prot;
char imaf_prot[112];
sprintf(imaf_prot,"rnnp%d.lst",getpid());

if((ff_prot=fopen(imaf_prot,"w")) == NULL)
 {
    iceb_er_op_fil(imaf_prot,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
 }

class rnn_d5w dod5;

//Печатаем шапку
int kol_strok=rpvnn_psw(dn,mn,gn,dk,mk,gk,ff,data->window);
if(ff_dot != NULL)
 rpvnn_psw(dn,mn,gn,dk,mk,gk,ff_dot,data->window);

char imaf_p_dbf[56];
char imaf_p_dbf_d[56];
char imaf_v_dbf[56];
char imaf_v_dbf_d[56];

memset(imaf_p_dbf,'\0',sizeof(imaf_p_dbf));
memset(imaf_p_dbf_d,'\0',sizeof(imaf_p_dbf_d));
memset(imaf_v_dbf,'\0',sizeof(imaf_v_dbf));
memset(imaf_v_dbf_d,'\0',sizeof(imaf_v_dbf_d));

if(iceb_u_sravmydat(data->rk->datan.ravno(),"1.1.2011") < 0)
 {
  if(data->rk->pr_ras == 0 || data->rk->pr_ras == 1)
   {
    fprintf(ff_lst_rozkor,"\nКоригування податкового кредиту\n");

    if(rasrnn_pw(data->rk,kol_strok,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,
    ff_xml_rozkor,
    ff_lst_rozkor,
    &dod5,imaf_p_dbf,imaf_p_dbf_d,data->bar,data->view,data->buffer,data->window) == 0)

    if(data->rk->pr_ras == 0 )
     {
      fprintf(ff,"\f");
      if(ff_dot != NULL)
        fprintf(ff_dot,"\f");
      kol_strok=0;
     }
   }
  if(data->rk->pr_ras == 0 || data->rk->pr_ras == 2)
   {
    fprintf(ff_lst_rozkor,"\nКоригування податкових зобов'язань\n");
    rasrnn_vw(data->rk,kol_strok,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,
    ff_xml_rozkor,
    ff_lst_rozkor,
    &dod5,imaf_v_dbf,imaf_v_dbf_d,data->bar,data->view,data->buffer,data->window);
   }
 }

if(iceb_u_sravmydat(data->rk->datan.ravno(),"1.1.2011") >= 0)
 {
  if(data->rk->pr_ras == 0 || data->rk->pr_ras == 2)
   {
    fprintf(ff_lst_rozkor,"\nКоригування податкових зобов'язань\n");
    if(rasrnn_vw(data->rk,kol_strok,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,
    ff_xml_rozkor,
    ff_lst_rozkor,
    &dod5,imaf_v_dbf,imaf_v_dbf_d,data->bar,data->view,data->buffer,data->window) == 0)
    if(data->rk->pr_ras == 0 )
     {
      fprintf(ff,"\f");
      if(ff_dot != NULL)
        fprintf(ff_dot,"\f");
      kol_strok=0;
     }
    
   }
  if(data->rk->pr_ras == 0 || data->rk->pr_ras == 1)
   {
    fprintf(ff_lst_rozkor,"\nКоригування податкового кредиту\n");

    rasrnn_pw(data->rk,kol_strok,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,
    ff_xml_rozkor,
    ff_lst_rozkor,
    &dod5,imaf_p_dbf,imaf_p_dbf_d,data->bar,data->view,data->buffer,data->window);
   }
 }
iceb_podpis(ff_lst_rozkor,data->window);
fclose(ff_lst_rozkor);

class iceb_fioruk_rk rukov;
class iceb_fioruk_rk glbuh;
iceb_fioruk(1,&rukov,data->window);
iceb_fioruk(2,&glbuh,data->window);

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

fprintf(ff_xml_rozkor,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
fprintf(ff_xml_rozkor,"   <HBOS>%s</HBOS>\n",rukov.fio.ravno());
fprintf(ff_xml_rozkor,"   <HBUH>%s</HBUH>\n",glbuh.fio.ravno());
fprintf(ff_xml_rozkor,"   <HFO xsi:nil=\"true\"></HFO>\n");
fprintf(ff_xml_rozkor," </DECLARBODY>\n");
fprintf(ff_xml_rozkor,"</DECLAR>\n");
fclose(ff_xml_rozkor);


fprintf(ff_xml,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
fprintf(ff_xml,"   <HBOS>%s</HBOS>\n",rukov.fio.ravno());
fprintf(ff_xml,"   <HKBOS>%s</HKBOS>\n",rukov.inn.ravno());
fprintf(ff_xml,"   <HBUH>%s</HBUH>\n",glbuh.fio.ravno());
fprintf(ff_xml,"   <HKBUH>%s</HKBUH>\n",glbuh.inn.ravno());
fprintf(ff_xml,"  </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");
fclose(ff_xml);
if(ff_xml_dot != NULL)
 {
  fprintf(ff_xml_dot,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
  fprintf(ff_xml_dot,"   <HBOS>%s</HBOS>\n",rukov.fio.ravno());
  fprintf(ff_xml_dot,"   <HKBOS>%s</HKBOS>\n",rukov.inn.ravno());
  fprintf(ff_xml_dot,"   <HBUH>%s</HBUH>\n",glbuh.fio.ravno());
  fprintf(ff_xml_dot,"   <HKBUH>%s</HKBUH>\n",glbuh.inn.ravno());
  fprintf(ff_xml_dot,"  </DECLARBODY>\n");
  fprintf(ff_xml_dot,"</DECLAR>\n");
  fclose(ff_xml_dot);
 }
iceb_podpis(ff,data->window);
fclose(ff);

if(ff_dot != NULL)
 {
  iceb_podpis(ff_dot,data->window);
  fclose(ff_dot);

 }
iceb_podpis(ff_prot,data->window);
fclose(ff_prot);

char imaf_dod5_xml[112];
char imaf_svod[56];
char imaf_svod_naim[56];

char imaf_dod5_xml_dot[112];
char imaf_svod_dot[56];
memset(imaf_dod5_xml_dot,'\0',sizeof(imaf_dod5_xml_dot));
memset(imaf_svod_dot,'\0',sizeof(imaf_svod_dot));

rnnrd5w(dn,mn,gn,dk,mk,gk,imaf_dod5_xml,imaf_svod,imaf_svod_naim,imaf_dod5_xml_dot,imaf_svod_dot,data->rk->proc_dot.ravno_atof(),&dod5,data->window);


data->rk->imaf.plus(imaf);
data->rk->naim.plus("Реєстр податкових накладних (Загальний)");

if(imaf_dot[0] != '\0')
 {
  data->rk->imaf.plus(imaf_dot);
  data->rk->naim.plus("Реєстр податкових накладних (Переробного в-ва))");
 }

data->rk->imaf.plus(imaf_svod);
data->rk->naim.plus("Додаток 5");
if(imaf_svod_dot[0] != '\0')
 {
  data->rk->imaf.plus(imaf_svod_dot);
  data->rk->naim.plus("Додаток 2");
 }
data->rk->imaf.plus(imaf_svod_naim);
data->rk->naim.plus("Додаток 5 з найменуванням контрагентів");

data->rk->imaf.plus(imaf_prot);
data->rk->naim.plus("Протокол расчёта");

data->rk->imaf.plus(imaf_lst_rozkor);
data->rk->naim.plus("Додаток 1");


for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);
 
data->rk->imaf.plus(imaf_xml);
data->rk->naim.plus("Реєстр податкових накладних (Загальний) в форматі xml");

data->rk->imaf.plus(imaf_xml_rozkor);
data->rk->naim.plus("Додаток 1 в форматі xml");

if(imaf_xml_dot[0] != '\0')
 {
  data->rk->imaf.plus(imaf_xml_dot);
  data->rk->naim.plus("Реєстр податкових накладних в форматі xml (Переробного в-ва)");
 }

data->rk->imaf.plus(imaf_dod5_xml);
data->rk->naim.plus("Додаток 5 в форматі xml");
if(imaf_dod5_xml_dot[0] != '\0')
 {
  data->rk->imaf.plus(imaf_dod5_xml_dot);
  data->rk->naim.plus("Додаток 2 в форматі xml");
 }

/*Так как dbf файлы делаются отдельно прихоный отдельно расходный то любого из них может небыть*/
if(imaf_p_dbf[0] != '\0')
 {
  data->rk->imaf.plus(imaf_p_dbf);
  data->rk->naim.plus("Реєстр отриманних податкових накладных в форматі dbf (Загальний)");
 }
if(imaf_p_dbf_d[0] != '\0')
 {
  data->rk->imaf.plus(imaf_p_dbf_d);
  data->rk->naim.plus("Реєстр отриманих податкових накладных в формате dbf (Переробного в-ва)");
 }
printf("imaf_v_dbf=%s\n",imaf_v_dbf);

if(imaf_v_dbf[0] != '\0')
 {
  data->rk->imaf.plus(imaf_v_dbf);
  data->rk->naim.plus("Реєстр виданих податкових накладных в форматі dbf (Загальний)");
 }
if(imaf_v_dbf_d[0] != '\0')
 {
  data->rk->imaf.plus(imaf_v_dbf_d);
  data->rk->naim.plus("Реестр виданих податкових накладных в формате dbf (Переробного в-ва)");
 }











gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
