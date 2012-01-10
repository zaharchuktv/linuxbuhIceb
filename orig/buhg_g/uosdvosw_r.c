/*$Id: uosdvosw_r.c,v 1.19 2011-07-06 08:12:30 sasa Exp $*/
/*15.03.2010	28.12.2007	Белых А.И.	uosdvosw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "uosdvosw.h"
#define         KOLSTSH  6  //Количество строк в шапке 

class uosdvosw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class uosdvosw_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  int metka_ras; //0-по подразделениям 1-по материально-ответственным
  
  uosdvosw_r_data()
   {
    metka_ras=0;
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosdvosw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosdvosw_r_data *data);
gint uosdvosw_r1(class uosdvosw_r_data *data);
void  uosdvosw_r_v_knopka(GtkWidget *widget,class uosdvosw_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern short	startgoduos; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;

int uosdvosw_r(int metka_ras,class uosdvosw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class uosdvosw_r_data data;
data.metka_ras=metka_ras;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
if(metka_ras == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Распечатка ведомости движения основных средств по подразделениям"));
if(metka_ras == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Распечатка ведомости движения основных средств по мат. ответственным"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosdvosw_r_key_press),&data);

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
if(metka_ras == 0)
  repl.plus(gettext("Распечатка ведомости движения основных средств по подразделениям"));
if(metka_ras == 1)
  repl.plus(gettext("Распечатка ведомости движения основных средств по мат. ответственным"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(uosdvosw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)uosdvosw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosdvosw_r_v_knopka(GtkWidget *widget,class uosdvosw_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosdvosw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosdvosw_r_data *data)
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
/*******/
/*Шапка*/
/*******/
void            shsss(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,  //Счетчик листов
int *kst, //Счетчик строк
int skl,char nai[],
int mt,  //0-по подразделению 1-по материально-ответств
FILE *ff,FILE *ffby)
{
*kst+=KOLSTSH;
char bros[512];

if(mt == 0)
  sprintf(bros,"%s: %d %s",gettext("Подразделение"),skl,nai);

if(mt == 1)
  sprintf(bros,"%s: %d %s",gettext("Мат.ответственный"),skl,nai);

fprintf(ff,"%-*s %s N%d\n\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n",
iceb_u_kolbait(115,bros),bros,gettext("Лист"),*sl);

fprintf(ff,gettext("\
Инвентарный|             Наименование               |          | Остаток на  %02d.%02d.%4dг.      |     Приход за период          |     Расход за период          |  Остаток на %02d.%02d.%4dг.      |\n\
   номер   |          основных  средств             |  Номер   |-------------------------------|------------------------------------------------------------------------------------------------\n\
	   |                                        |          |   Количество  |  Сумма        |    Количество |     Сумма     |    Количество |     Сумма     |   Количество  |    Сумма      |\n"),
dn,mn,gn,dk,mk,gk);


fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ffby,"\
%-*s %s N%d\n\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n",
iceb_u_kolbait(115,bros),bros,gettext("Лист"),*sl);

fprintf(ffby,gettext("\
Инвентарный|             Наименование               |          | Остаток на  %02d.%02d.%4dг.      |     Приход за период          |     Расход за период          |  Остаток на %02d.%02d.%4dг.      |\n\
   номер   |          основных  средств             |  Номер   |-------------------------------|------------------------------------------------------------------------------------------------\n\
	   |                                        |          |   Количество  |  Сумма        |    Количество |     Сумма     |    Количество |     Сумма     |   Количество  |    Сумма      |\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ffby,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

}

/*******/
/*Счетчик*/
/*******/
void            gsapp(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,  //Счетчик листов
int *kst,  //Счетчик строк
int skl,char nai[],
int mt,  //0-по подразделению 1-по материально-ответств
FILE *ff,FILE *ffby)
{

*kst=*kst+1;

if(*kst > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  fprintf(ffby,"\f");
  *sl+=1;
  *kst=0;
  shsss(dn,mn,gn,dk,mk,gk,sl,kst,skl,nai,mt,ff,ffby);
 }

}

/****************/
/*Итого по шифру для движения*/
/****************/
void itsh(const char *kod,int k1,int k2,int k3,int k4,
double s1,double s2,double s3,double s4, //Суммы по налоговому учету
double s1by,double s2by,double s3by,double s4by, //Суммы по бух. учету
int met,  //0-по подразделению 1-по материально-ответственному 2 по счету 3 по шифру 4 общий
int *kst,FILE *ff,FILE *ffby)
{
char		bros[512];
if(met == 4)
  sprintf(bros,gettext("Общий итог"));

if(met == 1)
  sprintf(bros,"%s %s",gettext("Итого по мат.ответсвенному"),kod);

if(met == 0)
 {
  sprintf(bros,"%s %s",gettext("Итого по подразделению"),kod);
 }

if(met == 2)
 {
  sprintf(bros,"%s %s",gettext("Итого по счету"),kod);
 }

if(met == 3)
 {
  sprintf(bros,"%s %s",gettext("Итого по шифру"),kod);
 }

if(k1+s1+k2+s2+k3+s3+k4+s4 > 0.0001 || met == 4 || met == 1)
 {
  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%11s %-*s %10s %15d %15.2f %15d %15.2f %15d %15.2f \
%15d %15.2f\n"," ",iceb_u_kolbait(40,bros),bros," ",
  k1,s1,k2,s2,k3,s3,k4,s4);
 }

if(k1+s1by+k2+s2by+k3+s3by+k4+s4by > 0.0001 || met == 4 || met == 1)
 {
  fprintf(ffby,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%11s %-*s %10s %15d %15.2f %15d %15.2f %15d %15.2f \
%15d %15.2f\n"," ",iceb_u_kolbait(40,bros),bros," ",
  k1,s1by,k2,s2by,k3,s3by,k4,s4by);
 }

*kst=*kst+2;
/************
printw("%s:\n\
%-10s %10d %10.2f %10.2f\n\
%-10s %10d %10.2f %10.2f\n\
%-10s %10d %10.2f %10.2f\n\
%-10s %10d %10.2f %10.2f\n\n",
bros,
gettext("Остаток"),
k1,s1,s1by,
gettext("Приход"),
k2,s2,s2by,
gettext("Расход"),
k3,s3,s3by,
gettext("Остаток"),
k4,s4,s4by);
refresh();
*******************/
}

