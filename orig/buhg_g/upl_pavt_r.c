/*$Id: upl_pavt_r.c,v 1.11 2011-02-21 07:35:58 sasa Exp $*/
/*17.11.2008	31.03.2008	Белых А.И.	upl_pavt_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "upl_pavt.h"

class upl_pavt_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class upl_pavt_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  upl_pavt_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_pavt_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_pavt_r_data *data);
gint upl_pavt_r1(class upl_pavt_r_data *data);
void  upl_pavt_r_v_knopka(GtkWidget *widget,class upl_pavt_r_data *data);

double upl_ptw(short dn,short mn,short gn,short dk,short mk,short gk,char *kod_avt,char *kod_vod,GtkWidget *wpredok);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int upl_pavt_r(class upl_pavt_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class upl_pavt_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка оборотной ведомости движения топлива по автомобилям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(upl_pavt_r_key_press),&data);

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

repl.plus(gettext("Распечатка оборотной ведомости движения топлива по автомобилям"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(upl_pavt_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)upl_pavt_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_pavt_r_v_knopka(GtkWidget *widget,class upl_pavt_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   upl_pavt_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_pavt_r_data *data)
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

/**************************/
/*Шапка                   */
/**************************/

void hpov(short dn,short mn,short gn,
short dk,short mk,short gk,
FILE *ff,FILE *ff_f)
{
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
           Автомобиль          |      Водитель      | Остаток  |           Движение топлива за период      | Остаток  |\n\
      Марка    |Государс. номер|                    |%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_f,gettext("\
           Автомобиль          |      Водитель      | Остаток  |           Движение топлива за период      | Остаток  |       Фактические затраты                            |\n\
      Марка    |Государс. номер|                    |%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|По городу |За городом|На груз   |На мот.час|  Итого   |\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

}
/******************/
/*Надпись         */
/******************/

void uplovrp_n(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodvod,
const char *kodavt,
const char *podr,
const char *nadp,
FILE *ff)
{
iceb_u_zagolov(nadp,dn,mn,gn,dk,mk,gk,organ,ff);

if(kodvod[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Водитель"),kodvod);
if(kodavt[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Автомобиль"),kodavt);
if(podr[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Подразделение"),podr);
}

/******************************************/
/******************************************/

gint upl_pavt_r1(class upl_pavt_r_data *data)
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





//Определяем список автомобилей

sprintf(strsql,"select distinct kv,ka from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' order by ka,kv asc",gn,mn,dn,gk,mk,dk);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  
class iceb_u_spisok KVKA;  //код водителя|код автомобиля
class iceb_u_int kod_avt;  //коды автомобилей
#define OST_AVTOMOBIL 9
class iceb_u_double ost_avt[OST_AVTOMOBIL]; //остатки по автомобилям

class iceb_u_int kod_vod;  //коды водителей
#define OST_VODITEL 9
class iceb_u_double ost_vod[OST_VODITEL]; //остатки по водителям


float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kod_vod.ravno(),row[0],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->kod_avt.ravno(),row[1],0,0) != 0)
   continue;

  sprintf(strsql,"%s|%s",row[0],row[1]);
  KVKA.plus(strsql);
  
  if(kod_avt.find(atoi(row[1])) < 0)
   kod_avt.plus(atoi(row[1]),-1);

  if(kod_vod.find(atoi(row[0])) < 0)
   kod_vod.plus(atoi(row[0]),-1);
 }

int kkk=kod_avt.kolih();

for(int i=0 ; i < OST_AVTOMOBIL; i++)
 ost_avt[i].make_class(kkk);

kkk=kod_vod.kolih();

for(int i=0 ; i < OST_VODITEL; i++)
 ost_vod[i].make_class(kkk);
char imaf[56];
sprintf(imaf,"rpl%d.lst",getpid());
FILE *ff;


if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

uplovrp_n(dn,mn,gn,dk,mk,gk,data->rk->kod_vod.ravno(),data->rk->kod_avt.ravno(),data->rk->kod_pod.ravno(),gettext("Оботротная ведомость движения топлива по автомобилям"),ff);
char imaf_f[50];
sprintf(imaf_f,"rplf%d.lst",getpid());
FILE *ff_f;
if((ff_f = fopen(imaf_f,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_f,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

uplovrp_n(dn,mn,gn,dk,mk,gk,data->rk->kod_vod.ravno(),data->rk->kod_avt.ravno(),data->rk->kod_pod.ravno(),gettext("Оботротная ведомость движения топлива по автомобилям"),ff_f);

hpov(dn,mn,gn,dk,mk,gk,ff,ff_f);

int kol=KVKA.kolih();
//printw("kol=%d\n",kol);
char kodvd[20];
char kodav[20];
char naimvod[512];
char naimavt[512];
char rnavt[40];
double kolih=0.;

double rt_za_gor=0.;
double rt_po_gor=0.;
double rt_na_gruz=0.;
double rt_na_vrem=0.;

double irt_za_gor=0.;
double irt_po_gor=0.;
double irt_na_gruz=0.;
double irt_na_vrem=0.;

double dvig[4];
memset(&dvig,'\0',sizeof(dvig));
double idvig[4];
memset(&idvig,'\0',sizeof(idvig));
int nomer_avt=0;
char nomdok[32];
char datd[20];
char kp[10];
double peredano=0.;
double iperedano=0.;
char otmet[100];
class SQLCURSOR curr;
SQL_str row1;
for(int nomer=0; nomer < kol; nomer++)
 {

  iceb_u_polen(KVKA.ravno(nomer),kodvd,sizeof(kodav),1,'|');
  iceb_u_polen(KVKA.ravno(nomer),kodav,sizeof(kodvd),2,'|');

  iceb_pbar(data->bar,kol,nomer);    
  
  //Читаем водителя
  memset(naimvod,'\0',sizeof(naimvod));
  strcpy(naimvod,kodvd);
  sprintf(strsql,"select naik from Uplouot where kod=%s",kodvd);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)  
   {
    strcat(naimvod," ");
    strcat(naimvod,row[0]);
   }
  //Читаем автомобиль
  memset(rnavt,'\0',sizeof(rnavt));
  memset(naimavt,'\0',sizeof(naimavt));
  strcpy(naimavt,kodav);
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%s",kodav);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)  
   {
    strncpy(rnavt,row[0],sizeof(rnavt)-1);
    strcat(naimavt," ");
    strcat(naimavt,row[1]);
   }

  sprintf(strsql,"select kp,tz,kolih,datd,nomd,nz,kap from Upldok1 where datd >= '%d-%d-%d' \
and datd <= '%d-%d-%d' and kv=%s and ka=%s order by datd,nomd asc",
  gn,mn,dn,gk,mk,dk,kodvd,kodav);

  if((kolstr1=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  memset(&dvig,'\0',sizeof(dvig));



  memset(nomdok,'\0',sizeof(nomdok));
  rt_za_gor=rt_po_gor=rt_na_gruz=rt_na_vrem=0.;
  int sh_str=0;
  while(cur.read_cursor(&row) != 0)
   {
    if(sh_str == 0)
     {
      dvig[0]=ostvaw(dn,mn,gn,kodvd,kodav,atoi(row[5]),NULL,NULL,data->window);
      sh_str=1;
     }
    if(iceb_u_proverka(data->rk->kod_pod.ravno(),row[0],0,0) != 0)
     continue;
    kolih=atof(row[2]);               
    if(row[1][0] == '1')
     {
      if(atoi(row[6]) != 0)
        dvig[1]+=kolih;      
      else
        dvig[2]+=kolih;      
     }
    else
     {
      dvig[3]+=kolih;      
     }
    if(iceb_u_SRAV(nomdok,row[4],0) != 0)
     {
      memset(otmet,'\0',sizeof(otmet));
      if(nomdok[0] != '\0')
       {
        //читаем фактические затраты топлива в шапке документа
        sprintf(strsql,"select ztfa,ztfazg,ztvsf,ztvrf,otmet from Upldok where datd='%s' and kp=%s and nomd='%s'",
        datd,kp,nomdok);
        
        if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
         {
          rt_po_gor+=atof(row1[0]);
          rt_za_gor+=atof(row1[1]);
          rt_na_gruz+=atof(row1[2]);
          rt_na_vrem+=atof(row1[3]);
          strncpy(otmet,row1[4],sizeof(otmet)-1);
         }
        else
         {
          class iceb_u_str repl;
          
          repl.plus(gettext("Не найдена шапка документа !"));

          sprintf(strsql,"%s %s",datd,nomdok);
          repl.ps_plus(strsql);

          iceb_menu_soob(&repl,data->window);
          
         }
       }
      strncpy(nomdok,row[4],sizeof(nomdok)-1); 
      strncpy(datd,row[3],sizeof(datd)-1); 
      strncpy(kp,row[0],sizeof(kp)-1); 
     }    
   }
  if(kolstr1 != 0)
   {
    //читаем фактические затраты топлива в шапке документа
    sprintf(strsql,"select ztfa,ztfazg,ztvsf,ztvrf from Upldok where datd='%s' and kp=%s and nomd='%s'",
    datd,kp,nomdok);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
     {
      rt_po_gor+=atof(row1[0]);
      rt_za_gor+=atof(row1[1]);
      rt_na_gruz+=atof(row1[2]);
      rt_na_vrem+=atof(row1[3]);
     }
    else
     {
      class iceb_u_str repl;
      
      repl.plus(gettext("Не найдена шапка документа !"));

      sprintf(strsql,"%s %s",datd,nomdok);
      repl.ps_plus(strsql);

      iceb_menu_soob(&repl,data->window);
      
      
     }
   }

  peredano=upl_ptw(dn,mn,gn,dk,mk,gk,kodav,kodvd,data->window);
  iperedano+=peredano;
  
  kolih=dvig[0]+dvig[1]+dvig[2]-dvig[3]-peredano;

  nomer_avt=kod_avt.find(atoi(kodav));
  
  ost_avt[0].plus(dvig[0],nomer_avt);
  ost_avt[1].plus(dvig[1],nomer_avt);
  ost_avt[2].plus(dvig[2],nomer_avt);
  ost_avt[3].plus(dvig[3],nomer_avt);
  ost_avt[4].plus(peredano,nomer_avt);
  ost_avt[5].plus(rt_po_gor,nomer_avt);
  ost_avt[6].plus(rt_za_gor,nomer_avt);
  ost_avt[7].plus(rt_na_gruz,nomer_avt);
  ost_avt[8].plus(rt_na_vrem,nomer_avt);

  nomer_avt=kod_vod.find(atoi(kodvd));

  ost_vod[0].plus(dvig[0],nomer_avt);
  ost_vod[1].plus(dvig[1],nomer_avt);
  ost_vod[2].plus(dvig[2],nomer_avt);
  ost_vod[3].plus(dvig[3],nomer_avt);
  ost_vod[4].plus(peredano,nomer_avt);
  ost_vod[5].plus(rt_po_gor,nomer_avt);
  ost_vod[6].plus(rt_za_gor,nomer_avt);
  ost_vod[7].plus(rt_na_gruz,nomer_avt);
  ost_vod[8].plus(rt_na_vrem,nomer_avt);
  
  fprintf(ff,"%-*.*s %-*.*s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %s\n",
  iceb_u_kolbait(15,naimavt),iceb_u_kolbait(15,naimavt),naimavt,
  iceb_u_kolbait(15,rnavt),iceb_u_kolbait(15,rnavt),rnavt,
  iceb_u_kolbait(20,naimvod),iceb_u_kolbait(20,naimvod),naimvod,
  dvig[0],dvig[1],dvig[2],dvig[3],peredano,kolih,otmet);

  fprintf(ff_f,"%-*.*s %-*.*s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %s\n",
  iceb_u_kolbait(15,naimavt),iceb_u_kolbait(15,naimavt),naimavt,
  iceb_u_kolbait(15,rnavt),iceb_u_kolbait(15,rnavt),rnavt,
  iceb_u_kolbait(20,naimvod),iceb_u_kolbait(20,naimvod),naimvod,
  dvig[0],dvig[1],dvig[2],dvig[3],peredano,kolih,
  rt_po_gor,rt_za_gor,rt_na_gruz,rt_na_vrem,
  rt_po_gor+rt_za_gor+rt_na_gruz+rt_na_vrem,otmet);
  
  idvig[0]+=dvig[0];
  idvig[1]+=dvig[1];
  idvig[2]+=dvig[2];
  idvig[3]+=dvig[3];

  irt_za_gor+=rt_za_gor;
  irt_po_gor+=rt_po_gor;
  irt_na_gruz+=rt_na_gruz;
  irt_na_vrem+=rt_na_vrem;

 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

kolih=idvig[0]+idvig[1]+idvig[2]-idvig[3]-iperedano;

fprintf(ff,"\
%*s:%10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(52,gettext("Итого")),gettext("Итого"),
idvig[0],idvig[1],idvig[2],idvig[3],iperedano,kolih);

fprintf(ff_f,"\
%*s:%10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(52,gettext("Итого")),gettext("Итого"),idvig[0],idvig[1],idvig[2],idvig[3],iperedano,kolih,
irt_po_gor,irt_za_gor,irt_na_gruz,irt_na_vrem,
irt_po_gor+irt_za_gor+irt_na_gruz+irt_na_vrem);

iceb_podpis(ff,data->window);
fclose(ff);

iceb_podpis(ff_f,data->window);
fclose(ff_f);


//Распечатка только по автомобилям
char imaf_avt[30];
char imaf_avt_f[30];

sprintf(imaf_avt,"ostavt%d.lst",getpid());

if((ff = fopen(imaf_avt,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_avt,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

uplovrp_n(dn,mn,gn,dk,mk,gk,data->rk->kod_vod.ravno(),data->rk->kod_avt.ravno(),data->rk->kod_pod.ravno(),gettext("Оботротная ведомость движения топлива по автомобилям"),ff);

sprintf(imaf_avt_f,"ostavtf%d.lst",getpid());

if((ff_f = fopen(imaf_avt_f,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_avt_f,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

uplovrp_n(dn,mn,gn,dk,mk,gk,data->rk->kod_vod.ravno(),data->rk->kod_avt.ravno(),data->rk->kod_pod.ravno(),gettext("Оботротная ведомость движения топлива по автомобилям"),ff_f);


fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код |           Автомобиль          | Остаток  |    Движение топлива за период             | Остаток  |\n\
авт.|      Марка    |Государс. номер|%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
dn,mn,gn,dk,mk,gk);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");

fprintf(ff_f,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_f,gettext("\
Код |           Автомобиль          | Остаток  |      Движение топлива за период           | Остаток  |             Фактические затраты                      |\n\
авт.|      Марка    |Государс. номер|%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|По городу |За городом|На груз   |На мот.час|  Итого   |\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff_f,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");



kol=kod_avt.kolih();
int kavt;
irt_za_gor=0.;
irt_po_gor=0.;
irt_na_gruz=0.;
irt_na_vrem=0.;

for(int nomer=0; nomer < kol; nomer++)
 {
  kavt=kod_avt.ravno(nomer);
  //Читаем автомобиль
  memset(rnavt,'\0',sizeof(rnavt));
  memset(naimavt,'\0',sizeof(naimavt));
  
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%d",kavt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)  
   {
    strncpy(rnavt,row[0],sizeof(rnavt)-1);
    strncpy(naimavt,row[1],sizeof(naimavt)-1);
   }

  kolih=ost_avt[0].ravno(nomer)+ost_avt[1].ravno(nomer)+ost_avt[2].ravno(nomer)-\
  ost_avt[3].ravno(nomer)-ost_avt[4].ravno(nomer);

  fprintf(ff,"%4d %-*.*s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  kavt,
  iceb_u_kolbait(15,naimavt),iceb_u_kolbait(15,naimavt),naimavt,
  iceb_u_kolbait(15,rnavt),iceb_u_kolbait(15,rnavt),rnavt,
  ost_avt[0].ravno(nomer),
  ost_avt[1].ravno(nomer),
  ost_avt[2].ravno(nomer),
  ost_avt[3].ravno(nomer),
  ost_avt[4].ravno(nomer),
  kolih);

  fprintf(ff_f,"%4d %-*.*s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  kavt,
  iceb_u_kolbait(15,naimavt),iceb_u_kolbait(15,naimavt),naimavt,
  iceb_u_kolbait(15,rnavt),iceb_u_kolbait(15,rnavt),rnavt,
  ost_avt[0].ravno(nomer),
  ost_avt[1].ravno(nomer),
  ost_avt[2].ravno(nomer),
  ost_avt[3].ravno(nomer),
  ost_avt[4].ravno(nomer),
  kolih,
  ost_avt[5].ravno(nomer),
  ost_avt[6].ravno(nomer),
  ost_avt[7].ravno(nomer),
  ost_avt[8].ravno(nomer),
  ost_avt[5].ravno(nomer)+ ost_avt[6].ravno(nomer)+ ost_avt[7].ravno(nomer)+ost_avt[8].ravno(nomer));

 }


iceb_podpis(ff,data->window);
fclose(ff);

iceb_podpis(ff_f,data->window);
fclose(ff_f);

//Распечатка только по водителям
char imaf_vod[30];
char imaf_vod_f[30];

sprintf(imaf_vod,"ostvod%d.lst",getpid());

if((ff = fopen(imaf_vod,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_vod,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

uplovrp_n(dn,mn,gn,dk,mk,gk,data->rk->kod_vod.ravno(),data->rk->kod_avt.ravno(),data->rk->kod_pod.ravno(),gettext("Оборотная ведомость движения топлива по водителям"),ff);

sprintf(imaf_vod_f,"ostvodf%d.lst",getpid());

if((ff_f = fopen(imaf_vod_f,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_vod_f,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

uplovrp_n(dn,mn,gn,dk,mk,gk,data->rk->kod_vod.ravno(),data->rk->kod_avt.ravno(),data->rk->kod_pod.ravno(),gettext("Оборотная ведомость движения топлива по водителям"),ff_f);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код |       Фамилия водителя        | Остаток  |           Движение топлива за период      | Остаток  |\n\
вод.|                               |%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");

fprintf(ff_f,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_f,gettext("\
Код |       Фамилия водителя       | Остаток  |           Движение топлива за период      | Остаток  |             Фактические затраты                      |\n\
вод.|                              |%02d.%02d.%04d| Принято  | Получено |Потрачено | Передано |%02d.%02d.%04d|По городу |За городом|На груз   |На мот.час|  Итого   |\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff_f,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");


kol=kod_vod.kolih();

for(int nomer=0; nomer < kol; nomer++)
 {
  kavt=kod_vod.ravno(nomer);

  //Читаем водителя
  memset(naimvod,'\0',sizeof(naimvod));
  sprintf(strsql,"select naik from Uplouot where kod=%d",kavt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)  
   {
    strncpy(naimvod,row[0],sizeof(naimvod)-1);
   }
  kolih=ost_vod[0].ravno(nomer)+ost_vod[1].ravno(nomer)+ost_vod[2].ravno(nomer)-ost_vod[3].ravno(nomer)-ost_vod[4].ravno(nomer);

  fprintf(ff,"%4d %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  kavt,
  iceb_u_kolbait(31,naimvod),iceb_u_kolbait(31,naimvod),naimvod,
  ost_vod[0].ravno(nomer),
  ost_vod[1].ravno(nomer),
  ost_vod[2].ravno(nomer),
  ost_vod[3].ravno(nomer),
  ost_vod[4].ravno(nomer),
  kolih);

  fprintf(ff_f,"%4d %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  kavt,
  iceb_u_kolbait(30,naimvod),iceb_u_kolbait(30,naimvod),naimvod,
  ost_vod[0].ravno(nomer),
  ost_vod[1].ravno(nomer),
  ost_vod[2].ravno(nomer),
  ost_vod[3].ravno(nomer),
  ost_vod[4].ravno(nomer),
  kolih,
  ost_vod[5].ravno(nomer),
  ost_vod[6].ravno(nomer),
  ost_vod[7].ravno(nomer),
  ost_vod[8].ravno(nomer),
  ost_vod[5].ravno(nomer)+ ost_vod[6].ravno(nomer)+ ost_vod[7].ravno(nomer)+ost_vod[8].ravno(nomer));

 }


iceb_podpis(ff,data->window);
fclose(ff);

iceb_podpis(ff_f,data->window);
fclose(ff_f);

data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Оборотная ведомость движения топлива по водителях и автомобилях"));

data->rk->imaf.plus(imaf_avt);
data->rk->naim.plus(gettext("Оборотная ведомость движения топлива по автомобилях"));


data->rk->imaf.plus(imaf_vod);
data->rk->naim.plus(gettext("Оборотная ведомость движения топлива по водителям"));


data->rk->imaf.plus(imaf_f);

sprintf(strsql,"%s+%s",
gettext("Оборотная ведомость движения топлива по водителям и автомобилям"),
gettext("фактические затраты"));
data->rk->naim.plus(strsql);

data->rk->imaf.plus(imaf_avt_f);

sprintf(strsql,"%s+%s",
gettext("Оборотная ведомость движения топлива по автомобилям"),
gettext("фактические затраты"));

data->rk->naim.plus(strsql);

data->rk->imaf.plus(imaf_vod_f);

sprintf(strsql,"%s+%s",
gettext("Оборотная ведомость движения топлива по водителям"),
gettext("фактические затраты"));

data->rk->naim.plus(strsql);

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
