/*$Id: upl_ovdt_r.c,v 1.9 2011-02-21 07:35:58 sasa Exp $*/
/*17.11.2009	02.04.2008	Белых А.И.	upl_ovdt_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "upl_ovdt.h"
#define KOLIH_KOLONOK 5 //Количество вычисляемых колонок в отчёте. конечный остаток вычисляется по предыдущим колонкам
#define KOLIH_KOLONOK_MT 3 //Количество вычисляемых колонок в отчёте для марок топлива

class upl_ovdt_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class upl_ovdt_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  upl_ovdt_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_ovdt_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_ovdt_r_data *data);
gint upl_ovdt_r1(class upl_ovdt_r_data *data);
void  upl_ovdt_r_v_knopka(GtkWidget *widget,class upl_ovdt_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern short	startgodupl; //Стартовый год для "Учета путевых листов"

int upl_ovdt_r(class upl_ovdt_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class upl_ovdt_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка оборотной ведомости движения топлива"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(upl_ovdt_r_key_press),&data);

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

repl.plus(gettext("Распечатка оборотной ведомости движения топлива"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(upl_ovdt_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)upl_ovdt_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_ovdt_r_v_knopka(GtkWidget *widget,class upl_ovdt_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   upl_ovdt_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_ovdt_r_data *data)
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

/********************************************/
/*Распечатка массивов*/
/**********************************/
void  upl_obv_rs(int metka, //0- по автомобилям 1-по водителям
short dn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_int *kod,
double suma[][KOLIH_KOLONOK],
const char *kodvod,
const char *kodavt,
const char *imaf,
GtkWidget *wpredok)
{

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
 

char strsql[512];
if(metka == 0)
 strcpy(strsql,gettext("Оборотная ведомость движения топлива по автомобилям"));
if(metka == 1)
 strcpy(strsql,gettext("Оборотная ведомость движения топлива по водителям"));

iceb_u_zagolov(strsql,dn,mn,gn,dk,mk,gk,organ,ff);

if(metka == 0)
 {
  if(kodavt[0] != '\0')
   fprintf(ff,"%s:%s\n",gettext("Код автомобиля"),kodavt);
  fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
Код |           Автомобиль          | Остаток  |    Движение топлива за период             | Остаток  |\n\
авт.|      Марка    |Государс. номер|%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
  dn,mn,gn,dk,mk,gk);
/*********
1234 123456789012345 123456789012345 1234567890     1234567890
*************/
  fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
 }

if(metka == 1)
 {
  if(kodvod[0] != '\0')
   fprintf(ff,"%s:%s\n",gettext("Код водителя"),kodvod);
  fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код |       Фамилия водителя        | Остаток  |           Движение топлива за период      | Остаток  |\n\
вод.|                               |%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
  dn,mn,gn,dk,mk,gk);
/******
1234 123456789012345678901234567890
*************/
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
 }



double ostatok;
int metka_zero;
char naim[512];
SQL_str row;
SQLCURSOR cur;
double itogo[6];
memset(&itogo,'\0',sizeof(itogo));

for(int i=0; i < kod->kolih(); i++)
 {
  metka_zero=0;
  for(int ii=0; ii < KOLIH_KOLONOK ; ii++)
    if(suma[i][ii] != 0)
     {
      metka_zero=1;
      break;
     }    
  if(metka_zero == 0)
    continue;   

  
  memset(naim,'\0',sizeof(naim));
  if(metka == 0)
   {
    if(iceb_u_proverka(kodavt,kod->ravno_char(i),0,0) != 0)
     continue;
    sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",kod->ravno(i)); 
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      sprintf(naim,"%-15.15s %-15.15s",row[1],row[0]);
   }
  if(metka == 1)
   {
    if(iceb_u_proverka(kodvod,kod->ravno_char(i),0,0) != 0)
     continue;
    sprintf(strsql,"select naik from Uplouot where kod=%d",kod->ravno(i)); 
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      strncpy(naim,row[0],sizeof(naim)-1);
   }

  suma[i][0]=iceb_u_okrug(suma[i][0],0.001);
  suma[i][1]=iceb_u_okrug(suma[i][1],0.001);
  suma[i][2]=iceb_u_okrug(suma[i][2],0.001);
  suma[i][3]=iceb_u_okrug(suma[i][3],0.001);
  suma[i][4]=iceb_u_okrug(suma[i][4],0.001);
  
  ostatok=suma[i][0]+suma[i][1]+suma[i][2]-suma[i][3]-suma[i][4];
  ostatok=iceb_u_okrug(ostatok,0.001);
  
  fprintf(ff,"%4d %-*.*s %10.6g %10.6g %10.6g %10.6g %10.6g %10.6g\n",
  kod->ravno(i),
  iceb_u_kolbait(31,naim),iceb_u_kolbait(31,naim),naim,
  suma[i][0],suma[i][1],suma[i][2],suma[i][3],suma[i][4],ostatok);

  itogo[0]+=suma[i][0];
  itogo[1]+=suma[i][1];
  itogo[2]+=suma[i][2];
  itogo[3]+=suma[i][3];
  itogo[4]+=suma[i][4];
  itogo[5]+=ostatok;

 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%4s %*.*s %10.6g %10.6g %10.6g %10.6g %10.6g %10.6g\n",
"",
iceb_u_kolbait(31,gettext("Итого")),iceb_u_kolbait(31,gettext("Итого")),gettext("Итого"),
itogo[0],itogo[1],itogo[2],itogo[3],itogo[4],itogo[5]);

