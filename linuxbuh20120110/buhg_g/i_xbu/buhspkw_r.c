/*$Id: buhspkw_r.c,v 1.10 2011-02-21 07:35:51 sasa Exp $*/
/*27.11.2009	10.09.2008	Белых А.И.	buhspkw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhspkw.h"

class akt_sverki_po_grup //Для расчёта акта сверки по группе контрагента
 {
  public:
   class iceb_u_spisok data_nomd_kontr; //Дата|номер документа|код контрагента
   class iceb_u_spisok koment;    //комментарий
   class iceb_u_double suma_deb; 
   class iceb_u_double suma_kre; 
   class iceb_u_double start_saldo_deb; //сальдо по каждому контрагенту
   class iceb_u_double start_saldo_kre;
   char shet[32];  

   class iceb_u_spisok spisok_kontr; //Cписок контрагентов в группе
     
  akt_sverki_po_grup()
   {
    clear();    
   }
  void clear()
   {
    start_saldo_deb.clear_class();
    start_saldo_kre.clear_class();
    memset(shet,'\0',sizeof(shet));
    suma_deb.free_class();
    suma_kre.free_class();
    data_nomd_kontr.free_class();
    koment.free_class();    
   }
 };

class buhspkw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class buhspkw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  buhspkw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhspkw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhspkw_r_data *data);
gint buhspkw_r1(class buhspkw_r_data *data);
void  buhspkw_r_v_knopka(GtkWidget *widget,class buhspkw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern short	startgodb; /*Стартовый год*/
extern int      kol_strok_na_liste;

int buhspkw_r(class buhspkw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class buhspkw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт акта сверки по группе контрагентов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(buhspkw_r_key_press),&data);

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

repl.plus(gettext("Расчёт акта сверки по группе контрагентов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(buhspkw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)buhspkw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhspkw_r_v_knopka(GtkWidget *widget,class buhspkw_r_data *data)
{
//printf("buhspkw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   buhspkw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhspkw_r_data *data)
{
// printf("buhspkw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}

/*****************************/
/*Расчёт по счёту*/
/****************************/
void buhspk_rsh(short dn,short mn,short gn,
short dk,short mk,short gk,
class akt_sverki_po_grup *akt_sv,
GtkWidget *wpredok)
{
char strsql[512];
int kolstr;
SQL_str row;
class SQLCURSOR cur;

short d=1,m=1,g=gn;
if(startgodb != 0 && gn > startgodb)
 g=startgodb;

//читаем сальдо по контрагентам
sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk=1 and gs=%d and ns='%s'",
gn,akt_sv->shet);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

int nomer_kontr=0;

while(cur.read_cursor(&row) != 0)
 {
  if((nomer_kontr=akt_sv->spisok_kontr.find(row[0])) < 0)
   continue;

  akt_sv->start_saldo_deb.plus(atof(row[1]),nomer_kontr);
  akt_sv->start_saldo_kre.plus(atof(row[2]),nomer_kontr);
  
 }  



int nomer_prov=0;
char koment[512];

while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  sprintf(strsql,"select kodkon,nomd,deb,kre,komen from Prov where datp='%04d-%02d-%02d' and sh='%s'",g,m,d,akt_sv->shet);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }
  
  
  while(cur.read_cursor(&row) != 0)
   {
    if((nomer_kontr=akt_sv->spisok_kontr.find(row[0])) < 0)
     continue;

    if(iceb_u_sravmydat(dn,mn,gn,d,m,g) > 0)
     {
      akt_sv->start_saldo_deb.plus(atof(row[2]),nomer_kontr);
      akt_sv->start_saldo_kre.plus(atof(row[3]),nomer_kontr);
      continue;     
     }
    
    memset(koment,'\0',sizeof(koment));
    strcpy(koment,row[1]);
    strcat(koment," ");
    strcat(koment,row[4]);

    sprintf(strsql,"%d.%d.%d|%s|%s",d,m,g,row[1],row[0]);


    if(row[1][0] == '\0')
       nomer_prov=-1;
    else
       nomer_prov=akt_sv->data_nomd_kontr.find(strsql);

    if(nomer_prov < 0)
     {
      akt_sv->data_nomd_kontr.plus(strsql);
      if(iceb_u_strstrm(koment,"-ПДВ") == 1) //есть образец в строке
       {
        iceb_u_polen(koment,strsql,sizeof(strsql),1,'-');
        akt_sv->koment.plus(strsql);
        
       }
      else
        akt_sv->koment.plus(koment);
     }

    akt_sv->suma_deb.plus(atof(row[2]),nomer_prov);        
    akt_sv->suma_kre.plus(atof(row[3]),nomer_prov);        
   }  

  iceb_u_dpm(&d,&m,&g,1);
 }




}
/*********************/
/*Выдача итога*/
/*********************/

void buhspk_it(double deb_start,double kre_start,
double deb_oborot,double kre_oborot,
double deb_ob_mes,double kre_ob_mes,
FILE *ff)
{

double brr=deb_start+deb_oborot;
double krr=kre_start+kre_oborot;

if(deb_ob_mes != 0. || kre_ob_mes != 0.)
 {
  fprintf(ff,"%*s %10.2f %10.2f| |\n",
  iceb_u_kolbait(31,gettext("Оборот за месяц")),gettext("Оборот за месяц"),deb_ob_mes,kre_ob_mes);
 }

fprintf(ff,"%*s %10.2f %10.2f| |\n",
iceb_u_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),deb_oborot,kre_oborot);


fprintf(ff,"%*s %10.2f %10.2f| |\n",
iceb_u_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),brr,krr);