/****************/
/*Итоги для узкой форми*/
/****************/
void uosdvos_itog(const char *kod,int k1,int k2,int k3,int k4,
double s1,double s2,double s3,double s4, //Суммы по налоговому учету
double s1by,double s2by,double s3by,double s4by, //Суммы по бух. учету
int met,  //0-по подразделению 1-по материально-ответственному 2 по счету 3 по шифру 4 общий
int *kst,FILE *ff_nal,FILE *ff_buh)
{
char		bros[512];
if(met == 4)
  sprintf(bros,gettext("Общий итог"));

if(met == 1)
  sprintf(bros,"%s %s",gettext("Итого по мат.ответсвенному"),kod);

if(met == 0)
 {
  sprintf(bros,"%s %s",gettext("Итого по подразделению"),kod);
 }

if(met == 2)
 {
  sprintf(bros,"%s %s",gettext("Итого по счету"),kod);
 }

if(met == 3)
 {
  sprintf(bros,"%s %s",gettext("Итого по шифру"),kod);
 }


if(k1+s1+k2+s2+k3+s3+k4+s4 > 0.0001 || met == 4 || met == 1)
 {
  fprintf(ff_nal,"\
----------------------------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff_nal,"\
%11s %-*s %10s %8d %12.2f %8d %12.2f %8d %12.2f \
%8d %12.2f\n"," ",iceb_u_kolbait(30,bros),bros," ",
  k1,s1,k2,s2,k3,s3,k4,s4);
 }

if(k1+s1by+k2+s2by+k3+s3by+k4+s4by > 0.0001 || met == 4 || met == 1)
 {
  fprintf(ff_buh,"\
----------------------------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff_buh,"\
%11s %-*s %10s %8d %12.2f %8d %12.2f %8d %12.2f \
%8d %12.2f\n"," ",iceb_u_kolbait(30,bros),bros," ",
  k1,s1by,k2,s2by,k3,s3by,k4,s4by);
 }

*kst+=2;

}