iceb_podpis(ff,wpredok);


fclose(ff);


}
/******************************/
/*Распечатка по маркам топлива*/
/*******************************/
void  upl_obv_rs_mt(short dn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_spisok *kod,
double suma[][KOLIH_KOLONOK_MT],
char *imaf,
GtkWidget *wpredok)
{

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
 

char strsql[512];

iceb_u_zagolov(gettext("Оборотная ведомость движения по маркам топлива"),dn,mn,gn,dk,mk,gk,organ,ff);

fprintf(ff,"\
------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
   Код  |    Наименование топлива      | Остаток  |Движение за период   | Остаток  |\n\
топлива |                              |%02d.%02d.%04d| Получено |Потрачено |%02d.%02d.%04d|\n"),
dn,mn,gn,dk,mk,gk);

/******
12345678 123456789012345678901234567890
*************/
fprintf(ff,"\
------------------------------------------------------------------------------------\n");



double ostatok;
int metka_zero;
char naim[512];
SQL_str row;
SQLCURSOR cur;
double itogo[4];
memset(&itogo,'\0',sizeof(itogo));

for(int i=0; i < kod->kolih(); i++)
 {
  metka_zero=0;
  for(int ii=0; ii < KOLIH_KOLONOK_MT ; ii++)
    if(suma[i][ii] != 0)
     {
      metka_zero=1;
      break;
     }    
  if(metka_zero == 0)
    continue;   

  
  memset(naim,'\0',sizeof(naim));
//    if(iceb_u_proverka(kodavt,kod->ravno_char(i),0,0) != 0)
//     continue;
  sprintf(strsql,"select kodm from Uplmt where kodt='%s'",kod->ravno(i)); 
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    sprintf(strsql,"select naimat from Material where kodm=%s",row[0]); 
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      strncpy(naim,row[0],sizeof(naim)-1);
   }
  ostatok=suma[i][0]+suma[i][1]-suma[i][2];

  fprintf(ff,"%-*s %-*.*s %10.6g %10.6g %10.6g %10.6g\n",
  iceb_u_kolbait(8,kod->ravno(i)),kod->ravno(i),
  iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,
  suma[i][0],suma[i][1],suma[i][2],ostatok);

  itogo[0]+=suma[i][0];
  itogo[1]+=suma[i][1];
  itogo[2]+=suma[i][2];
  itogo[3]+=ostatok;

 }

fprintf(ff,"\
------------------------------------------------------------------------------------\n");

fprintf(ff,"%8s %*.*s %10.6g %10.6g %10.6g %10.6g\n",
"",
iceb_u_kolbait(30,gettext("Итого")),iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),
itogo[0],itogo[1],itogo[2],itogo[3]);

iceb_podpis(ff,wpredok);


fclose(ff);


}


/******************************************/
/******************************************/

