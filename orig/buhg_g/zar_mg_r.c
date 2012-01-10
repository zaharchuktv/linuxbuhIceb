/*$Id: zar_mg_r.c,v 1.13 2011-02-21 07:35:59 sasa Exp $*/
/*20.11.2009	06.12.2006	Белых А.И.	zar_mg_r.c
Расчёт распределения зарплаты мужчины/женщины
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class zar_mg_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  const char *data_r;
  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;
    
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_mg_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_mg_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_mg_r_data *data);
gint zar_mg_r1(class zar_mg_r_data *data);
void  zar_mg_r_v_knopka(GtkWidget *widget,class zar_mg_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_mg_r(const char *data_r,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_mg_r_data data;

data.data_r=data_r;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать распределение зарплаты мужчины/женщины"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_mg_r_key_press),&data);

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

repl.plus(gettext("Распечатать распределение зарплаты мужчины/женщины"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_mg_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_mg_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_mg_r_v_knopka(GtkWidget *widget,class zar_mg_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_mg_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_mg_r_data *data)
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

gint zar_mg_r1(class zar_mg_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mn,gn;
iceb_u_rsdat1(&mn,&gn,data->data_r);



//Создаем массивы для видов табеля

class iceb_u_int TABEL;

int kolstr=0;
class SQLCURSOR cur;
sprintf(strsql,"select kod from Tabel");
int koltabel=0;
if((koltabel=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(FALSE);
 }
if(koltabel == 0)
 {
  iceb_menu_soob(gettext("Не введены виды табеля !"),data->window);
  return(FALSE);
 }
SQL_str row,row1,row2;
while(cur.read_cursor(&row) != 0)
  TABEL.plus(atoi(row[0]),-1);

class iceb_u_int DNIMUG;
class iceb_u_int HASMUG;
class iceb_u_int KDNMUG;
class iceb_u_int DNIMUG_S;
class iceb_u_int HASMUG_S;
class iceb_u_int KDNMUG_S;

class iceb_u_int DNIGEN;
class iceb_u_int HASGEN;
class iceb_u_int KDNGEN;
class iceb_u_int DNIGEN_S;
class iceb_u_int HASGEN_S;
class iceb_u_int KDNGEN_S;

DNIMUG.make_class(koltabel);
HASMUG.make_class(koltabel);
KDNMUG.make_class(koltabel);
DNIMUG_S.make_class(koltabel);
HASMUG_S.make_class(koltabel);
KDNMUG_S.make_class(koltabel);

DNIGEN.make_class(koltabel);
HASGEN.make_class(koltabel);
KDNGEN.make_class(koltabel);

DNIGEN_S.make_class(koltabel);
HASGEN_S.make_class(koltabel);
KDNGEN_S.make_class(koltabel);


//Создаем массивы для видов начислений

class iceb_u_int NASH;
int kolnah=0;
sprintf(strsql,"select kod from Nash");
if((kolnah=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(FALSE);
 }
if(kolnah == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни одного начисления !"),data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
  NASH.plus(atoi(row[0]),-1);

class iceb_u_double ZNAHMUG;
class iceb_u_double ZNAHMUG_S;
class iceb_u_double ZNAHGEN;
class iceb_u_double ZNAHGEN_S;
ZNAHMUG.make_class(kolnah);
ZNAHMUG_S.make_class(kolnah);
ZNAHGEN.make_class(kolnah);
ZNAHGEN_S.make_class(kolnah);

double		tablic[4][3]; 
memset(&tablic,'\0',sizeof(tablic));
sprintf(strsql,"select tabn,datk,sovm from Zarn where god=%d and mes=%d",gn,mn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  return(FALSE);
 }

char imaf_prot[50];
sprintf(imaf_prot,"pol%d.lst",getpid());
FILE *ff_prot;
if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prot,"",errno,data->window);
  return(FALSE);
 }
short du,mu,gu;
short metka_sovm=0;

float kolstr1=0;
int kolmug=0;
int kolmug_s=0;
int kolgen=0;
int kolgen_s=0;
class SQLCURSOR curr;
class SQLCURSOR cur1;  
double sum=0;
int pol=0;
int kolstr2=0;
int kom1=0;
int i;
int kodt=0;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s\n",row[0]);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  iceb_u_rsdat(&du,&mu,&gu,row[1],2);

  if(du != 0)
   if(iceb_u_sravmydat(du,mu,gu,1,mn,gn) < 0)
    continue;
  metka_sovm=atoi(row[2]);
  
  sprintf(strsql,"select pl,fio from Kartb where tabn=%s",row[0]);
  pol=0;
  
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    pol=atoi(row1[0]);
  else
   {
    
    sprintf(strsql,"%s %s !",gettext("Не найден табельный номер"),row[0]);
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  if(pol == 0)
   {
    kolmug++;  
    if(metka_sovm == 1)
     kolmug_s++;
   }
  if(pol == 1)
   {
    kolgen++;  
    if(metka_sovm == 1)
     kolgen_s++;
   }
  if(pol == 0)
   fprintf(ff_prot,"%-5s %-*.*s %s\n",row[0],
   iceb_u_kolbait(30,row1[1]),iceb_u_kolbait(30,row1[1]),row1[1],
   gettext("Мужчина"));
  if(pol == 1)
   fprintf(ff_prot,"%-5s %-*.*s %s\n",row[0],iceb_u_kolbait(30,row1[1]),iceb_u_kolbait(30,row1[1]),row1[1],gettext("Женщина"));
   
  //Читаем отработанное время
  sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and \
mes=%d and tabn=%s",gn,mn,row[0]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  while(cur1.read_cursor(&row1) != 0)
   {
    kodt=atoi(row1[0]);
    i=TABEL.find(kodt);
    if(pol == 0)
     {
      DNIMUG.plus(atoi(row1[1]),i);
      HASMUG.plus(atoi(row1[2]),i);
      KDNMUG.plus(atoi(row1[3]),i);
      if(metka_sovm == 1)
       {
        DNIMUG_S.plus(atoi(row1[1]),i);
        HASMUG_S.plus(atoi(row1[2]),i);
        KDNMUG_S.plus(atoi(row1[3]),i);
       }
     }
    if(pol == 1)
     {
      DNIGEN.plus(atoi(row1[1]),i);
      HASGEN.plus(atoi(row1[2]),i);
      KDNGEN.plus(atoi(row1[3]),i);
      if(metka_sovm == 1)
       {
        DNIGEN_S.plus(atoi(row1[1]),i);
        HASGEN_S.plus(atoi(row1[2]),i);
        KDNGEN_S.plus(atoi(row1[3]),i);
       }
     }
   }


  sprintf(strsql,"select knah,suma from Zarp where datz >= '%d-%d-1' \
and datz <= '%d-%d-31' and tabn=%s and prn='1' and suma <> 0.",
  gn,mn,gn,mn,row[0]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  while(cur1.read_cursor(&row1) != 0)
   {
    sum=atof(row1[1]);
    sprintf(strsql,"select vidn from Nash where kod=%s",row1[0]);
    if(sql_readkey(&bd,strsql,&row2,&curr) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код начисления"),row1[0]);
      iceb_menu_soob(strsql,data->window);
     }
    kom1=atoi(row2[0]);
    tablic[pol][kom1]+=sum;
    if(metka_sovm == 1)
     tablic[pol+2][kom1]+=sum;
    
    i=NASH.find(atoi(row1[0]));
    if(pol == 0)
     { 
      ZNAHMUG.plus(sum,i);
      if(metka_sovm == 1)
       ZNAHMUG_S.plus(sum,i);
     }
    if(pol == 1)
     { 
      ZNAHGEN.plus(sum,i);
      if(metka_sovm == 1)
        ZNAHGEN_S.plus(sum,i);
      
     }

   }    
 }
fclose(ff_prot);
char imaf[56];
sprintf(imaf,"m-g%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return(FALSE);
 }
short dkm=1;
iceb_u_dpm(&dkm,&mn,&gn,5);

iceb_u_zagolov(gettext("Расчёт распределения зарплат мужчины/женщины"),0,0,0,0,mn,gn,"",ff);

fprintf(ff,"\n%s:%s\n",gettext("Пол"),gettext("Мужчина"));
fprintf(ff,"----------------------------------------------------------\n");

fprintf(ff,"\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8d\n\n",
iceb_u_kolbait(10,gettext("Основная")),gettext("Основная"),
tablic[0][0],
iceb_u_kolbait(10,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[0][1],
iceb_u_kolbait(10,gettext("Прочая")),gettext("Прочая"),
tablic[0][2],
iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),
tablic[0][0]+tablic[0][1]+tablic[0][2],
iceb_u_kolbait(10,gettext("Количество")),gettext("Количество"),kolmug);

int s1=0,s2=0,s3=0;
char naimtab[100];
for(int i=0; i < koltabel; i++)
 {
  kodt=TABEL.ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",kodt);
  memset(naimtab,'\0',sizeof(naimtab));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(naimtab,row[0],sizeof(naimtab)-1);

  fprintf(ff,"%3d %-*.*s %10d %10d %10d\n",
  kodt,
  iceb_u_kolbait(30,naimtab),iceb_u_kolbait(30,naimtab),naimtab,
  DNIMUG.ravno(i),HASMUG.ravno(i),
  KDNMUG.ravno(i));

  s1+=DNIMUG.ravno(i);
  s2+=HASMUG.ravno(i);
  s3+=KDNMUG.ravno(i);

 }

fprintf(ff,"%3s %*s %10d %10d %10d\n\n",
" ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),s1,s2,s3);

sum=0.;
for(int i=0; i < kolnah; i++)
 {
  if(ZNAHMUG.ravno(i) == 0.)
    continue;
  kodt=NASH.ravno(i);
  sprintf(strsql,"select naik from Nash where kod=%d",kodt);
  memset(naimtab,'\0',sizeof(naimtab));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(naimtab,row[0],sizeof(naimtab)-1);

  fprintf(ff,"%3d %-*.*s %10.2f\n",
  kodt,iceb_u_kolbait(30,naimtab),iceb_u_kolbait(30,naimtab),naimtab,ZNAHMUG.ravno(i));
  sum+=ZNAHMUG.ravno(i);
 }

fprintf(ff,"%3s %*s %10.2f\n",
" ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),sum);

fprintf(ff,"\n%s\n",gettext("Мужчины совместители"));
fprintf(ff,"----------------------------------------------------------\n");

fprintf(ff,"\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8d\n\n",
iceb_u_kolbait(10,gettext("Основная")),gettext("Основная"),
tablic[2][0],
iceb_u_kolbait(10,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[2][1],
iceb_u_kolbait(10,gettext("Прочая")),gettext("Прочая"),
tablic[2][2],
iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),
tablic[2][0]+tablic[2][1]+tablic[2][2],
iceb_u_kolbait(10,gettext("Количество")),gettext("Количество"),kolmug_s);

s1=s2=s3=0;
for(int i=0; i < koltabel; i++)
 {
  kodt=TABEL.ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",kodt);
  memset(naimtab,'\0',sizeof(naimtab));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(naimtab,row[0],sizeof(naimtab)-1);

  fprintf(ff,"%3d %-*.*s %10d %10d %10d\n",
  kodt,
  iceb_u_kolbait(30,naimtab),iceb_u_kolbait(30,naimtab),naimtab,
  DNIMUG_S.ravno(i),HASMUG_S.ravno(i),
  KDNMUG_S.ravno(i));

  s1+=DNIMUG_S.ravno(i);
  s2+=HASMUG_S.ravno(i);
  s3+=KDNMUG_S.ravno(i);

 }
fprintf(ff,"%3s %*s %10d %10d %10d\n\n",
" ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),s1,s2,s3);

sum=0.;
for(int i=0; i < kolnah; i++)
 {
  if(ZNAHMUG_S.ravno(i) == 0.)
    continue;
  kodt=NASH.ravno(i);
  sprintf(strsql,"select naik from Nash where kod=%d",kodt);
  memset(naimtab,'\0',sizeof(naimtab));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(naimtab,row[0],sizeof(naimtab)-1);

  fprintf(ff,"%3d %-*.*s %10.2f\n",
  kodt,
  iceb_u_kolbait(30,naimtab),iceb_u_kolbait(30,naimtab),naimtab,
  ZNAHMUG_S.ravno(i));
  sum+=ZNAHMUG_S.ravno(i);
 }
fprintf(ff,"%3s %*s %10.2f\n",
" ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),sum);

sprintf(strsql,"\n%s:%s\n",gettext("Пол"),gettext("Мужчина"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
sprintf(strsql,"---------------------------------\n");

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8d\n",
iceb_u_kolbait(10,gettext("Основная")),gettext("Основная"),
tablic[0][0],
iceb_u_kolbait(10,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[0][1],
iceb_u_kolbait(10,gettext("Прочая")),gettext("Прочая"),
tablic[0][2],
iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),
tablic[0][0]+tablic[0][1]+tablic[0][2],
iceb_u_kolbait(10,gettext("Количество")),gettext("Количество"),kolmug);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff,"\n%s:%s\n",gettext("Пол"),gettext("Женщина"));
fprintf(ff,"----------------------------------------------------------\n");

fprintf(ff,"\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8d\n\n",
iceb_u_kolbait(10,gettext("Основная")),gettext("Основная"),
tablic[1][0],
iceb_u_kolbait(10,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[1][1],
iceb_u_kolbait(10,gettext("Прочая")),gettext("Прочая"),
tablic[1][2],
iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),
tablic[1][0]+tablic[1][1]+tablic[1][2],
iceb_u_kolbait(10,gettext("Количество")),gettext("Количество"),kolgen);
s1=s2=s3=0;
for(int i=0; i < koltabel; i++)
 {
  kodt=TABEL.ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",kodt);
  memset(naimtab,'\0',sizeof(naimtab));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(naimtab,row[0],sizeof(naimtab)-1);

  fprintf(ff,"%3d %-*.*s %10d %10d %10d\n",
  kodt,
  iceb_u_kolbait(30,naimtab),iceb_u_kolbait(30,naimtab),naimtab,
  DNIGEN.ravno(i),HASGEN.ravno(i),
  KDNGEN.ravno(i));
  s1+=DNIGEN.ravno(i);
  s2+=HASGEN.ravno(i);
  s3+=KDNGEN.ravno(i);
 }
fprintf(ff,"%3s %*s %10d %10d %10d\n\n",
" ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),s1,s2,s3);

sum=0.;
for(int i=0; i < kolnah; i++)
 {
  if(ZNAHGEN.ravno(i) == 0.)
    continue;
  kodt=NASH.ravno(i);
  sprintf(strsql,"select naik from Nash where kod=%d",kodt);
  memset(naimtab,'\0',sizeof(naimtab));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(naimtab,row[0],sizeof(naimtab)-1);

  fprintf(ff,"%3d %-*.*s %10.2f\n",
  kodt,iceb_u_kolbait(30,naimtab),iceb_u_kolbait(30,naimtab),naimtab,ZNAHGEN.ravno(i));
  sum+=ZNAHGEN.ravno(i);
 }
fprintf(ff,"%3s %*s %10.2f\n",
" ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),sum);

fprintf(ff,"\n%s\n",gettext("Женщины совместители"));
fprintf(ff,"----------------------------------------------------------\n");

fprintf(ff,"\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8d\n\n",
iceb_u_kolbait(10,gettext("Основная")),gettext("Основная"),
tablic[3][0],
iceb_u_kolbait(10,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[3][1],
iceb_u_kolbait(10,gettext("Прочая")),gettext("Прочая"),
tablic[3][2],
iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),
tablic[3][0]+tablic[3][1]+tablic[3][2],
iceb_u_kolbait(10,gettext("Количество")),gettext("Количество"),kolgen_s);
s1=s2=s3=0;
for(int i=0; i < koltabel; i++)
 {
  kodt=TABEL.ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",kodt);
  memset(naimtab,'\0',sizeof(naimtab));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(naimtab,row[0],sizeof(naimtab)-1);

  fprintf(ff,"%3d %-*.*s %10d %10d %10d\n",kodt,iceb_u_kolbait(30,naimtab),iceb_u_kolbait(30,naimtab),naimtab,
  DNIGEN_S.ravno(i),HASGEN_S.ravno(i),
  KDNGEN_S.ravno(i));
  s1+=DNIGEN_S.ravno(i);
  s2+=HASGEN_S.ravno(i);
  s3+=KDNGEN_S.ravno(i);
 }
fprintf(ff,"%3s %30s %10d %10d %10d\n\n",
" ",gettext("Итого"),s1,s2,s3);

sum=0.;
for(int i=0; i < kolnah; i++)
 {
  if(ZNAHGEN_S.ravno(i) == 0.)
    continue;
  kodt=NASH.ravno(i);
  sprintf(strsql,"select naik from Nash where kod=%d",kodt);
  memset(naimtab,'\0',sizeof(naimtab));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strncpy(naimtab,row[0],sizeof(naimtab)-1);

  fprintf(ff,"%3d %-*.*s %10.2f\n",
  kodt,iceb_u_kolbait(30,naimtab),iceb_u_kolbait(30,naimtab),naimtab,ZNAHGEN_S.ravno(i));
  sum+=ZNAHGEN_S.ravno(i);
 }
fprintf(ff,"%3s %*s %10.2f\n",
" ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),sum);

sprintf(strsql,"\n%s:%s\n",gettext("Пол"),gettext("Женщина"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
sprintf(strsql,"---------------------------------\n");

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8.2f\n\
%*s:%8d\n",
iceb_u_kolbait(10,gettext("Основная")),gettext("Основная"),
tablic[1][0],
iceb_u_kolbait(10,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[1][1],
iceb_u_kolbait(10,gettext("Прочая")),gettext("Прочая"),
tablic[1][2],
iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),
tablic[1][0]+tablic[1][1]+tablic[1][2],
iceb_u_kolbait(10,gettext("Количество")),gettext("Количество"),kolgen);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);



iceb_podpis(ff,data->window);
fclose(ff);

data->imaf->plus(imaf);
data->imaf->plus(imaf_prot);
data->naimf->plus(gettext("Расчёт распределения зарплат мужчины/женщины"));
data->naimf->plus(gettext("Протокол хода расчёта"));


for(int nom=0; nom < data->imaf->kolih(); nom++)
 iceb_ustpeh(data->imaf->ravno(nom),0,data->window);

data->kon_ras=0;
data->voz=0;
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