/*********************************/
/*Шапка для узкой форми*/
/**********************************/
void uosdvos_suf(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,  //Счетчик листов
int *kst, //Счетчик строк
int skl,char nai[],
int mt,  //0-по подразделению 1-по материально-ответств
FILE *ff_nal,FILE *ff_buh)
{
//*sl+=1; //На одном и томже листе может быть выведено несколько шапок
*kst+=KOLSTSH;

char		bros[512];

if(mt == 0)
  sprintf(bros,"%s: %d %s",gettext("Подразделение"),skl,nai);

if(mt == 1)
  sprintf(bros,"%s: %d %s",gettext("Мат.ответственный"),skl,nai);


fprintf(ff_nal,"\
%-*s %s N%d\n\
----------------------------------------------------------------------------------------------------------------------------------------------\n",
iceb_u_kolbait(115,bros),bros,
gettext("Лист"),
*sl);

fprintf(ff_nal,gettext("\
Инвентарный|          Наименование        |          |Остаток на %02d.%02d.%4d|   Приход за период  |  Расход за период   |Остаток на %02d.%02d.%4d|\n\
   номер   |       основных  средств      |  Номер   |---------------------|---------------------|---------------------|----------------------\n\
           |                              |          |Количест|  Сумма     |Количест|    Сумма   |Количест|   Сумма    |Количест|  Сумма     |\n"),
dn,mn,gn,dk,mk,gk);


fprintf(ff_nal,"\
----------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_buh,"\
%-*s %s N%d\n\
----------------------------------------------------------------------------------------------------------------------------------------------\n",
iceb_u_kolbait(115,bros),bros,
gettext("Лист"),
*sl);

fprintf(ff_buh,gettext("\
Инвентарный|          Наименование        |          |Остаток на %02d.%02d.%4d|   Приход за период  |  Расход за период   |Остаток на %02d.%02d.%4d|\n\
   номер   |       основных  средств      |  Номер   |---------------------|---------------------|---------------------|----------------------\n\
           |                              |          |Количест|  Сумма     |Количест|    Сумма   |Количест|   Сумма    |Количест|  Сумма     |\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff_buh,"\
----------------------------------------------------------------------------------------------------------------------------------------------\n");
/*
12345678901 123456789012345678901234567890 1234567890 12345678 123456789012 12345678 123456789012 12345678 123456789012 12345678 123456789012 
*/



}
/*******/
/*Счетчик для узкой форми*/
/*******/
void  uosdvos_shet_uf(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,  //Счетчик листов
int *kst,  //Счетчик строк
int skl,char nai[],
int mt,  //0-по подразделению 1-по материально-ответств
FILE *ff_nal,FILE *ff_buh)
{

*kst+=1;
if(*kst <= kol_strok_na_liste)
 return;

fprintf(ff_nal,"\f");
fprintf(ff_buh,"\f");
*sl+=1;
*kst=1;
uosdvos_suf(dn,mn,gn,dk,mk,gk,sl,kst,skl,nai,mt,ff_nal,ff_buh);

}
/****************************/
/****************************/
int uosdvos_rek(int in,short dn,short mn,short gn,
int mt,
int metkasort,
const char *naimos,
const char *podrz,
const char *kodolz,
const char *hnaz,
const char *hnabyz,
const char *shetupoi,
const char *hauz,
int matotn,
int podrn,
FILE *ff,
GtkWidget *wpredok)
{

class poiinpdw_data rekin;
poiinpdw(in,mn,gn,&rekin,wpredok);

if(rekin.shetu.ravno()[0] == '\0')
  rekin.shetu.new_plus("???");    

if(iceb_u_proverka(hnaz,rekin.hna.ravno(),0,0) != 0)
 return(1);

if(iceb_u_proverka(hnabyz,rekin.hnaby.ravno(),0,0) != 0)
 return(1);

if(iceb_u_proverka(shetupoi,rekin.shetu.ravno(),1,0) != 0)
 return(1);

if(iceb_u_proverka(hauz,rekin.hau.ravno(),0,0) != 0)
 return(1);

if(mt == 0)                             
 {
  if(metkasort == 0)
    fprintf(ff,"%s|%d|%d|%s|%d|%s|\n",rekin.shetu.ravno(),podrn,in,naimos,matotn,rekin.nomz.ravno());
  if(metkasort == 1)
    fprintf(ff,"%s|%d|%d|%s|%d|%s|\n",rekin.hzt.ravno(),podrn,in,naimos,matotn,rekin.nomz.ravno());
 }

if(mt == 1)
 {
  if(metkasort == 0)
   fprintf(ff,"%s|%d|%d|%s|%d|%s|\n",rekin.shetu.ravno(),matotn,in,naimos,podrn,rekin.nomz.ravno());
  if(metkasort == 1)
   fprintf(ff,"%s|%d|%d|%s|%d|%s|\n",rekin.hzt.ravno(),matotn,in,naimos,podrn,rekin.nomz.ravno());
 }

return(0);
}

/*************************************/
/*Если вернули 0 всё впорядке
               1 не подходит
               2 ещё не пришло или уже списано
******************** */
int uosdvos_poi(int mt,int in,
short dn,short mn,short gn,
short dk,short mk,short gk,
int metkasort,
const char *naimos,
const char *podrz,
const char *kodolz,
const char *hnaz,
const char *hnabyz,
const char *shetupoi,
const char *hauz,
FILE *ff,
GtkWidget *wpredok)
{
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[512];
int kolstr=0;
int podrn=0;
int matotn=0;

/*Узнаём где находится на начало периода*/
if(poiinw(in,dn,mn,gn,&podrn,&matotn,wpredok) == 0)
 {
  if(iceb_u_proverka(podrz,podrn,0,0) == 0)
   {
    if(iceb_u_proverka(kodolz,matotn,0,0) == 0)
     {
     uosdvos_rek(in,dn,mn,gn,mt,metkasort,naimos,podrz,kodolz,hnaz,hnabyz,shetupoi,hauz,matotn,podrn,ff,wpredok);
     }
   }
 }

/*просмотриваем период -может быть куча внутренних перемещений*/

sprintf(strsql,"select datd,podr,nomd from Uosdok1 where \
datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' and innom=%d and podt=1 \
order by datd asc, tipz desc",gn,mn,dn,gk,mk,dk,in);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(podrz,row[1],0,0) != 0)
   continue;
  podrn=atoi(row[1]);
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  /**************Читаем документ*/
  sprintf(strsql,"select kodol from Uosdok where datd='%s' and \
  nomd='%s'",row[0],row[2]);

  //printw("%s\n",strsql);

  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"poiin-Не найден документ N%s от %s !",row[2],row[0]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }
  else
   {
    matotn=atoi(row1[0]);
   }
  if(iceb_u_proverka(kodolz,matotn,0,0) != 0)
   continue;

  uosdvos_rek(in,d,m,g,mt,metkasort,naimos,podrz,kodolz,hnaz,hnabyz,shetupoi,hauz,matotn,podrn,ff,wpredok);
 }


