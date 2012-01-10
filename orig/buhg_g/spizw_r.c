/*$Id: spizw_r.c,v 1.16 2011-02-21 07:35:57 sasa Exp $*/
/*16.12.2009	17.05.2005	Белых А.И. 	spizw_r.c
Расчёт списания материалов изделия с карточек материального учёта
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "spizw.h"

class spizw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class spizw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  spizw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   spizw_r_key_press(GtkWidget *widget,GdkEventKey *event,class spizw_r_data *data);
gint spizw_r1(class spizw_r_data *data);
void  spizw_r_v_knopka(GtkWidget *widget,class spizw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

int spizw_r(class spizw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class spizw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Списание материалов изделия с карточек"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(spizw_r_key_press),&data);

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

repl.plus(gettext("Списание материалов изделия с карточек"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(spizw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)spizw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  spizw_r_v_knopka(GtkWidget *widget,class spizw_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spizw_r_key_press(GtkWidget *widget,GdkEventKey *event,class spizw_r_data *data)
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

/******************************************/
/******************************************/

gint spizw_r1(class spizw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
short ds,ms,gs;

//iceb_poldan("Н.Д.С.",bros,"matnast.alx",data->window);
//float pnds=iceb_u_atof(bros);
float pnds=iceb_pnds(data->window);

if(iceb_u_rsdat(&ds,&ms,&gs,data->rk->datas.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

//Проверяем код склада

sprintf(strsql,"select naik from Sklad where kod=%s",data->rk->sklad.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код склада"),data->rk->sklad.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

//Проверяем есть ли этот код операции в расходах
sprintf(strsql,"select kod from Rashod where kod='%s'",data->rk->kodop.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data->rk->kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(data->rk->nomdok.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён номер документа !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(data->rk->koliz.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Не введёно количество !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


//Проверяем номер документа
//Уникальный номер документа для прихода и расхода
sprintf(strsql,"select nomd from Dokummat where \
datd >= '%d-01-01' and datd <= '%d-12-31' and sklad=%s and nomd='%s'",
gs,gs,data->rk->sklad.ravno(),data->rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,data->window) >= 1)
 {
  sprintf(strsql,gettext("С номером %s документ уже есть !"),data->rk->nomdok.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



SQL_str row;

if(zap_s_mudokw(2,ds,ms,gs,data->rk->sklad.ravno_atoi(),
"00",data->rk->nomdok.ravno(),
"",data->rk->kodop.ravno(),0,0,"",0,0,0,1,data->window) != 0)
 {
  iceb_msql_error(&bd,"",strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 



double  kolndet=0.;
int     kodm;
double  kolih=0;

class iceb_razuz_data m_det;
m_det.kod_izd=data->rk->kod_izdel.ravno_atoi();
if((kolndet=iceb_razuz_kod(&m_det,data->window)) == 0)
 {
  sprintf(strsql,"%s %s !",gettext("Не найдено ни одной детали для изделия"),
  data->rk->kod_izdel.ravno());
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kolkar=0;
double cena=0.;
char shetu[32];
memset(shetu,'\0',sizeof(shetu));
char ei[32];
double ndss=pnds;
SQLCURSOR cur;
kolndet=m_det.kod_det_ei.kolih();

for(int sss=0; sss < kolndet; sss++)
 {
  if(m_det.metka_mu.ravno(sss) == 1)
   continue;

  memset(ei,'\0',sizeof(ei));
   
  iceb_u_polen(m_det.kod_det_ei.ravno(sss),&kodm,1,'|');
  iceb_u_polen(m_det.kod_det_ei.ravno(sss),ei,sizeof(ei),2,'|');
  kolih=m_det.kolih_det_ei.ravno(sss)*data->rk->koliz.ravno_atof();;

  cena=0.;
  //Читаем цену
  sprintf(strsql,"select cenapr from Material where kodm=%d",kodm);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    cena=atof(row[0]);



  iceb_u_int nomkars;
  iceb_u_double kolpriv;
    
  if((kolkar=findkarw(ds,ms,gs,data->rk->sklad.ravno_atoi(),kodm,kolih,&nomkars,&kolpriv,0,cena,shetu,data->window)) <= 0)
    zapvdokw(ds,ms,gs,data->rk->sklad.ravno_atoi(),0,kodm,data->rk->nomdok.ravno(),kolih,cena,ei,ndss,0,0,2,0,"","",data->window);
  else
   {
    double kolihpi=0.;
    for(int sht=0; sht < kolkar; sht++)
     {
      int nomkar=nomkars.ravno(sht);
      double kolihp=kolpriv.ravno(sht);
      kolihpi+=kolihp;
      zapvdokw(ds,ms,gs,data->rk->sklad.ravno_atoi(),nomkar,kodm,data->rk->nomdok.ravno(),kolihp,cena,ei,ndss,0,0,2,0,"","",data->window);
     }
    if(fabs(kolih-kolihpi) > 0.000001)
      zapvdokw(ds,ms,gs,data->rk->sklad.ravno_atoi(),0,kodm,data->rk->nomdok.ravno(),kolih-kolihpi,cena,ei,ndss,0,0,2,0,"","",data->window);
   }


 }

podtdok1w(ds,ms,gs,data->rk->sklad.ravno_atoi(),data->rk->nomdok.ravno(),2,ds,ms,gs,1,data->window);
podvdokw(ds,ms,gs,data->rk->nomdok.ravno(),data->rk->sklad.ravno_atoi(),data->window);






gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