if(brr >= krr)
 {
  brr=brr-krr;
  fprintf(ff,"%*s %10.2f %10s| |\n",
  iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),brr," ");
 }
else
 {
  brr=krr-brr;
  fprintf(ff,"%*s %10s %10.2f| |\n",
  iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",brr);
 }
}

/******************************/
/*Распечатка целиком по cчёту*/
/**************************/
void buhspk_rg(short dn,short mn,short gn,
class akt_sverki_po_grup *akt_sv,FILE *ff,
GtkWidget *wpredok)
{

iceb_zagsh(akt_sv->shet,ff,wpredok);

double start_deb=0.;
double start_kre=0.;

start_deb=akt_sv->start_saldo_deb.suma();
start_kre=akt_sv->start_saldo_kre.suma();

if(start_deb > start_kre)
  fprintf(ff,"%02d.%02d.%4d %-*.*s %10.2f %10s| |\n",
  dn,mn,gn,
  iceb_u_kolbait(20,gettext("Сальдо начальное")),iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
  start_deb-start_kre," ");
else
 fprintf(ff,"%02d.%02d.%4d %-*.*s %10s %10.2f| |\n",
 dn,mn,gn,
 iceb_u_kolbait(20,gettext("Сальдо начальное")),iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
 " ",start_kre-start_deb);


fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
short d,m,g;
short mz=0,gz=0;
double deb_oborot=0.;
double kre_oborot=0.;
double deb_ob_mes=0.;
double kre_ob_mes=0.;
double deb,kre;
 
for(int ii=0; ii < akt_sv->data_nomd_kontr.kolih(); ii++)
 {
  iceb_u_rsdat(&d,&m,&g,akt_sv->data_nomd_kontr.ravno(ii),1);
  if(mz != m || gz != g)
   {
    if(mz != 0)
       buhspk_it(start_deb,start_kre,deb_oborot,kre_oborot,deb_ob_mes,kre_ob_mes,ff);
    deb_ob_mes=kre_ob_mes=0.;    
    mz=m;
    gz=g;
   }     
  
  deb=akt_sv->suma_deb.ravno(ii);
  kre=akt_sv->suma_kre.ravno(ii);
  
  fprintf(ff,"%02d.%02d.%4d %-*.*s %10.2f %10.2f| |\n",
  d,m,g,
  iceb_u_kolbait(20,akt_sv->koment.ravno(ii)),iceb_u_kolbait(20,akt_sv->koment.ravno(ii)),akt_sv->koment.ravno(ii),
  deb,kre);

  fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
  deb_oborot+=deb;
  kre_oborot+=kre;
  deb_ob_mes+=deb;
  kre_ob_mes+=kre;
 }

buhspk_it(start_deb,start_kre,deb_oborot,kre_oborot,deb_ob_mes,kre_ob_mes,ff);



}

