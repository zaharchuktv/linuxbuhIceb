/*$Id: zar_rbh_r.c,v 1.14 2011-03-28 06:55:46 sasa Exp $*/
/*20.11.2009	31.01.2007	Белых А.И.	zar_rbh_r.c
Расчёт частей зарплатных счетов
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class zar_rbh_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  const char *data_d;
  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;
    
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_rbh_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_rbh_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_rbh_r_data *data);
gint zar_rbh_r1(class zar_rbh_r_data *data);
void  zar_rbh_r_v_knopka(GtkWidget *widget,class zar_rbh_r_data *data);

void dolizs_rs(class iceb_u_int *knv_nalog_661,
class iceb_u_double *knv_nalog_suma_661,
class iceb_u_int *knv_nalog_663,
class iceb_u_double *knv_nalog_suma_663,
double itogo_suma_661,
double itogo_suma_663,
double suma_nalog,
FILE *ff,
GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern char *organ;

extern char *shetb; /*Бюджетные счета начислений*/
extern char *shetbu; /*Бюджетные счета удержаний*/

extern short *knvr;/*Коды начислений не входящие в расчет подоходного налога*/
//extern short kodpn;   /*Код подоходного налога*/
extern class iceb_u_str kodpn_all;
extern short kodpen;  /*Код пенсионных отчислений*/
extern short *kodnvpen; /*Коды не входящие в расчет пенсионного отчисления*/

extern short kodsocstr;  /*Код отчисления на соц-страх*/
extern short *kodsocstrnv; //Коды не входящие в расчет соцстраха

extern short *kodbzrnv; //Коды не входящие в расчет отчислений на безработицу
extern short kodbzr;  /*Код отчисления на безработицу*/

extern short kuprof; //Код удержания в профсоюзный фонд
extern short *knvrprof; //Коды не входящие в расчет удержаний в профсоюзный фонд

extern char shrpz[16]; /*Счет расчетов по зарплате*/
extern class iceb_u_spisok dop_zar_sheta; /*Список дополнительных зарплатных счетов*/
extern class iceb_u_spisok dop_zar_sheta_spn; 


int zar_rbh_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_rbh_r_data data;

data.data_d=data_d;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт частей зарплатных счетов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_rbh_r_key_press),&data);

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

