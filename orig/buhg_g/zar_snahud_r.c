/*$Id: zar_snahud_r.c,v 1.10 2011-02-21 07:35:59 sasa Exp $*/
/*20.11.2009	30.01.2007	Белых А.И.	zar_snahud1_r.c
Расчёт свода начислений и удержаиний
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_snahud.h"

class zar_snahud1_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_snahud_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_snahud1_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_snahud1_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_snahud1_r_data *data);
gint zar_snahud1_r1(class zar_snahud1_r_data *data);
void  zar_snahud1_r_v_knopka(GtkWidget *widget,class zar_snahud1_r_data *data);

void snu_ras(int metka,class iceb_u_int *kod,double *suma,FILE *ff,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;


int zar_snahud_r(class zar_snahud_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_snahud1_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать свод начислений и удержаний"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_snahud1_r_key_press),&data);

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

repl.plus(gettext("Распечатать свод начислений и удержаний"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_snahud1_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_snahud1_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_snahud1_r_v_knopka(GtkWidget *widget,class zar_snahud1_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_snahud1_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_snahud1_r_data *data)
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

gint zar_snahud1_r1(class zar_snahud1_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mn,gn;
short mk,gk;

iceb_u_rsdat1(&mn,&gn,data->rk->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->rk->datan.ravno());
if(mk == 0)
 {
  mk=mn;
  gk=gn;
 }
sprintf(strsql,"%s %d.%d %s %d.%d\n",gettext("Период с"),mn,gn,
gettext("по"),mk,gk);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);









int kolstr;
SQL_str row;
SQLCURSOR cur;
class iceb_u_int nahis;
class iceb_u_int uder;

//формируем масив начислений
sprintf(strsql,"select kod from Nash");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не ввели начисления !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 
while(cur.read_cursor(&row) != 0)
  nahis.plus(atoi(row[0]),-1);

double suma_nahis[nahis.kolih()];
memset(&suma_nahis,'\0',sizeof(suma_nahis));
//формируем масив удержаний
sprintf(strsql,"select kod from Uder");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не введены удержания !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
  uder.plus(atoi(row[0]),-1);

double suma_uder[uder.kolih()];
memset(&suma_uder,'\0',sizeof(suma_uder));


sprintf(strsql,"select tabn,prn,knah,suma,shet,podr from Zarp where \
datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%02d' and suma <> 0. \
order by tabn asc",gn,mn,1,gk,mk,31);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

float kolstr1=0;
int prn=0;
double suma=0.;
int knah=0;
int nomer=0;
short dz,mz,gz;
SQL_str row1;
class SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {

//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->podr.ravno(),row[5],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->shet.ravno(),row[4],0,0) != 0)
    continue;

  if(data->rk->kateg.getdlinna() > 1)
   {
    iceb_u_rsdat(&dz,&mz,&gz,row[6],2);    
    sprintf(strsql,"select kateg from Zarn where tabn=%s and god=%d and mes=%d",row[0],gz,mz);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      if(iceb_u_proverka(data->rk->kateg.ravno(),row1[0],0,0) != 0)
       continue;
     }
    else
     {
      sprintf(strsql,"Не найдена категория для %s !",row[0]);
      iceb_menu_soob(strsql,data->window);
     }
   }

  prn=atoi(row[1]);
  knah=atoi(row[2]);
  suma=atof(row[3]);  
  if(prn == 1)//начисления
   {
    if((nomer=nahis.find(knah)) < 0)
     continue;
    suma_nahis[nomer]+=suma;
   }

  if(prn == 2) //удержания
   {
    if((nomer=uder.find(knah)) < 0)
     continue;
    suma_uder[nomer]+=suma;
   }
  
 }

char imaf[40];
FILE *ff;
sprintf(imaf,"snu%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Свод начислений и удержаний"),1,mn,gn,31,mk,gk,organ,ff);

if(data->rk->shet.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());

if(data->rk->podr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),data->rk->podr.ravno());

if(data->rk->tabnom.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),data->rk->tabnom.ravno());

fprintf(ff,"\n");

snu_ras(1,&nahis,suma_nahis,ff,data->window);

fprintf(ff,"\n");

snu_ras(2,&uder,suma_uder,ff,data->window);

iceb_podpis(ff,data->window);

fclose(ff);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Свод начислений и удержаний"));
iceb_ustpeh(imaf,3,data->window);




data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}


/*******************************/
/*Распечатка*/
/*******************************/

void snu_ras(int metka,//1-Начисления 2-Удержания
class iceb_u_int *kod,
double *suma,
FILE *ff,
GtkWidget *wpredok)
{
char strsql[512];
char naim[512];
SQL_str row;
SQLCURSOR cur;

if(metka == 1)
 fprintf(ff,"%s\n",gettext("Начисления")); 
if(metka == 2)
 fprintf(ff,"%s\n",gettext("Удержания")); 

fprintf(ff,"\
----------------------------------------------\n");

fprintf(ff,gettext("\
Код|     Наименование             |  Сумма   |\n"));

fprintf(ff,"\
----------------------------------------------\n");
double itogo=0.;
for(int i=0; i < kod->kolih() ; i++)
 {
  if(suma[i] == 0.)
   continue;
  if(metka == 1)
   sprintf(strsql,"select naik from Nash where kod=%d",kod->ravno(i));
  if(metka == 2)
   sprintf(strsql,"select naik from Uder where kod=%d",kod->ravno(i));
  memset(naim,'\0',sizeof(naim));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    strncpy(naim,row[0],sizeof(naim)-1);
    
  fprintf(ff,"%3d %-*.*s %10.2f\n",kod->ravno(i),iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,suma[i]);
  itogo+=suma[i];
 }
fprintf(ff,"\
----------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(34,gettext("Итого")),gettext("Итого"),itogo);

}