gint upl_ovdt_r1(class upl_ovdt_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;


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



int kolstr;
class iceb_u_int m_kod_avt;
class iceb_u_int m_kod_vod;
class iceb_u_spisok m_kod_mt;

short godn=startgodupl;
if(godn > gn || godn == 0)
 godn=gn;
 
sprintf(strsql,"select kod from Uplavt");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

double m_suma_avt[kolstr][KOLIH_KOLONOK];
memset(m_suma_avt,'\0',sizeof(m_suma_avt));

while(cur.read_cursor(&row) != 0)
 m_kod_avt.plus(atoi(row[0]),-1);

sprintf(strsql,"select kod from Uplouot where mt=1");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

double m_suma_vod[kolstr][KOLIH_KOLONOK];
memset(m_suma_vod,'\0',sizeof(m_suma_vod));
while(cur.read_cursor(&row) != 0)
 m_kod_vod.plus(atoi(row[0]),-1);

sprintf(strsql,"select kodt from Uplmt");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

double m_suma_mt[kolstr][KOLIH_KOLONOK_MT];
memset(m_suma_mt,'\0',sizeof(m_suma_mt));
while(cur.read_cursor(&row) != 0)
 m_kod_mt.plus(row[0]);


//смотрим сальдо стартовое по машинам
sprintf(strsql,"select ka,kv,sal,kmt from Uplsal where god=%d",godn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
double kolih;
while(cur.read_cursor(&row) != 0)
 {
//  if(atoi(row[0]) == 1)
//   printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  kolih=atof(row[2]);
  
  //вообщето он однозначно должен быть в списке
  if(m_kod_avt.find(row[0]) < 0)
    m_kod_avt.plus(row[0]);    
  
  m_suma_avt[m_kod_avt.find(row[0])][0]+=kolih;

  //вообщето он однозначно должен быть в списке
  if(m_kod_vod.find(row[1]) < 0)
    m_kod_vod.plus(row[1]);    
  
  m_suma_vod[m_kod_vod.find(row[1])][0]+=kolih;
  
  m_suma_mt[m_kod_mt.find_r(row[3])][0]+=kolih;
 }

sprintf(strsql,"select datd,ka,kv,kodsp,tz,kolih,kap,kodtp from Upldok1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd asc",
godn,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short d,m,g;
int tipz;
int kap;
int nomer_avt;
int nomer_vod;
float kolstr1=0;
int nomer_mt=0;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  tipz=atoi(row[4]);
  
  kolih=atof(row[5]);
  
  kap=atoi(row[6]);
  
  iceb_u_rsdat(&d,&m,&g,row[0],2);  

  nomer_avt=m_kod_avt.find(row[1]);
  nomer_vod=m_kod_vod.find(row[2]);
  nomer_mt=m_kod_mt.find_r(row[7]);

  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0)//До периода
   {
    if(tipz == 1)
     {
      m_suma_vod[nomer_vod][0]+=kolih;
      m_suma_avt[nomer_avt][0]+=kolih;
      if(kap == 0)
       m_suma_mt[nomer_mt][0]+=kolih;
     }
    if(tipz == 2)
     {
      m_suma_vod[nomer_vod][0]-=kolih;
      m_suma_avt[nomer_avt][0]-=kolih;
      m_suma_mt[nomer_mt][0]-=kolih;
     }

    if(kap != 0) //Передача топлива
     {
      nomer_avt=m_kod_avt.find(kap);
      nomer_vod=m_kod_vod.find(row[3]);

      m_suma_vod[nomer_vod][0]-=kolih;
      m_suma_avt[nomer_avt][0]-=kolih;
      
     }       
    continue;
   }   

  //Период
  if(tipz == 1) //Приход
   {
    if(kap == 0) //Чистый приход
     {
      m_suma_vod[nomer_vod][2]+=kolih;
      m_suma_avt[nomer_avt][2]+=kolih;
      m_suma_mt[nomer_mt][1]+=kolih;
     }  
    else //Передача топлива
     {
      m_suma_vod[nomer_vod][1]+=kolih;
      m_suma_avt[nomer_avt][1]+=kolih;

      //Записываем от какого автомобиля и какого водителя ушло
       
      nomer_avt=m_kod_avt.find(kap);
      nomer_vod=m_kod_vod.find(row[3]);

      m_suma_vod[nomer_vod][4]+=kolih;
      m_suma_avt[nomer_avt][4]+=kolih;
  
     }
     
   }

  if(tipz == 2) //Расход
   {
    m_suma_vod[nomer_vod][3]+=kolih;
    m_suma_avt[nomer_avt][3]+=kolih;
    m_suma_mt[nomer_mt][2]+=kolih;
   }
    
 }



//Распечатываем по автомобилям

char imaf_avt[50];
sprintf(imaf_avt,"ovdt_a%d.lst",getpid());

upl_obv_rs(0,dn,mn,gn,dk,mk,gk,&m_kod_avt,m_suma_avt,data->rk->kod_vod.ravno(),data->rk->kod_avt.ravno(),imaf_avt,data->window);

char imaf_vod[50];
sprintf(imaf_vod,"ovdt_v%d.lst",getpid());

upl_obv_rs(1,dn,mn,gn,dk,mk,gk,&m_kod_vod,m_suma_vod,data->rk->kod_vod.ravno(),data->rk->kod_avt.ravno(),imaf_vod,data->window);


char imaf_mt[50];
sprintf(imaf_mt,"ovdt_m%d.lst",getpid());
upl_obv_rs_mt(dn,mn,gn,dk,mk,gk,&m_kod_mt,m_suma_mt,imaf_mt,data->window);

data->rk->imaf.plus(imaf_vod);
data->rk->naim.plus(gettext("Оборотная ведомость движения топлива по водителям"));
data->rk->imaf.plus(imaf_avt);
data->rk->naim.plus(gettext("Оборотная ведомость движения топлива по автомобилям"));
data->rk->imaf.plus(imaf_mt);
data->rk->naim.plus(gettext("Оборотная ведомость движения топлива по маркам топлива"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),0,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