repl.plus(gettext("Расчёт частей зарплатных счетов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_rbh_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_rbh_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_rbh_r_v_knopka(GtkWidget *widget,class zar_rbh_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_rbh_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_rbh_r_data *data)
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

/*****************************/
/*Расчёт по долям*/
/**********************/
void dolizs_rs(class iceb_u_int *knv_nalog_661, /*Коды не входящие в расчёт для налога*/
class iceb_u_double *knv_nalog_suma_661, /*Суммы не входящие в расчёт для основного счёта*/
class iceb_u_double *knv_nalog_suma_663, /*Суммы не входящие в расчёт для дополнительных счетов*/
double itogo_suma_661, /*Общая сумма начисления для главного счёта 661 вместе с кодами не входящими в расчёт*/
double *itogo_suma_663, /*Общие суммы начислений для дополнительных счетов вместе с кодами не входящими в расчёт*/
double suma_nalog,      /*Общая сумма налога*/
class iceb_u_spisok *dop_zar_sheta, /*Список дополнительных зарплатных счетов*/
class iceb_u_spisok *dop_zar_sheta_spn, /*Список начислений для которых применяются дополнительные зарплатные счета*/
class iceb_u_int *vse_kodi_nah, /*Список всех кодов начислений*/
FILE *ff,
GtkWidget *wpredok)
{
double suma=0.;
char naim[512];
char strsql[512];
SQL_str row;
SQLCURSOR cur;

int kolih_dop_shet=dop_zar_sheta->kolih();

double suma_po_dop_sh=0;
for(int ii=0; ii < kolih_dop_shet; ii++)
 suma_po_dop_sh+=itogo_suma_663[ii];
double itogo_nah=itogo_suma_661+suma_po_dop_sh;

fprintf(ff,"%s:%.2f\n%s:%.2f\n",
gettext("Общая сумма всех начислений"),
itogo_nah,
gettext("Общая сумма налога"),
suma_nalog);

double itogo_knv_nalog_661=0.;
if(knv_nalog_661->kolih() > 0)
 {
  fprintf(ff,"%s %s\n",gettext("Счёт"),shrpz);
  fprintf(ff,"%s:\n",gettext("Коды начислений, с которых не начислялся налог"));
  for(int i=0; i < knv_nalog_661->kolih(); i++)
   {
    suma=knv_nalog_suma_661->ravno(i);
    memset(naim,'\0',sizeof(naim));
    //читаем наименование начисления
    sprintf(strsql,"select naik from Nash where kod=%d",knv_nalog_661->ravno(i));
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     strncpy(naim,row[0],sizeof(naim)-1);     
    fprintf(ff,"%-3d %-*.*s %10.2f\n",knv_nalog_661->ravno(i),iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,suma);
    itogo_knv_nalog_661+=suma;
   }
  fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(34,gettext("Итого")),gettext("Итого"),itogo_knv_nalog_661);
 }
double suma_vr_661=itogo_suma_661-itogo_knv_nalog_661;

fprintf(ff,"\n%s %s -> %.2f-%.2f=%.2f\n",
gettext("Сумма с которой брался налог для счёта"),
shrpz,itogo_suma_661,itogo_knv_nalog_661,suma_vr_661);




itogo_suma_661-=itogo_knv_nalog_661;

double itogo_knv_nalog_663=0.;
int kolih_nah=vse_kodi_nah->kolih();
double suma_vr_dopsh[kolih_dop_shet];
memset(suma_vr_dopsh,'\0',sizeof(suma_vr_dopsh));
double i_suma_vr_dopsh=0.;
for(int ii=0; ii < kolih_dop_shet; ii++)
 {
  fprintf(ff,"\n%s %s\n-------------------------------------\n",gettext("Счёт"),dop_zar_sheta->ravno(ii));
  fprintf(ff,"%s:\n",gettext("Коды начислений, с которых не начислялся налог"));

  itogo_knv_nalog_663=0.;

  for(int i=0; i < kolih_nah; i++)
   {
    /*Проверяем есть ли это начисление в списке для этого счёта, чтобы не отнимать сумму не входящюю в расчёт*/
    if(iceb_u_proverka(dop_zar_sheta_spn->ravno(ii),vse_kodi_nah->ravno(i),0,1) != 0)
     continue;

    if((suma=knv_nalog_suma_663->ravno(ii*kolih_nah+i)) == 0.)
     continue;
  
    memset(naim,'\0',sizeof(naim));
    //читаем наименование начисления
    sprintf(strsql,"select naik from Nash where kod=%d",vse_kodi_nah->ravno(i));
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     strncpy(naim,row[0],sizeof(naim)-1);     
    fprintf(ff,"%-3d %-*.*s %10.2f\n",vse_kodi_nah->ravno(i),iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,suma);
    itogo_knv_nalog_663+=suma;
   }
  fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(34,gettext("Итого")),gettext("Итого"),itogo_knv_nalog_663);

  suma_vr_dopsh[ii]+=itogo_suma_663[ii]-itogo_knv_nalog_663;
  i_suma_vr_dopsh+=suma_vr_dopsh[ii];
  fprintf(ff,"Сумма с которой брался налог для %s счёта -> %.2f-%.2f=%.2f\n",
  dop_zar_sheta->ravno(ii),itogo_suma_663[ii],itogo_knv_nalog_663,suma_vr_dopsh[ii]);
 }


fprintf(ff,"\n%s:\n\
---------------------------------------------------------\n\
%*s %10.2f\n",
gettext("Общая сумма взятая в расчёт по всем счетам"),
iceb_u_kolbait(6,shrpz),shrpz,
suma_vr_661);

for(int ii=0; ii < kolih_dop_shet; ii++)
 fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(6,dop_zar_sheta->ravno(ii)),dop_zar_sheta->ravno(ii),suma_vr_dopsh[ii]);