/******************************/
/*Распечатка по cчёту в разре по контрагентам*/
/**************************/
void buhspk_rg_k(int metka_mi, /*0-с месячными итогами 1-буз*/
short dn,short mn,short gn,
class akt_sverki_po_grup *akt_sv,FILE *ff,
GtkWidget *wpredok)
{

iceb_zagsh(akt_sv->shet,ff,wpredok);

double start_deb=0.;
double start_kre=0.;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
char naim_kontr[512];
char kod_kontr[50];
double i_start_deb=0.;
double i_start_kre=0.;
double i_oborot_deb=0.;
double i_oborot_kre=0.;
int kolih_r_k=0;
for(int kkk=0; kkk < akt_sv->spisok_kontr.kolih(); kkk++)
 {
  //проверяем есть ли этот код в списке счёта
  sprintf(strsql,"select ns from Skontr where ns='%s' and kodkon='%s'",akt_sv->shet,akt_sv->spisok_kontr.ravno(kkk));
  if(iceb_sql_readkey(strsql,wpredok) != 1)
   continue;
   
  memset(naim_kontr,'\0',sizeof(naim_kontr));
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",akt_sv->spisok_kontr.ravno(kkk));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim_kontr,row[0],sizeof(naim_kontr)-1);    
  
  fprintf(ff,"\n%s %s\n",akt_sv->spisok_kontr.ravno(kkk),naim_kontr);
  
  start_deb=akt_sv->start_saldo_deb.ravno(kkk);
  start_kre=akt_sv->start_saldo_kre.ravno(kkk);

  if(start_deb > start_kre)
    fprintf(ff,"%02d.%02d.%4d %-*.*s %10.2f %10s| |\n",
    dn,mn,gn,
    iceb_u_kolbait(20,gettext("Сальдо начальное")),iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
    start_deb-start_kre," ");
  else
   fprintf(ff,"%02d.%02d.%4d %-*.*s %10s %10.2f| |\n",
   dn,mn,gn,
   iceb_u_kolbait(20,gettext("Сальдо начальное")),iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
   " ",start_kre-start_deb);


  fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
  short d,m,g;
  short mz=0,gz=0;
  double deb_oborot=0.;
  double kre_oborot=0.;
  double deb_ob_mes=0.;
  double kre_ob_mes=0.;
  double deb,kre;
   
  for(int ii=0; ii < akt_sv->data_nomd_kontr.kolih(); ii++)
   {
    iceb_u_polen(akt_sv->data_nomd_kontr.ravno(ii),kod_kontr,sizeof(kod_kontr),3,'|');
    if(iceb_u_SRAV(kod_kontr,akt_sv->spisok_kontr.ravno(kkk),0) != 0)
     continue;
    
    iceb_u_rsdat(&d,&m,&g,akt_sv->data_nomd_kontr.ravno(ii),1);
    if(mz != m || gz != g)
     {
      if(metka_mi == 0)
       if(mz != 0)
         buhspk_it(start_deb,start_kre,deb_oborot,kre_oborot,deb_ob_mes,kre_ob_mes,ff);
      deb_ob_mes=kre_ob_mes=0.;    
      mz=m;
      gz=g;
     }     
    
    deb=akt_sv->suma_deb.ravno(ii);
    kre=akt_sv->suma_kre.ravno(ii);
    
    fprintf(ff,"%02d.%02d.%4d %-*.*s %10.2f %10.2f| |\n",
    d,m,g,
    iceb_u_kolbait(20,akt_sv->koment.ravno(ii)),iceb_u_kolbait(20,akt_sv->koment.ravno(ii)),akt_sv->koment.ravno(ii),
    deb,kre);

    fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
    deb_oborot+=deb;
    kre_oborot+=kre;
    deb_ob_mes+=deb;
    kre_ob_mes+=kre;
   }
  if(metka_mi == 1)
    deb_ob_mes=kre_ob_mes=0.;
  buhspk_it(start_deb,start_kre,deb_oborot,kre_oborot,deb_ob_mes,kre_ob_mes,ff);
  kolih_r_k++;
  i_oborot_deb+=deb_oborot;
  i_oborot_kre+=kre_oborot;
  i_start_deb+=start_deb;
  i_start_kre+=start_kre;
  
 }

if(kolih_r_k > 1)
 {
  fprintf(ff,"%s %s:\n",gettext("Общий итог по счёту"),akt_sv->shet);
  fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");

  if(i_start_deb > i_start_kre)
   {
    fprintf(ff,"%*s %10.2f %10s| |\n",
    iceb_u_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное"),i_start_deb-i_start_kre," ");
   }
  else
   {
    fprintf(ff,"%*s %10s %10.2f| |\n",
    iceb_u_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",i_start_kre-i_start_deb);
   }

  buhspk_it(i_start_deb,i_start_kre,i_oborot_deb,i_oborot_kre,0.,0.,ff);
 }

}



/******************************************/
/******************************************/

gint buhspkw_r1(class buhspkw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);



int kolstr=0;
class akt_sverki_po_grup akt_sv;
char naim_gr_kontr[100];

//Проверяем код группы контрагента
sprintf(strsql,"select naik from Gkont where kod=%d",data->rk->kodgr.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код группы"),data->rk->kodgr.ravno());
  iceb_menu_soob(strsql,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

strncpy(naim_gr_kontr,row[0],sizeof(naim_gr_kontr)-1);

//создаём список контрагентов 
sprintf(strsql,"select kodkon from Kontragent where grup='%s'",data->rk->kodgr.ravno());
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s !",gettext("Нет ни одного контрагента в группе"),data->rk->kodgr.ravno());
  iceb_menu_soob(strsql,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  akt_sv.spisok_kontr.plus(row[0]);
 }

akt_sv.start_saldo_deb.make_class(akt_sv.spisok_kontr.kolih());
akt_sv.start_saldo_kre.make_class(akt_sv.spisok_kontr.kolih());

//Создаём список счетов 
sprintf(strsql,"select ns,kodkon from Skontr");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s !",gettext("Нет ни одного контрагента в списках счетов!"),data->rk->kodgr.ravno());
  iceb_menu_soob(strsql,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok spisok_shet;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->shet.ravno(),row[0],0,0) != 0)
   continue;
   
  if(akt_sv.spisok_kontr.find(row[1]) != 0)
   continue;

  spisok_shet.plus(row[0]);
 }

FILE *ff;
char imaf[56];
sprintf(imaf,"aspg%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 
 }

iceb_zagacts(dn,mn,gn,dk,mk,gk,data->rk->kodgr.ravno(),naim_gr_kontr,ff);

FILE *ff_k;
char imaf_k[56];
sprintf(imaf_k,"aspgk%d.lst",getpid());
if((ff_k=fopen(imaf_k,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_k,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 
 }
iceb_zagacts(dn,mn,gn,dk,mk,gk,data->rk->kodgr.ravno(),naim_gr_kontr,ff_k);

FILE *ff_kbmi;
char imaf_kbmi[56];
sprintf(imaf_kbmi,"aspgk%d.lst",getpid());
if((ff_kbmi=fopen(imaf_kbmi,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_kbmi,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 
 }

iceb_zagacts(dn,mn,gn,dk,mk,gk,data->rk->kodgr.ravno(),naim_gr_kontr,ff_kbmi);


kolstr1=0;

double start_deb=0.,start_kre=0.;
double oborot_deb=0.,oborot_kre=0.;

for(int ii=0; ii < spisok_shet.kolih(); ii++)
 {
  iceb_pbar(data->bar,spisok_shet.kolih(),++kolstr1);    
  strncpy(akt_sv.shet,spisok_shet.ravno(ii),sizeof(akt_sv.shet)-1);  
  buhspk_rsh(dn,mn,gn,dk,mk,gk,&akt_sv,data->window);

  buhspk_rg(dn,mn,gn,&akt_sv,ff,data->window);
  buhspk_rg_k(0,dn,mn,gn,&akt_sv,ff_k,data->window);
  buhspk_rg_k(1,dn,mn,gn,&akt_sv,ff_kbmi,data->window);

  start_deb+=akt_sv.start_saldo_deb.suma();
  start_kre+=akt_sv.start_saldo_kre.suma();

  oborot_deb+=akt_sv.suma_deb.suma();
  oborot_kre+=akt_sv.suma_kre.suma();
  
  akt_sv.clear();
 }

if(spisok_shet.kolih() > 1)
 {
  fprintf(ff,"\n%s:\n",gettext("Общий итог"));
  fprintf(ff_k,"\n%s:\n",gettext("Общий итог"));
  fprintf(ff_kbmi,"\n%s:\n",gettext("Общий итог"));

  fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
  fprintf(ff_k,"\
------------------------------------------------------ ---------------------------------------------------\n");
  fprintf(ff_kbmi,"\
------------------------------------------------------ ---------------------------------------------------\n");

  if(start_deb > start_kre)
   {
    fprintf(ff,"%*s %10.2f %10s| |\n",
    iceb_u_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное"),start_deb-start_kre," ");
    fprintf(ff_k,"%*s %10.2f %10s| |\n",
    iceb_u_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное"),start_deb-start_kre," ");
    fprintf(ff_kbmi,"%*s %10.2f %10s| |\n",
    iceb_u_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное"),start_deb-start_kre," ");
   }
  else
   {
    fprintf(ff,"%*s %10s %10.2f| |\n",
    iceb_u_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",start_kre-start_deb);
    fprintf(ff_k,"%*s %10s %10.2f| |\n",
    iceb_u_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",start_kre-start_deb);
    fprintf(ff_kbmi,"%*s %10s %10.2f| |\n",
    iceb_u_kolbait(31,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",start_kre-start_deb);
   }

  fprintf(ff,"%*s %10.2f %10.2f| |\n",
  iceb_u_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),oborot_deb,oborot_kre);

  fprintf(ff_k,"%*s %10.2f %10.2f| |\n",
  iceb_u_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),oborot_deb,oborot_kre);

  fprintf(ff_kbmi,"%*s %10.2f %10.2f| |\n",
  iceb_u_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),oborot_deb,oborot_kre);

  fprintf(ff,"%*s %10.2f %10.2f| |\n",
  iceb_u_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),start_deb+oborot_deb,start_kre+oborot_kre);
  fprintf(ff_k,"%*s %10.2f %10.2f| |\n",
  iceb_u_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),start_deb+oborot_deb,start_kre+oborot_kre);

  fprintf(ff_kbmi,"%*s %10.2f %10.2f| |\n",
  iceb_u_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),start_deb+oborot_deb,start_kre+oborot_kre);

  if(start_deb+oborot_deb > start_kre+oborot_kre)
   {
    fprintf(ff,"%*s %10.2f %10s| |\n",
    iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),
    start_deb+oborot_deb - start_kre-oborot_kre," ");

    fprintf(ff_k,"%*s %10.2f %10s| |\n",
    iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),
    start_deb+oborot_deb - start_kre-oborot_kre," ");

    fprintf(ff_kbmi,"%*s %10.2f %10s| |\n",
    iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),
    start_deb+oborot_deb - start_kre-oborot_kre," ");
   }
  else
   {
    fprintf(ff,"%*s %10s %10.2f| |\n",iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",start_kre+oborot_kre-start_deb-oborot_deb);
    fprintf(ff_k,"%*s %10s %10.2f| |\n",iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",start_kre+oborot_kre-start_deb-oborot_deb);
    fprintf(ff_kbmi,"%*s %10s %10.2f| |\n",iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",start_kre+oborot_kre-start_deb-oborot_deb);

   }
 }
  
iceb_konact(ff,data->window);
fclose(ff);

iceb_konact(ff_k,data->window);
fclose(ff_k);

iceb_konact(ff_kbmi,data->window);
fclose(ff_kbmi);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Акт сверки по группе контрагентов"));
data->rk->imaf.plus(imaf_k);
data->rk->naimf.plus(gettext("Акт сверки по группе контрагентов с общими суммами по документам"));
data->rk->imaf.plus(imaf_kbmi);
data->rk->naimf.plus(gettext("Акт сверки с общими суммами по документам без месячных итогов"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
  iceb_ustpeh(data->rk->imaf.ravno(nom),1,data->window);
  
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