return(0);
}

/******************************************/
/******************************************/

gint uosdvosw_r1(class uosdvosw_r_data *data)
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



int vids=2;
if(data->rk->metka_sort == 1)
 vids=3;      
int kolstr=0;
sprintf(strsql,"select innom,naim from Uosin");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
if(kolstr == 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
FILE *ff;
char imaftmp[50];

sprintf(imaftmp,"uos%d.tmp",getpid());

if((ff = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
float kolstr1=0;
int in=0;
//int kolstr2=0;
SQL_str row1;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s\n",row[0],row[1]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
      continue;

  in=atoi(row[0]);

  /*Смотрим где оно было на начало периода и в периоде*/
  uosdvos_poi(data->metka_ras,in,dn,mn,gn,dk,mk,gk,data->rk->metka_sort,row[1],data->rk->podr.ravno(),data->rk->mat_ot.ravno(),data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->shetu.ravno(),data->rk->hau.ravno(),ff,data->window);

 }
fclose(ff);

//Сортировка с удалением дублированных записей
//sprintf(strsql,"sort -o %s -t\\| +0 -1 +1n -2 +2n -3 -u %s",imaftmp,imaftmp);
sprintf(strsql,"sort -o %s -t\\| -k1,2 -k2,3n -k3,4n -u %s",imaftmp,imaftmp);
system(strsql);
struct stat work;
stat(imaftmp,&work);
FILE *ff1;
if((ff1 = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafby[50];
char imaf[56];
if(data->metka_ras == 0)
 {
  sprintf(imafby,"vdpbu%d.lst",getpid());
  sprintf(imaf,"vdpod%d.lst",getpid());
 }
if(data->metka_ras == 1)
 {
  sprintf(imafby,"vdobu%d.lst",getpid());
  sprintf(imaf,"vdotl%d.lst",getpid());
 }

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ffby;
if((ffby = fopen(imafby,"w")) == NULL)
 {
  iceb_er_op_fil(imafby,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



//файлы для узкой формы распечатки 
char imaf_buh[56];
char imaf_nal[56];
FILE *ff_buh;
FILE *ff_nal;
int sl_uz=0; //Счётчик листов для узкой форми
int kolstrok_uz=0; //Счётчик строк для узкой форми

sprintf(imaf_buh,"vdbuh%d.lst",getpid());
sprintf(imaf_nal,"vdnal%d.lst",getpid());

if((ff_buh = fopen(imaf_buh,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_buh,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



if((ff_nal = fopen(imaf_nal,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_nal,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



sprintf(strsql,"%s. (%s.)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Налоговый учет"));
iceb_u_zagolov(strsql,dn,mn,gn,dk,mk,gk,organ,ff);
iceb_u_zagolov(strsql,dn,mn,gn,dk,mk,gk,organ,ff_nal);

sprintf(strsql,"%s. (%s.)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Бухгалтерский учет"));
iceb_u_zagolov(strsql,dn,mn,gn,dk,mk,gk,organ,ffby);
iceb_u_zagolov(strsql,dn,mn,gn,dk,mk,gk,organ,ff_buh);

kolstrok_uz=5;
sl_uz=1;
int sli=1;
int kst=5;
int klst=0;
if(data->rk->podr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),data->rk->podr.ravno());
  iceb_printcod(ff,"Uospod","kod","naik",0,data->rk->podr.ravno(),&klst);
  fprintf(ffby,"%s: %s\n",gettext("Подразделение"),data->rk->podr.ravno());
  iceb_printcod(ffby,"Uospod","kod","naik",0,data->rk->podr.ravno(),&klst);

  fprintf(ff_buh,"%s: %s\n",gettext("Подразделение"),data->rk->podr.ravno());
  iceb_printcod(ff_buh,"Uospod","kod","naik",0,data->rk->podr.ravno(),&klst);
  fprintf(ff_nal,"%s: %s\n",gettext("Подразделение"),data->rk->podr.ravno());
  iceb_printcod(ff_nal,"Uospod","kod","naik",0,data->rk->podr.ravno(),&klst);

  kolstrok_uz+=klst+1;
  kst+=klst+1;
 }
if(data->rk->grupa_bu.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа налогового учета"),data->rk->grupa_bu.ravno());
  iceb_printcod(ff,"Uosgrup","kod","naik",1,data->rk->grupa_bu.ravno(),&klst);

  fprintf(ffby,"%s: %s\n",gettext("Группа налогового учета"),data->rk->grupa_bu.ravno());
  iceb_printcod(ffby,"Uosgrup","kod","naik",1,data->rk->grupa_bu.ravno(),&klst);

  fprintf(ff_nal,"%s: %s\n",gettext("Группа налогового учета"),data->rk->grupa_bu.ravno());
  iceb_printcod(ff_nal,"Uosgrup","kod","naik",1,data->rk->grupa_bu.ravno(),&klst);

  fprintf(ff_buh,"%s: %s\n",gettext("Группа налогового учета"),data->rk->grupa_bu.ravno());
  iceb_printcod(ff_buh,"Uosgrup","kod","naik",1,data->rk->grupa_bu.ravno(),&klst);

  kolstrok_uz+=klst+1;
  kst+=klst+1;
 }
if(data->rk->grupa_bu.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учета"),data->rk->grupa_bu.ravno());
  iceb_printcod(ff,"Uosgrup1","kod","naik",1,data->rk->grupa_bu.ravno(),&klst);

  fprintf(ffby,"%s: %s\n",gettext("Группа бухгалтерского учета"),data->rk->grupa_bu.ravno());
  iceb_printcod(ffby,"Uosgrup1","kod","naik",1,data->rk->grupa_bu.ravno(),&klst);

  fprintf(ff_nal,"%s: %s\n",gettext("Группа бухгалтерского учета"),data->rk->grupa_bu.ravno());
  iceb_printcod(ff_nal,"Uosgrup1","kod","naik",1,data->rk->grupa_bu.ravno(),&klst);

  fprintf(ff_buh,"%s: %s\n",gettext("Группа бухгалтерского учета"),data->rk->grupa_bu.ravno());
  iceb_printcod(ff_buh,"Uosgrup1","kod","naik",1,data->rk->grupa_bu.ravno(),&klst);

  kolstrok_uz+=klst+1;
  kst+=klst+1;
 }
if(data->rk->mat_ot.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Мат.-ответственный"),data->rk->mat_ot.ravno());
  iceb_printcod(ff,"Uosol","kod","naik",1,data->rk->mat_ot.ravno(),&klst);
  fprintf(ffby,"%s: %s\n",gettext("Мат.-ответственный"),data->rk->mat_ot.ravno());
  iceb_printcod(ffby,"Uosol","kod","naik",0,data->rk->mat_ot.ravno(),&klst);

  fprintf(ff_nal,"%s: %s\n",gettext("Мат.-ответственный"),data->rk->mat_ot.ravno());
  iceb_printcod(ff_nal,"Uosol","kod","naik",1,data->rk->mat_ot.ravno(),&klst);
  fprintf(ff_buh,"%s: %s\n",gettext("Мат.-ответственный"),data->rk->mat_ot.ravno());
  iceb_printcod(ff_buh,"Uosol","kod","naik",0,data->rk->mat_ot.ravno(),&klst);

  kolstrok_uz+=klst+1;
  kst+=klst+1;
 }
if(data->rk->shetu.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счет"),data->rk->shetu.ravno());
  fprintf(ffby,"%s: %s\n",gettext("Счет"),data->rk->shetu.ravno());

  fprintf(ff_nal,"%s: %s\n",gettext("Счет"),data->rk->shetu.ravno());
  fprintf(ff_buh,"%s: %s\n",gettext("Счет"),data->rk->shetu.ravno());

  kolstrok_uz++;
  kst++;
 }
if(data->rk->hau.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учета"),data->rk->hau.ravno());
  iceb_printcod(ff,"Uoshau","kod","naik",0,data->rk->hau.ravno(),&klst);
  fprintf(ffby,"%s: %s\n",gettext("Шифр аналитического учета"),data->rk->hau.ravno());
  iceb_printcod(ffby,"Uoshau","kod","naik",0,data->rk->hau.ravno(),&klst);

  fprintf(ff_nal,"%s: %s\n",gettext("Шифр аналитического учета"),data->rk->hau.ravno());
  iceb_printcod(ff_nal,"Uoshau","kod","naik",0,data->rk->hau.ravno(),&klst);
  fprintf(ff_buh,"%s: %s\n",gettext("Шифр аналитического учета"),data->rk->hau.ravno());
  iceb_printcod(ff_buh,"Uoshau","kod","naik",0,data->rk->hau.ravno(),&klst);

  kolstrok_uz+=klst+1;
  kst+=klst+1;
 }
//memset(str,'\0',sizeof(str));

int k1=0,k2=0,k3=0,k4=0,kk1=0,kk2=0,kk3=0,kk4=0;
int k11=0,k21=0,k31=0,k41=0;
double ss1=0,s2=0,ss2=0,s3=0,ss3=0,s4=0,ss4=0.;
//bs11=bs21=bs31=s11=s21=s31=s41=0.;
int mnp=0;   /* 0-старое подразделение 1-новое*/

in=0;
int podz=-1;
char shetuz[56];
char shetu[56];
memset(shetuz,'\0',sizeof(shetuz));
kolstr=0;
class ostuosw_data ost_in;
char nomz[112];
char strr[1024];
int pod=0;
class iceb_u_str naimos("");
double		ss1by=0.,ss2by=0.,ss3by=0.; /* Итоги по подр*/
double          s4by=0.,ss4by=0.;
double		s1by=0.,s2by=0.,s3by=0.;
double		s1=0.;
double          s11=0.,s21=0.,s31=0.,s41=0.;
double          s11by=0.,s21by=0.,s31by=0.,s41by=0.;
char naipod[512];
class SQLCURSOR curr;
char naishet[512];
while(fgets(strr,sizeof(strr),ff1) != NULL)
 {
// printw("%s",strr);
  kolstr+=strlen(strr);
//  strzag(LINES-1,0,work.st_size,kolstr);
  iceb_pbar(data->bar,work.st_size,kolstr);    

  iceb_u_pole(strr,shetu,1,'|');
  iceb_u_polen(strr,&pod,2,'|');

  iceb_u_polen(strr,&in,3,'|');

  iceb_u_polen(strr,&naimos,4,'|');

  iceb_u_polen(strr,nomz,sizeof(nomz),6,'|');
  

  if(podz != pod)
   {
    mnp=1;
    if(podz != -1)
     {
      sprintf(strsql,"%d",podz);
      itsh(strsql,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ss1by,ss2by,ss3by,ss4by,data->metka_ras,&kst,ff,ffby);
      uosdvos_itog(strsql,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ss1by,ss2by,ss3by,ss4by,data->metka_ras,&kolstrok_uz,ff_nal,ff_buh);
      ss1=ss2=ss3=ss4=0.;
      ss1by=ss2by=ss3by=ss4by=0.;
      kk1=kk2=kk3=kk4=0;
     }

    if(data->metka_ras == 0)
     {
//      iceb_printw(iceb_u_toutf(strr),data->buffer,data->view);
      /*Читаем наименование подразделения*/
      memset(naipod,'\0',sizeof(naipod));
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        sprintf(strsql,"Не найден код подразделения %d !",pod);
        iceb_menu_soob(strsql,data->window);
        fprintf(ff,"Не найден код подразделения %d !\n",pod);
       }
      else
        strncpy(naipod,row1[0],sizeof(naipod)-1);
     }

    if(data->metka_ras == 1)
     {
      /*Читаем мат.-ответственного*/
      memset(naipod,'\0',sizeof(naipod));
      sprintf(strsql,"select naik from Uosol where kod=%d",pod);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        sprintf(strsql,"Не найден мат.-ответственный %d !",pod);
        iceb_menu_soob(strsql,data->window);
        fprintf(ff,"Не найден мат.-ответственный %d !\n",pod);
       }
      else
        strncpy(naipod,row1[0],sizeof(naipod)-1);
     }

    podz=pod;
   }

  if(iceb_u_SRAV(shetu,shetuz,0) != 0)
   {
    if(shetuz[0] != '\0')
     {
      itsh(shetuz,k1,k2,k3,k4,s1,s2,s3,s4,s1by,s2by,s3by,s4by,vids,&kst,ff,ffby);
      uosdvos_itog(shetuz,k1,k2,k3,k4,s1,s2,s3,s4,s1by,s2by,s3by,s4by,vids,&kolstrok_uz,ff_nal,ff_buh);
      s1=s2=s3=s4=0.;
      s1by=s2by=s3by=s4by=0.;
      k1=k2=k3=k4=0;
     }
    if(data->rk->metka_sort == 0)
     {
      memset(naishet,'\0',sizeof(naishet));
      sprintf(strsql,"select nais from Plansh where ns='%s'",shetu);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        sprintf(strsql,gettext("Не найден счет %s в плане счетов !"),shetu);
        iceb_menu_soob(strsql,data->window);
        fprintf(ff,gettext("Не найден счет %s в плане счетов !"),shetu);

        fprintf(ff,"\n");
       }
      else
        strncpy(naishet,row1[0],sizeof(naipod)-1);
      fprintf(ff,"%s: %s %s\n",gettext("Счет"),shetu,naishet);
      fprintf(ffby,"%s: %s %s\n",gettext("Счет"),shetu,naishet);
      kst++;
     }
    if(data->rk->metka_sort == 1)
     {
      memset(naishet,'\0',sizeof(naishet));
      sprintf(strsql,"select naik from Uoshz where kod='%s'",shetu);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден шифр производственных затрат"),shetu);
        iceb_menu_soob(strsql,data->window);
        fprintf(ff,"%s %s !\n",gettext("Не найден шифр производственных затрат"),shetu);
       }
      else
        strncpy(naishet,row1[0],sizeof(naipod)-1);
      fprintf(ff,"%s: %s %s\n",gettext("Шифр затрат"),shetu,naishet);
      fprintf(ffby,"%s: %s %s\n",gettext("Шифр затрат"),shetu,naishet);
      kst++;
     }

    strcpy(shetuz,shetu);
   }

  ostuosw(in,dn,mn,gn,dk,mk,gk,pod,data->metka_ras,&ost_in,data->window);
  
  
  if(ost_in.ostm[0] != 0 || ost_in.ostm[1] != 0 || ost_in.ostm[3] != 0)
   {
    if(mnp == 1)
     {
      if(kst > kol_strok_na_liste-KOLSTSH-5)
       {
        fprintf(ff,"\f");
        fprintf(ffby,"\f");
        fprintf(ff_nal,"\f");
        fprintf(ff_buh,"\f");
        sli++;
        sl_uz++;
        kst=0;
        kolstrok_uz=0; //Сразу за выводом шапки идет запуск счётчика
       }
      else
       {
        gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,pod,naipod,data->metka_ras,ff,ffby);
        uosdvos_shet_uf(dn,mn,gn,dk,mk,gk,&sl_uz,&kolstrok_uz,pod,naipod,data->metka_ras,ff_nal,ff_buh);
        fprintf(ff,"\n");
        fprintf(ffby,"\n");
        fprintf(ff_nal,"\n");
        fprintf(ff_buh,"\n");
       }               

      if(data->metka_ras == 0)
       sprintf(strsql,"\n%s - %d/%s\n",
       gettext("Поразделение"),podz,naipod);

      if(data->metka_ras == 1)
       sprintf(strsql,"\n%s - %d/%s\n",
       gettext("Мат.-ответственный"),podz,naipod);

      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

      shsss(dn,mn,gn,dk,mk,gk,&sli,&kst,pod,naipod,data->metka_ras,ff,ffby);
      uosdvos_suf(dn,mn,gn,dk,mk,gk,&sl_uz,&kolstrok_uz,pod,naipod,data->metka_ras,ff_nal,ff_buh);
      mnp=0;
     }
    gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,pod,naipod,data->metka_ras,ff,ffby);
    uosdvos_shet_uf(dn,mn,gn,dk,mk,gk,&sl_uz,&kolstrok_uz,pod,naipod,data->metka_ras,ff_nal,ff_buh);

    fprintf(ff,"\
%11d %-*s %-*.*s %15d %15.2f %15d %15.2f %15d %15.2f \
%15d %15.2f\n",
    in,
    iceb_u_kolbait(40,naimos.ravno()),naimos.ravno(),
    iceb_u_kolbait(10,nomz),iceb_u_kolbait(10,nomz),nomz,
    ost_in.ostm[0],ost_in.ostmc[0],ost_in.ostm[1],ost_in.ostmc[1],ost_in.ostm[2],ost_in.ostmc[2],ost_in.ostm[3],ost_in.ostmc[3]);

    fprintf(ffby,"\
%11d %-*s %-*.*s %15d %15.2f %15d %15.2f %15d %15.2f \
%15d %15.2f\n",
    in,
    iceb_u_kolbait(40,naimos.ravno()),naimos.ravno(),
    iceb_u_kolbait(10,nomz),iceb_u_kolbait(10,nomz),nomz,
    ost_in.ostm[0],ost_in.ostmcby[0],ost_in.ostm[1],ost_in.ostmcby[1],ost_in.ostm[2],ost_in.ostmcby[2],ost_in.ostm[3],ost_in.ostmcby[3]);

    fprintf(ff_nal,"\
%11d %-*.*s %-*.*s %8d %12.2f %8d %12.2f %8d %12.2f %8d %12.2f\n",
    in,
    iceb_u_kolbait(30,naimos.ravno()),iceb_u_kolbait(30,naimos.ravno()),naimos.ravno(),
    iceb_u_kolbait(10,nomz),iceb_u_kolbait(10,nomz),nomz,
    ost_in.ostm[0],ost_in.ostmc[0],ost_in.ostm[1],ost_in.ostmc[1],ost_in.ostm[2],ost_in.ostmc[2],ost_in.ostm[3],ost_in.ostmc[3]);

    fprintf(ff_buh,"\
%11d %-*.*s %-*.*s %8d %12.2f %8d %12.2f %8d %12.2f %8d %12.2f\n",
    in,
    iceb_u_kolbait(30,naimos.ravno()),iceb_u_kolbait(30,naimos.ravno()),naimos.ravno(),
    iceb_u_kolbait(10,nomz),iceb_u_kolbait(10,nomz),nomz,
    ost_in.ostm[0],ost_in.ostmcby[0],ost_in.ostm[1],ost_in.ostmcby[1],ost_in.ostm[2],ost_in.ostmcby[2],ost_in.ostm[3],ost_in.ostmcby[3]);

    if(iceb_u_strlen(naimos.ravno()) > 30)
     {
      uosdvos_shet_uf(dn,mn,gn,dk,mk,gk,&sl_uz,&kolstrok_uz,pod,naipod,data->metka_ras,ff_nal,ff_buh);
      fprintf(ff_nal,"%11s %s\n","",iceb_u_adrsimv(30,naimos.ravno()));
      fprintf(ff_buh,"%11s %s\n","",iceb_u_adrsimv(30,naimos.ravno()));
     }
    k1+=ost_in.ostm[0];   kk1+=ost_in.ostm[0];   k11+=ost_in.ostm[0];
    s1+=ost_in.ostmc[0];  ss1+=ost_in.ostmc[0];  s11+=ost_in.ostmc[0];
    s1by+=ost_in.ostmcby[0];  ss1by+=ost_in.ostmcby[0];  s11by+=ost_in.ostmcby[0];

    k2+=ost_in.ostm[1];   kk2+=ost_in.ostm[1];   k21+=ost_in.ostm[1];
    s2+=ost_in.ostmc[1];  ss2+=ost_in.ostmc[1];  s21+=ost_in.ostmc[1];
    s2by+=ost_in.ostmcby[1];  ss2by+=ost_in.ostmcby[1];  s21by+=ost_in.ostmcby[1];

    k3+=ost_in.ostm[2];   kk3+=ost_in.ostm[2];   k31+=ost_in.ostm[2];
    s3+=ost_in.ostmc[2];  ss3+=ost_in.ostmc[2];  s31+=ost_in.ostmc[2];
    s3by+=ost_in.ostmcby[2];  ss3by+=ost_in.ostmcby[2];  s31by+=ost_in.ostmcby[2];

    k4+=ost_in.ostm[3];   kk4+=ost_in.ostm[3];   k41+=ost_in.ostm[3];
    s4+=ost_in.ostmc[3];  ss4+=ost_in.ostmc[3];  s41+=ost_in.ostmc[3];
    s4by+=ost_in.ostmcby[3];  ss4by+=ost_in.ostmcby[3];  s41by+=ost_in.ostmcby[3];
   }
 }

fclose(ff1);

unlink(imaftmp);

sprintf(strsql,"%d",podz);
itsh(strsql,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ss1by,ss2by,ss3by,ss4by,data->metka_ras,&kst,ff,ffby);
uosdvos_itog(strsql,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ss1by,ss2by,ss3by,ss4by,data->metka_ras,&kolstrok_uz,ff_nal,ff_buh);

itsh(shetuz,k1,k2,k3,k4,s1,s2,s3,s4,s1by,s2by,s3by,s4by,vids,&kst,ff,ffby);
uosdvos_itog(shetuz,k1,k2,k3,k4,s1,s2,s3,s4,s1by,s2by,s3by,s4by,vids,&kolstrok_uz,ff_nal,ff_buh);

/*Итог по организации*/
if(data->rk->podr.ravno()[0] == '\0')
 {
  itsh("0",k11,k21,k31,k41,s11,s21,s31,s41,s11by,s21by,s31by,s41by,4,&kst,ff,ffby);
  uosdvos_itog("0",k11,k21,k31,k41,s11,s21,s31,s41,s11by,s21by,s31by,s41by,4,&kolstrok_uz,ff_nal,ff_buh);
 }
else
 {
  itsh(strsql,k11,k21,k31,k41,s11,s21,s31,s41,s11by,s21by,s31by,s41by,data->metka_ras,&kst,ff,ffby);
  uosdvos_itog(strsql,k11,k21,k31,k41,s11,s21,s31,s41,s11by,s21by,s31by,s41by,data->metka_ras,&kolstrok_uz,ff_nal,ff_buh);
  /*Итог по органищации представляем как по подразделению*/
 }
iceb_podpis(ff,data->window);
iceb_podpis(ffby,data->window);
iceb_podpis(ff_nal,data->window);
iceb_podpis(ff_buh,data->window);

fclose(ff);
fclose(ffby);
fclose(ff_nal);
fclose(ff_buh);



data->rk->imaf.plus(imaf_nal);

sprintf(strsql,"%s (%s A3)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Налоговый учёт"));
data->rk->naim.plus(strsql);

data->rk->imaf.plus(imaf_buh);

sprintf(strsql,"%s (%s A3)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Бухгалтерский учёт"));
data->rk->naim.plus(strsql);

data->rk->imaf.plus(imaf);

sprintf(strsql,"%s (%s A4)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Налоговый учёт"));
data->rk->naim.plus(strsql);

data->rk->imaf.plus(imafby);

sprintf(strsql,"%s (%s A4)",
gettext("Оборотная ведомость движения основных средств"),
gettext("Бухгалтерский учёт"));
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