fprintf(ff,"\
---------------------------------------------------------\n");

double suma_vr=i_suma_vr_dopsh+suma_vr_661;
fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(6,gettext("Итого")),gettext("Итого"),suma_vr);

fprintf(ff,"\n%s:\n",gettext("Расчитываем доли налога по счетам"));

fprintf(ff,"%s %6s",gettext("Счёт"),shrpz);
double nalog=suma_vr_661*suma_nalog/suma_vr;
nalog=iceb_u_okrug(nalog,0.01);

fprintf(ff," %.2f*%.2f/%.2f=%.2f\n",suma_vr_661,suma_nalog,suma_vr,nalog);
double inalog=nalog;
for(int ii=0; ii < kolih_dop_shet; ii++)
 {
  fprintf(ff,"%s %6s",gettext("Счёт"),dop_zar_sheta->ravno(ii));
  nalog=suma_vr_dopsh[ii]*suma_nalog/suma_vr;
  nalog=iceb_u_okrug(nalog,0.01);
  inalog+=nalog;
  fprintf(ff," %.2f*%.2f/%.2f=%.2f\n",suma_vr_dopsh[ii],suma_nalog,suma_vr,nalog);
 }
fprintf(ff,"%s:%.2f\n",gettext("Итого"),inalog);

}

/******************************************/
/******************************************/

gint zar_rbh_r1(class zar_rbh_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mr,gr;
iceb_u_rsdat1(&mr,&gr,data->data_d);

SQLCURSOR cur;
SQL_str row;
int kolstr=0;

class iceb_u_int sn661; //Список начислений для 661 счёта
sprintf(strsql,"select kod from Nash");

int kolih_nah=0;
if((kolih_nah=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolih_nah == 0)
 {
  iceb_menu_soob(gettext("Не введены начисления!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 
 }

int kod;
class iceb_u_int vse_kodi_nah; /*Список всех кодов начисления*/
int metka=0;

while(cur.read_cursor(&row) != 0)
 {
  kod=atoi(row[0]);
  vse_kodi_nah.plus(kod,-1);

  metka=0;
  for(int ii=0; ii < dop_zar_sheta_spn.kolih(); ii++)
   if(iceb_u_proverka(dop_zar_sheta_spn.ravno(ii),row[0],0,1) == 0)
    {
     metka=1;
     break;
    }
  if(metka == 0)
   sn661.plus(kod,-1);
 }

sprintf(strsql,"select tabn,prn,knah,suma,shet from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and suma <> 0. order by tabn asc",gr,mr,1,gr,mr,31);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

float kolstr1=0;
short prn=0;
double suma_podoh=0.;
double suma_pens=0.;
double suma_socstr=0.;
double suma_bezrab=0.;
double suma_profs=0.;
int knah=0;
double suma=0.;
int kolih_dop_shet=dop_zar_sheta.kolih();

class iceb_u_spisok knah_shet_663[kolih_dop_shet]; //Список начисление-счёт
class iceb_u_double suma_knah_shet_663[kolih_dop_shet]; //Суммы по начислениям-счетам


class iceb_u_double suma_nah_661;


class iceb_u_int knv_podoh_661; //коды не вошедшие в расчёт налога
class iceb_u_double knv_podoh_suma_661; //суммы по кодам не вошедшим в растчёт

class iceb_u_double knv_podoh_suma_663; //суммы по кодам не вошедшим в растчёт
knv_podoh_suma_663.make_class(kolih_dop_shet*kolih_nah);

class iceb_u_int knv_pens_661; //коды не вошедшие в расчёт налога
class iceb_u_double knv_pens_suma_661; //суммы по кодам не вошедшим в расчёт


class iceb_u_double knv_pens_suma_663; //суммы по кодам не вошедшим в растчёт
knv_pens_suma_663.make_class(kolih_dop_shet*kolih_nah);

class iceb_u_int knv_profs_661; //коды не вошедшие в расчёт налога
class iceb_u_double knv_profs_suma_661; //суммы по кодам не вошедшим в растчёт

class iceb_u_double knv_profs_suma_663; //суммы по кодам не вошедшим в растчёт
knv_profs_suma_663.make_class(kolih_dop_shet*kolih_nah);

class iceb_u_int knv_socstr_661; //коды не вошедшие в расчёт налога
class iceb_u_double knv_socstr_suma_661; //суммы по кодам не вошедшим в растчёт

class iceb_u_double knv_socstr_suma_663; //суммы по кодам не вошедшим в растчёт
knv_socstr_suma_663.make_class(kolih_dop_shet*kolih_nah);

class iceb_u_int knv_bezrab_661; //коды не вошедшие в расчёт налога
class iceb_u_double knv_bezrab_suma_661; //суммы по кодам не вошедшим в растчёт

class iceb_u_double knv_bezrab_suma_663; //суммы по кодам не вошедшим в растчёт
knv_bezrab_suma_663.make_class(kolih_dop_shet*kolih_nah);
suma_nah_661.make_class(sn661.kolih());
metka=0;
int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
  
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  prn=atoi(row[1]);
  knah=atoi(row[2]);  
  suma=atof(row[3]);
  
  if(prn == 1)  
   {
    if(iceb_u_proverka(shetb,row[4],0,1) == 0)
     continue; 
    
    if((nomer=sn661.find(knah)) >= 0)
      suma_nah_661.plus(suma,nomer);

    for(int ii=0 ; ii < kolih_dop_shet; ii++)
     if(iceb_u_proverka(dop_zar_sheta_spn.ravno(ii),row[2],0,1) == 0)
       {
        sprintf(strsql,"%d|%s",knah,row[4]);
        
        if((nomer=knah_shet_663[ii].find(strsql)) < 0)
         knah_shet_663[ii].plus(strsql);
        suma_knah_shet_663[ii].plus(suma,nomer);
       
       }

    if(provkodw(knvr,knah) >= 0) //проверка на коды не входящие в расчёт подоходного налога
     {
      metka=0;
      for(int ii=0 ; ii < kolih_dop_shet; ii++)
       {            
        if(iceb_u_proverka(dop_zar_sheta_spn.ravno(ii),row[2],0,1) == 0)
         {
          nomer=vse_kodi_nah.find(row[2]);
          knv_podoh_suma_663.plus(suma,ii*kolih_nah+nomer);
          metka=1;
         }
        
       }      

      if(metka == 0)
       {
        if((nomer=knv_podoh_661.find(knah)) < 0)
          knv_podoh_661.plus(knah,nomer);
        knv_podoh_suma_661.plus(suma,nomer);
       }
     }
    if(provkodw(kodnvpen,knah) >= 0) //проверка на коды не входящие в расчёт пенсионного
     {
      metka=0;
      for(int ii=0 ; ii < kolih_dop_shet; ii++)
       {            
        if(iceb_u_proverka(dop_zar_sheta_spn.ravno(ii),row[2],0,1) == 0)
         {
          nomer=vse_kodi_nah.find(row[2]);
          knv_pens_suma_663.plus(suma,ii*kolih_nah+nomer);
          metka=1;
         }
       }
      if(metka == 0)
       {
        if((nomer=knv_pens_661.find(knah)) < 0)
          knv_pens_661.plus(knah,nomer);
        knv_pens_suma_661.plus(suma,nomer);
       }
     }
    if(provkodw(knvrprof,knah) >= 0) //проверка на коды не входящие в расчёт профсоюзного взноса
     {
      metka=0;
      for(int ii=0 ; ii < kolih_dop_shet; ii++)
       {            
        if(iceb_u_proverka(dop_zar_sheta_spn.ravno(ii),row[2],0,1) == 0)
         {
          nomer=vse_kodi_nah.find(row[2]);
          knv_profs_suma_663.plus(suma,ii*kolih_nah+nomer);
          metka=1;
         }
       }
      if(metka == 0)
       {
        if((nomer=knv_profs_661.find(knah)) < 0)
          knv_profs_661.plus(knah,nomer);
        knv_profs_suma_661.plus(suma,nomer);
       }
     }

    if(provkodw(kodsocstrnv,knah) >= 0) //проверка на коды не входящие в расчёт соцстраха
     {
      metka=0;
      for(int ii=0 ; ii < kolih_dop_shet; ii++)
       {            
        if(iceb_u_proverka(dop_zar_sheta_spn.ravno(ii),row[2],0,1) == 0)
         {
          nomer=vse_kodi_nah.find(row[2]);
          knv_socstr_suma_663.plus(suma,ii*kolih_nah+nomer);
          metka=1;
         }
       }

      if(metka == 0)
       {
        if((nomer=knv_socstr_661.find(knah)) < 0)
          knv_socstr_661.plus(knah,nomer);
        knv_socstr_suma_661.plus(suma,nomer);
       }
     }

    if(provkodw(kodbzrnv,knah) >= 0) //проверка на коды не входящие в расчёт безработицы
     {
      metka=0;
      for(int ii=0 ; ii < kolih_dop_shet; ii++)
       {            
        if(iceb_u_proverka(dop_zar_sheta_spn.ravno(ii),row[2],0,1) == 0)
         {
          nomer=vse_kodi_nah.find(row[2]);
          knv_bezrab_suma_663.plus(suma,ii*kolih_nah+nomer);
          metka=1;
         }
       }
      if(metka == 0)
       {
        if((nomer=knv_bezrab_661.find(knah)) < 0)
          knv_bezrab_661.plus(knah,nomer);
        knv_bezrab_suma_661.plus(suma,nomer);
       }
     }
   }

  if(prn == 2)
   {
    if(iceb_u_proverka(shetbu,row[4],0,1) == 0)
     continue; 
    
//    if(kodpn == knah)
    if(iceb_u_proverka(kodpn_all.ravno(),knah,0,1) == 0)
     suma_podoh+=suma;
    
    if(kodpen == knah)
     suma_pens+=suma;
    
    if(kodsocstr == knah)
     suma_socstr+=suma;
    
    if(kodbzr == knah)
     suma_bezrab+=suma;
    
    if(kuprof == knah)
     suma_profs+=suma;
     
   } 
 }

char imaf[30];
FILE *ff;

sprintf(imaf,"doli%d.tmp",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(gettext("Расчет частей зарплатных счетов"),0,0,0,0,mr,gr,organ,ff);

char naim[512];

double itogo_suma_661=0.;

fprintf(ff,"%s %s\n",gettext("Начисления по счёту"),shrpz);
fprintf(ff,"%s: %d\n",gettext("Количество начислений"),sn661.kolih());
fprintf(ff,"----------------------------------------------------------\n");

for(int i=0; i < sn661.kolih(); i++)
 {
  suma=suma_nah_661.ravno(i);
  if(suma == 0.)
   continue;
  memset(naim,'\0',sizeof(naim));
  //читаем наименование начисления
  sprintf(strsql,"select naik from Nash where kod=%d",sn661.ravno(i));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);     
  fprintf(ff,"%-3d %-*.*s %10.2f\n",sn661.ravno(i),iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,suma);
  itogo_suma_661+=suma;  
 }

fprintf(ff,"----------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(34,gettext("Итого")),gettext("Итого"),itogo_suma_661);

double itogo_suma_663[kolih_dop_shet];
memset(itogo_suma_663,'\0',sizeof(itogo_suma_663));
double itogo=0.;
for(int ii=0; ii < kolih_dop_shet; ii++)
 {
  double suma_po_shetu=0.;
  fprintf(ff,"\n%s %s\n",gettext("Начисления по счёту"),dop_zar_sheta.ravno(ii));


  if(knah_shet_663[ii].kolih() > 0)
   {
    fprintf(ff,"%s: %d\n",gettext("Количество начислений"),knah_shet_663[ii].kolih());
    fprintf(ff,"----------------------------------------------------------\n");
    char shet_n[32];
    for(int i=0; i < knah_shet_663[ii].kolih(); i++)
     {
        
      suma=suma_knah_shet_663[ii].ravno(i);
      if(suma == 0.)
       continue;

      iceb_u_polen(knah_shet_663[ii].ravno(i),strsql,sizeof(strsql),1,'|');
      knah=atoi(strsql);

      iceb_u_polen(knah_shet_663[ii].ravno(i),shet_n,sizeof(shet_n),2,'|');
      
      memset(naim,'\0',sizeof(naim));
      //читаем наименование начисления
      sprintf(strsql,"select naik from Nash where kod=%d",knah);
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       strncpy(naim,row[0],sizeof(naim)-1);     
      fprintf(ff,"%-3d %-6s %-*.*s %10.2f\n",knah,shet_n,iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,suma);
      itogo_suma_663[ii]+=suma;  
      itogo+=suma;
      suma_po_shetu+=suma;
     }
    fprintf(ff,"----------------------------------------------------------\n");

    fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),suma_po_shetu);

   }
 }

fprintf(ff,"\n\
%s:%10.2f\n",gettext("Общая сумма начислений по всем счетам"),itogo_suma_661+itogo);

suma_podoh*=-1;
suma_pens*=-1;
suma_profs*=-1;
suma_bezrab*=-1;
suma_socstr*=-1;

fprintf(ff,"\n\
%s:%10.2f\n\
%s:%10.2f\n\
%s:%10.2f\n\
%s:%10.2f\n\
%s:%10.2f\n",
gettext("Общая сумма начисления подоходного налога"),suma_podoh,
gettext("Общая сумма начисления в пенсионный фонд"),suma_pens,
gettext("Общая сумма начисления профсоюзного отчисления"),suma_profs,
gettext("Общая сумма начисления в фонд безработицы"),suma_bezrab,
gettext("Общая сумма начисления в фонд соц-страха"),suma_socstr);

fprintf(ff,"\n***************%s**************\n",gettext("Подоходный налог"));
dolizs_rs(&knv_podoh_661,&knv_podoh_suma_661,&knv_podoh_suma_663,itogo_suma_661,itogo_suma_663,suma_podoh,&dop_zar_sheta,&dop_zar_sheta_spn,&vse_kodi_nah,ff,data->window);

fprintf(ff,"\n***************%s**************\n",gettext("Пенсионный фонд"));
dolizs_rs(&knv_pens_661,&knv_pens_suma_661,&knv_pens_suma_663,itogo_suma_661,itogo_suma_663,suma_pens,&dop_zar_sheta,&dop_zar_sheta_spn,&vse_kodi_nah,ff,data->window);

fprintf(ff,"\n***************%s**************\n",gettext("Профсоюзный взнос"));
dolizs_rs(&knv_profs_661,&knv_profs_suma_661,&knv_profs_suma_663,itogo_suma_661,itogo_suma_663,suma_profs,&dop_zar_sheta,&dop_zar_sheta_spn,&vse_kodi_nah,ff,data->window);


fprintf(ff,"\n***************%s**************\n",gettext("Соц. страхование (с работника)"));
dolizs_rs(&knv_socstr_661,&knv_socstr_suma_661,&knv_socstr_suma_663,itogo_suma_661,itogo_suma_663,suma_socstr,&dop_zar_sheta,&dop_zar_sheta_spn,&vse_kodi_nah,ff,data->window);

fprintf(ff,"\n***************%s**************\n",gettext("Фонд безработицы"));
dolizs_rs(&knv_bezrab_661,&knv_bezrab_suma_661,&knv_bezrab_suma_663,itogo_suma_661,itogo_suma_663,suma_bezrab,&dop_zar_sheta,&dop_zar_sheta_spn,&vse_kodi_nah,ff,data->window);

iceb_podpis(ff,data->window);

fclose(ff);

data->imaf->new_plus(imaf);
data->naimf->new_plus(gettext("Расчёт частей зарплатных счетов"));

iceb_ustpeh(imaf,0,data->window);


data->kon_ras=0;
data->voz=0;
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
