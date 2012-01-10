/*$Id: uosvosw_r.c,v 1.17 2011-08-29 07:13:44 sasa Exp $*/
/*12.02.2010	14.01.2008	Белых А.И.	uosvosw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "uosvosw.h"
#define         KOLSTSH  6  /*Количество строк в шапке*/

class uosvosw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class uosvosw_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  int metka_r;  
  uosvosw_r_data()
   {
    metka_r=0;
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosvosw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvosw_r_data *data);
gint uosvosw_r1(class uosvosw_r_data *data);
void  uosvosw_r_v_knopka(GtkWidget *widget,class uosvosw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern short	startgoduos; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;
//extern char	*spgnunpa; //Список групп налогового учета с не пообъектной амортизацией
extern char	*spgnu; //Список групп налогового учета

int uosvosw_r(int metka_r,class uosvosw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class uosvosw_r_data data;
data.rk=datark;
data.metka_r=metka_r;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

if(metka_r == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Распечатать ведомость остаточной стоимости (налоговый учёт)"));
if(metka_r == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Распечатать ведомость остаточной стоимости (бух. учёт)"));

//sprintf(strsql,"%s %s",name_system,gettext("Распечатка ведомости аморт-отчислений для налогового учёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosvosw_r_key_press),&data);

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
if(metka_r == 0)
  repl.plus(gettext("Распечатать ведомость остаточной стоимости (налоговый учёт)"));
if(metka_r == 1)
  repl.plus(gettext("Распечатать ведомость остаточной стоимости (бух. учёт)"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(uosvosw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)uosvosw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosvosw_r_v_knopka(GtkWidget *widget,class uosvosw_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosvosw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvosw_r_data *data)
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

/****************/
/*Итого по шифру и подразделению для износа*/
/****************/
void uosvos_ithz(const char *sh,double k1,double s1,
short mt,  //0-по подразделению 1- по шифру 2-общий итог 3-по счету учёта
int *kst,
double *iznos_per,
class bsiz_per *data_i,
FILE *ff,FILE *ffhpz)
{
char		bros[512];
char		str[512];

memset(str,'\0',sizeof(str));
memset(str,'-',93);

if(mt == 2)
 {
  sprintf(bros,gettext("Общий итог"));
 }

if(mt == 1)
 {
  if(sh[0] == '\0')
   return;
  sprintf(bros,"%s %s",gettext("Итого по шифру"),sh);
  if(ffhpz != NULL)
   fprintf(ffhpz,"%s\n\
%11s %-*s %10s %12.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",k1,s1,k1-s1);
 }

if(mt == 0)
 {
  if(k1+s1+(k1-s1) < 0.0001)
   return;
  sprintf(bros,"%s %s",gettext("Итого по подразделению"),sh);
 }

if(mt == 3)
 {
  if(k1+s1+(k1-s1) < 0.0001)
   return;
  sprintf(bros,"%s %s",gettext("Итого по счёту"),sh);
 }

fprintf(ff,"%s\n\
%11s %-*s %10s %12.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",k1,s1,k1-s1);

 
if(mt == 2 || mt == 3)
 if(ffhpz != NULL)
  fprintf(ffhpz,"%s\n\
%11s %-*s %10s %12.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",k1,s1,k1-s1);

*kst=*kst+2;
/***********
printw("\n%s:\n\
%s %12.2f %12.2f %12.2f\n",
bros,
gettext("Баланс. ст./Износ/Ост.ст.:"),
k1,s1,k1-s1);
************/
data_i->clear_data(); //обнуляем массив

}
/********/
/*Шапка*/
/*******/

void uosvos_hhap(short dk,short mk,short gk,
int sl, //Счетчик листов
int *kst,  //Счетчик строк
int skl,
const char *nai,
short mt,  //0-по подразделению 1-по материально-ответств 
FILE *ff)
{
char		str[512];
char		bros[512];

if(mt == 0)
 sprintf(bros,"%s %d %s",gettext("Подразделение"),skl,nai);

if(mt == 1)
 sprintf(bros,"%s %d %s",gettext("Мат.-ответственный"),skl,nai);

if(mt == 2)
 sprintf(bros,"%s",nai);

memset(str,'\0',sizeof(str));
memset(str,'-',93);

fprintf(ff,"\
%-*s %s N%d\n%s\n",
iceb_u_kolbait(71,bros),bros,
gettext("Лист"),
sl,str);

fprintf(ff,gettext("\
Инвентарный|     Наименование             |          | Балансовая |   Износ    | Остаточная |\n\
  номер    |                              |  Номер   |стоимость на|    на      |стоимость на|\n\
	   |                              |          |%02d.%02d.%dг.|%02d.%02d.%dг.|%02d.%02d.%dг.|\n"),
dk,mk,gk,dk,mk,gk,dk,mk,gk);

fprintf(ff,"%s\n",str);

 
*kst+=KOLSTSH;

}

/*******/
/*Счетчик строк*/
/*******/
void uosvos_gsap(short dk,short mk,short gk,
int *sl,  //Счетчик листов
int *kst, //Счетчик строк
int skl,const char *nai,
short mt,   //0-по подразделению 1-по материально-ответств
FILE *ff)
{

*kst=*kst+1;

if(*kst <= kol_strok_na_liste)
 return;

*sl+=1;
*kst=1;
fprintf(ff,"\f");

uosvos_hhap(dk,mk,gk,*sl,kst,skl,nai,mt,ff);


}
/****************/
/*Распечатка одной группы налогового учёта для которой амартизация считается в целом по группе*/
/******************************************/

void uosvos_porgup(char *kodgr,
short dk,short mk,short gk,
double *suma_pogr,
FILE *ff,FILE *ffhpz,
GtkWidget *wpredok)
{
char naigrup[512];
SQL_str row;
SQLCURSOR cur;
char strsql[512];

memset(naigrup,'\0',sizeof(naigrup));

sprintf(strsql,"select naik from Uosgrup where kod='%s'",kodgr);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 strncpy(naigrup,row[0],sizeof(naigrup)-1);
  
class bsiz_data data;
bsiz23pw(kodgr,dk,mk,gk,dk,mk,gk,&data,wpredok);

fprintf(ff,"\
%11s %-*.*s %-10.10s %12.2f %12.2f %12.2f\n",
kodgr,
iceb_u_kolbait(30,naigrup),iceb_u_kolbait(30,naigrup),naigrup,
"",

data.nu.bs_nah_per,
data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per,
data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per));




fprintf(ffhpz,"\
%11s %-*.*s %-10.10s %12.2f %12.2f %12.2f\n",
kodgr,
iceb_u_kolbait(30,naigrup),iceb_u_kolbait(30,naigrup),naigrup,
"",

data.nu.bs_nah_per,
data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per,
data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per));


suma_pogr[0]+=data.nu.bs_nah_per;
suma_pogr[1]+=data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per;
suma_pogr[2]+=data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per);

}

/********************************/
/*Распечатка в целом по группе по списку групп*/
/*******************************/

void uosvos_pogurp_s(short dk,short mk,short gk,
short sli,
int *kst,
const char *hnaz,
double *suma_pogr,
FILE *ff,FILE *ffhpz,
GtkWidget *wpredok)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select kod from Uosgrup where ta=1");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;
fprintf(ff,"\n\n");
uosvos_hhap(dk,mk,gk,sli,kst,0,gettext("Группы с амортизацией в целом по группе"),2,ff);

fprintf(ffhpz,"\n\n");
uosvos_hhap(dk,mk,gk,sli,kst,0,gettext("Группы с амортизацией в целом по группе"),2,ffhpz);
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(hnaz,row[0],0,0) != 0)
   continue;

  uosvos_porgup(row[0],dk,mk,gk,suma_pogr,ff,ffhpz,wpredok);
 }

char str[1024];
memset(str,'\0',sizeof(str));
memset(str,'-',93);

fprintf(ff,"%s\n",str);
fprintf(ff,"\
%*s %12.2f %12.2f %12.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
suma_pogr[0],
suma_pogr[1],
suma_pogr[2]);

fprintf(ffhpz,"%s\n",str);
fprintf(ffhpz,"\
%*s %12.2f %12.2f %12.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
suma_pogr[0],
suma_pogr[1],
suma_pogr[2]);
 


}
/**********************************/
/*Начало отчётов*/
/***********************************/

void uosvosii_start(short dk,short mk,short gk,
int viduh,
int metr,
int metka_sort,
int *kst,
const char *podrz,
const char *hnaz,
const char *hnabyz,
const char *kodolz,
const char *hauz,
const char *shetupoi,
const char *innomp,
const char *hztz,
int sosob,
int metkaost,
FILE *ff)
{

*kst=5;

iceb_u_zagolov(gettext("Ведомость остаточной стоимости основных средств"),0,0,0,dk,mk,gk,organ,ff);

*kst+=1;

if(viduh == 0)
 {
  fprintf(ff,"%s\n",gettext("Налоговый учет"));
 }
if(viduh == 1)
 {
  fprintf(ff,"%s\n",gettext("Бухгалтерский учет"));
 }

int klst=0;
if(podrz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),podrz);
  iceb_printcod(ff,"Uospod","kod","naik",0,podrz,&klst);
  *kst+=klst+1;
 }

if(hnaz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа налогового учета"),hnaz);
  iceb_printcod(ff,"Uosgrup","kod","naik",1,hnaz,&klst);
  *kst+=klst+1;
 }
if(hnabyz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учета"),hnabyz);
  iceb_printcod(ff,"Uosgrup1","kod","naik",1,hnabyz,&klst);
  *kst+=klst+1;
 }
if(kodolz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Мат.-ответственный"),kodolz);
  iceb_printcod(ff,"Uosol","kod","naik",1,kodolz,&klst);
  *kst+=klst+1;
 }
if(hauz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учета"),hauz);
  iceb_printcod(ff,"Uoshau","kod","naik",0,hauz,&klst);
  *kst+=klst+1;
 }
if(shetupoi[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счет"),shetupoi);
  *kst+=1;
 }

if(innomp[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),innomp);
  *kst+=1;
 }

if(hztz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр затрат"),hztz);
  *kst+=1;
 }

if(sosob != 0)
 {
  
  fprintf(ff,"%s:%d",gettext("Состояние объекта"),sosob);
  if(sosob == 1)
   fprintf(ff,"%s\n",gettext("Основные средства, амортизация для которых считается"));
  if(sosob == 2)
   fprintf(ff,"%s\n",gettext("Основные средства, амортизация для которых не считается"));
   
  *kst+=1;
 }

if(metkaost == 1)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с нулевою остаточной стоимостью"));
  *kst+=1;
 }
if(metkaost == 2)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с ненулевой остаточной стоимостью"));
  *kst+=1;
 }
*kst+=1;
if(metr == 0)
 fprintf(ff,"%s\n",gettext("По подразделениям"));
if(metr == 1)
 fprintf(ff,"%s\n",gettext("По материально-ответственным"));

*kst+=1;
if(metka_sort == 0)
 fprintf(ff,"%s\n",gettext("Сортировка по группам"));
if(metka_sort == 1)
 fprintf(ff,"%s\n",gettext("Сортировать по счетам учёта"));

}


/******************************************/
/******************************************/

gint uosvosw_r1(class uosvosw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;



short dk,mk,gk;

iceb_u_rsdat(&dk,&mk,&gk,data->rk->datak.ravno(),1);
 
int kolstr=0;
sprintf(strsql,"select innom,naim from Uosin");

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
char imaftmp[50];
sprintf(imaftmp,"uos%d.tmp",getpid());
FILE *ff;
if((ff = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char pole1[50];
int kolstr3=0;
float kolstr1=0;
class SQLCURSOR cur1;
int in=0;
int poddz=0,kodotl=0;
int pod=0,podz=0;
SQL_str row1;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
    continue;
    
  in=atoi(row[0]);

  if(poiinw(in,dk,mk,gk,&poddz,&kodotl,data->window) != 0) 
     continue; //Произошел расход или на эту дату небыло прихода

  if(iceb_u_proverka(data->rk->podr.ravno(),poddz,0,0) != 0)
      continue;

  if(iceb_u_proverka(data->rk->mat_ot.ravno(),kodotl,0,0) != 0)
      continue;

  class poiinpdw_data rekin;
  poiinpdw(in,mk,gk,&rekin,data->window);
  if(data->metka_r == 0) //Пропускаем все основные средства если для них амортизазия считается в целом по группе
   if(uosprovgrw(rekin.hna.ravno(),data->window) == 1)
    continue;

  if(data->metka_r == 0 ) //Налоговый учёт
   {
    if(rekin.soso == 0 || rekin.soso == 3) //амортизация считается
     if(data->rk->sost_ob == 2)
      continue;
    if(rekin.soso == 1 || rekin.soso == 2) //амортизация не считаестя
     if(data->rk->sost_ob == 1)
      continue;
    
   }
  if(data->metka_r == 1 ) //Бух.учёт
   {
    if(rekin.soso == 0 || rekin.soso == 2) //амортизация считается
     if(data->rk->sost_ob == 2)
      continue;
    if(rekin.soso == 1 || rekin.soso == 3) //амортизация не считаестя
     if(data->rk->sost_ob == 1)
      continue;
    
   }
  if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->hzt.ravno(),rekin.hzt.ravno(),0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->grupa_nu.ravno(),rekin.hna.ravno(),0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->grupa_bu.ravno(),rekin.hnaby.ravno(),0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->hau.ravno(),rekin.hau.ravno(),0,0) != 0)
    continue;

  kolstr3++;

  memset(pole1,'\0',sizeof(pole1));      
  if(data->rk->metka_mat_podr == 0) //По подразделениям
   {
      
      if(data->rk->metka_sort == 0)
       {
        if(data->metka_r == 0) //налоговый учёт
          strcpy(pole1,rekin.hna.ravno());
        if(data->metka_r == 1) //бухгалтерский учёт
          strcpy(pole1,rekin.hnaby.ravno());

        if(pole1[0] == '\0')
         strcpy(pole1,"???");
        fprintf(ff,"%s|%d|%s|%s|%d|%s|\n",pole1,poddz,row[0],row[1],kodotl,rekin.nomz.ravno());
       }
      if(data->rk->metka_sort == 1)
        fprintf(ff,"%s|%d|%s|%s|%d|%s|\n",rekin.shetu.ravno(),poddz,row[0],row[1],kodotl,rekin.nomz.ravno());
    
   }

  if(data->rk->metka_mat_podr == 1) //По материально-ответственным
   {

      if(data->rk->metka_sort == 0)
       {
        if(data->metka_r == 0) //Налоговый учет
          strcpy(pole1,rekin.hna.ravno());
        if(data->metka_r == 1) //Бухгалтерский учет
          strcpy(pole1,rekin.hnaby.ravno());

        if(pole1[0] == '\0')
         strcpy(pole1,"???");

        fprintf(ff,"%s|%d|%s|%s|%d|%s|\n",pole1,kodotl,row[0],row[1],poddz,rekin.nomz.ravno());
       }
      if(data->rk->metka_sort == 1)
        fprintf(ff,"%s|%d|%s|%s|%d|%s|\n",rekin.shetu.ravno(),kodotl,row[0],row[1],poddz,rekin.nomz.ravno());
   }


 }
fclose(ff);

//sprintf(strsql,"sort -o %s -t\\| +0 -1 +1 -2 %s",imaftmp,imaftmp);
sprintf(strsql,"sort -o %s -t\\| -k1,2 %s",imaftmp,imaftmp);
system(strsql);
FILE *ff1;
if((ff1 = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf[56 ];
sprintf(imaf,"vos%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafhpz[56];
sprintf(imafhpz,"vosi%d.lst",getpid());
FILE *ffhpz;
if((ffhpz = fopen(imafhpz,"w")) == NULL)
 {
  iceb_er_op_fil(imafhpz,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


char imaf_br[56];

sprintf(imaf_br,"vosbr%d.lst",getpid());
FILE *ff_br;

if((ff_br = fopen(imaf_br,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_br,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int sli=0,kst=0;

uosvosii_start(dk,mk,gk,data->metka_r,data->rk->metka_mat_podr,data->rk->metka_sort,&kst,data->rk->podr.ravno(),
data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->mat_ot.ravno(),data->rk->hau.ravno(),data->rk->shetu.ravno(),data->rk->innom.ravno(),data->rk->hzt.ravno(),data->rk->sost_ob,data->rk->metka_ost,ff);

uosvosii_start(dk,mk,gk,data->metka_r,data->rk->metka_mat_podr,data->rk->metka_sort,&kst,data->rk->podr.ravno(),
data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->mat_ot.ravno(),data->rk->hau.ravno(),data->rk->shetu.ravno(),data->rk->innom.ravno(),data->rk->hzt.ravno(),data->rk->sost_ob,data->rk->metka_ost,ffhpz);


int kolstr_br=0;
int kol_list_br=1;

uosvosii_start(dk,mk,gk,data->metka_r,data->rk->metka_mat_podr,data->rk->metka_sort,&kolstr_br,data->rk->podr.ravno(),
data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->mat_ot.ravno(),data->rk->hau.ravno(),data->rk->shetu.ravno(),data->rk->innom.ravno(),data->rk->hzt.ravno(),data->rk->sost_ob,data->rk->metka_ost,ff_br);

 
int kstt=0;
uosvos_hhap(dk,mk,gk,1,&kstt,0,"",2,ffhpz);

uosvos_hhap(dk,mk,gk,kol_list_br,&kolstr_br,0,"",2,ff_br);

double bs1=0.,bss1=0.;
double s1=0.,ss1=0.;
double bs11=0.,s11=0.;
int mvsi=0,mvsh=0;
in=0;
sli=1;
podz=0;
kolstr1=0;
char hna1[56];
char hna[56];
char naipod[512];
memset(hna1,'\0',sizeof(hna1));
memset(naipod,'\0',sizeof(naipod));
memset(hna,'\0',sizeof(hna));
double iznos_per[2];
double i_iznos_per[2];
double iznos_per_pod[2];
double iznos_per_hna[2];
memset(&i_iznos_per,'\0',sizeof(i_iznos_per));
memset(&iznos_per,'\0',sizeof(iznos_per));
memset(&iznos_per_pod,'\0',sizeof(iznos_per_pod));
memset(&iznos_per_hna,'\0',sizeof(iznos_per_hna));

class bsiz_per data_podr; //Итоги по подразделению
class bsiz_per data_hz;   //Итоги по шифру затрат
class bsiz_per data_oi;   //Общий итог по всему отчёту
class bsiz_per data_shet; //Итог по счёту
char nomz[112];
char strr[1024];
char bros[512];
int podrp=0;
char naimos[512];
class SQLCURSOR curr;
char naigrup[512];
int kolos=0;
while(fgets(strr,sizeof(strr),ff1) != NULL)
 {

  iceb_pbar(data->bar,kolstr3,++kolstr1);    

  iceb_u_polen(strr,hna,sizeof(hna),1,'|');
  iceb_u_polen(strr,bros,sizeof(bros),2,'|');
  podrp=pod=atoi(bros);
  iceb_u_polen(strr,bros,sizeof(bros),3,'|');
  in=atol(bros);
  iceb_u_polen(strr,naimos,sizeof(naimos),4,'|');
  if(data->rk->metka_mat_podr == 1) /*По мат-отв*/
   {
    iceb_u_polen(strr,bros,sizeof(bros),5,'|');
    podrp=atoi(bros);
   }

  iceb_u_polen(strr,nomz,sizeof(nomz),6,'|');

  memset(&iznos_per,'\0',sizeof(iznos_per));

  
  class bsiz_data data_per;
  
  bsizpw(in,podrp,dk,mk,gk,dk,mk,gk,&data_per,data->window);
  
  double pr1=0.,pr2=0.;  
  if(data->metka_r == 0)
   {
    pr1=data_per.nu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.nu.izmen_iznosa_nah_per+data_per.nu.amort_nah_per;  //Износ на начало периода
      
   }
  
  if(data->metka_r == 1)
   {
    pr1=data_per.bu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.bu.izmen_iznosa_nah_per+data_per.bu.amort_nah_per;  //Износ на начало периода
    
   }
    
  
  
  
  if(fabs(pr1) < 0.01 ) //Значит уже продан или списан или передан в другое подразделение
   {
//    printw("in=%d %d.%d.%d %d.%d.%d\n",in,dn,mn,gn,dp,mp,gp);
//    printw("pr1=%.2f pr2=%.2f pr11=%.2f pr22=%.2f\n",
//    pr1,pr2,pr11,pr22);
//    OSTANOV();
    continue;
   }


  if(podz != pod)
   {
    if(podz != 0)
     {
      sprintf(bros,"%d",podz);
      uosvos_ithz(bros,bss1,ss1,0,&kst,iznos_per_pod,&data_podr,ff,ffhpz);
      memset(&iznos_per_pod,'\0',sizeof(iznos_per_pod));
      bss1=0.;
      ss1=0.;
     }

    if(data->rk->metka_mat_podr == 0)
     {
      /*Читаем наименование подразделения*/
      memset(naipod,'\0',sizeof(naipod));
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),pod);
        iceb_menu_soob(strsql,data->window);
        fprintf(ff,"%s %d !\n",gettext("Не найден код подразделения"),pod);
       }
      else
        strncpy(naipod,row1[0],sizeof(naipod)-1);
     }

    if(data->rk->metka_mat_podr == 1)
     {
      /*Читаем мат.-ответственного*/
      memset(naipod,'\0',sizeof(naipod));
      sprintf(strsql,"select naik from Uosol where kod=%d",pod);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        sprintf(strsql,"%s %d !",gettext("Не найден мат.-ответственный"),pod);
        iceb_menu_soob(strsql,data->window);
        
        fprintf(ff,"%s %d !\n",gettext("Не найден мат.-ответственный"),pod);
       }
      else
        strncpy(naipod,row1[0],sizeof(naipod)-1);
     }

    mvsh++;
             
    podz=pod;
    if(data->rk->metka_mat_podr == 0)
     sprintf(strsql,"\n%s - %d/%s\n",gettext("Подразделение"),podz,naipod);
    if(data->rk->metka_mat_podr == 1)
     sprintf(strsql,"\n%s - %d/%s\n",gettext("Мат.-ответственный"),podz,naipod);

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
   }

  if(iceb_u_SRAV(hna1,hna,0) != 0)
   {
    if(hna1[0] != '\0')
     {
      if(data->rk->metka_sort == 0)
       uosvos_ithz(hna1,bs1,s1,1,&kst,iznos_per_hna,&data_hz,ff,ffhpz);
      if(data->rk->metka_sort == 1)
       uosvos_ithz(hna1,bs1,s1,3,&kst,iznos_per_hna,&data_shet,ff,ffhpz);
      memset(&iznos_per_hna,'\0',sizeof(iznos_per_hna));
      bs1=s1=0.;
     }
    
    if(data->rk->metka_sort == 0)
     {
      /*Уэнаем наименование шифра нормы аморт. отчисления*/
      memset(naigrup,'\0',sizeof(naigrup));
      if(iceb_u_SRAV(hna,"???",0) != 0)
       {
        if(data->metka_r == 0)
          sprintf(strsql,"select naik from Uosgrup where kod='%s'",hna);
        if(data->metka_r == 1)
          sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",hna);

        if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найдено группу"),hna);
          iceb_menu_soob(strsql,data->window);
          fprintf(ff,"%s %s !\n",gettext("Не найдено группу"),hna);
         }
        else
          strncpy(naigrup,row1[0],sizeof(naigrup)-1);
       }
      else
       strcpy(naigrup,"Не введена в карточку группа");
      sprintf(strsql,"\n%s %s \"%s\"\n",gettext("Группа"),hna,naigrup);

      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
     }
    if(data->rk->metka_sort == 1)
     {
      sprintf(strsql,"select nais from Plansh where ns='%s'",hna);
      memset(naigrup,'\0',sizeof(naigrup));
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
        strncpy(naigrup,row1[0],sizeof(naigrup)-1);
     
      sprintf(strsql,"\n%s %s \"%s\"\n",
      gettext("Счёт учёта"),hna,naigrup);

      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
     }
    mvsi++;


    strcpy(hna1,hna);

   }
  if(mvsh > 0 || mvsi > 0) 
   {  
    if(kst > kol_strok_na_liste-KOLSTSH-5)
     {
      fprintf(ff,"\f");
      sli++;
      kst=0;
     }
    if(mvsi > 0)
     {
      if(data->rk->metka_sort == 0)
       {
        fprintf(ff,"\n%s %s \"%s\"\n",gettext("Группа"),hna,naigrup);
       }
      if(data->rk->metka_sort == 1)
       {
        fprintf(ff,"\n%s %s \"%s\"\n",
        gettext("Счёт учёта"),hna,naigrup);
       }
      mvsi=0;
      kst+=2;
     } 
    uosvos_hhap(dk,mk,gk,sli,&kst,pod,naipod,data->rk->metka_mat_podr,ff);
    mvsh=0;
   }

  if(data->metka_r == 0)
   {
    data_podr.plus(&data_per.nu);
    data_oi.plus(&data_per.nu);
    data_hz.plus(&data_per.nu);
    data_shet.plus(&data_per.nu);
   }
  if(data->metka_r == 1)
   {
    data_podr.plus(&data_per.bu);
    data_oi.plus(&data_per.bu);
    data_hz.plus(&data_per.bu);
    data_shet.plus(&data_per.bu);
   }
  double izn=pr2;
  double bsn=pr1;
  double osts=bsn-izn;

  if(osts >  0.009 && data->rk->metka_ost == 1)
   continue;

  if(osts ==  0. && data->rk->metka_ost == 2)
   continue;

  bss1+=bsn;
  ss1+=izn;
  
  bs1+=bsn;
  s1+=izn;
  
  bs11+=bsn;
  s11+=izn;


  if(fabs(bsn+izn+osts) > 0.009)
   {

    uosvos_gsap(dk,mk,gk,&sli,&kst,pod,naipod,data->rk->metka_mat_podr,ff);
    fprintf(ff,"%11d %-*.*s %-*.*s %12.2f %12.2f %12.2f\n",
    in,
    iceb_u_kolbait(30,naimos),iceb_u_kolbait(30,naimos),naimos,
    iceb_u_kolbait(10,nomz),iceb_u_kolbait(10,nomz),nomz,
    bsn,izn,osts);

    uosvos_gsap(dk,mk,gk,&kol_list_br,&kolstr_br,0,"",2,ff_br);
    fprintf(ff_br,"%11d %-*.*s %-*.*s %12.2f %12.2f %12.2f\n",
    in,
    iceb_u_kolbait(30,naimos),iceb_u_kolbait(30,naimos),naimos,
    iceb_u_kolbait(10,nomz),iceb_u_kolbait(10,nomz),nomz,
    bsn,izn,osts);

    if(iceb_u_strlen(naimos) > 30)
     {
    
      uosvos_gsap(dk,mk,gk,&sli,&kst,pod,naipod,data->rk->metka_mat_podr,ff);
      fprintf(ff,"%11s %s\n","",iceb_u_adrsimv(30,naimos));
  
      uosvos_gsap(dk,mk,gk,&kol_list_br,&kolstr_br,0,"",2,ff_br);
      fprintf(ff_br,"%11s %s\n","",iceb_u_adrsimv(30,naimos));
     }
    kolos++;
   }
 }


fclose(ff1);

unlink(imaftmp);


sprintf(bros,"%d",pod);
uosvos_ithz(bros,bss1,ss1,0,&kst,iznos_per_pod,&data_podr,ff,ffhpz);

if(data->rk->metka_sort == 0)
  uosvos_ithz(hna,bs1,s1,1,&kst,iznos_per_hna,&data_hz,ff,ffhpz);
if(data->rk->metka_sort == 1)
  uosvos_ithz(hna,bs1,s1,3,&kst,iznos_per_hna,&data_shet,ff,ffhpz);

/*Итог Общий*/
uosvos_ithz(" ",bs11,s11,2,&kst,i_iznos_per,&data_oi,ff,ffhpz);
uosvos_ithz(" ",bs11,s11,2,&kst,i_iznos_per,&data_oi,ff_br,NULL);


if(data->metka_r == 0) //распечатка по группам с общегрупповой амортизацией
 {
  double suma_pogr[3];
  memset(suma_pogr,'\0',sizeof(suma_pogr));

  uosvos_pogurp_s(dk,mk,gk,sli,&kst,data->rk->grupa_nu.ravno(),suma_pogr,ff,ffhpz,data->window);

  suma_pogr[0]+=bs11;
  suma_pogr[1]+=s11;
  suma_pogr[2]+=bs11-s11;
  
  char str[1024];
  memset(str,'\0',sizeof(str));
  memset(str,'-',93);

  fprintf(ff,"%s\n",str);
  fprintf(ff,"\
%*s %12.2f %12.2f %12.2f\n",
  iceb_u_kolbait(53,gettext("Общий итог")),gettext("Общий итог"),
  suma_pogr[0],
  suma_pogr[1],
  suma_pogr[2]);


  fprintf(ffhpz,"%s\n",str);
  fprintf(ffhpz,"\
%*s %12.2f %12.2f %12.2f\n",
  iceb_u_kolbait(53,gettext("Общий итог")),gettext("Общий итог"),
  suma_pogr[0],
  suma_pogr[1],
  suma_pogr[2]);

 }





sprintf(strsql,"\n%s:%d\n",gettext("Количество основных средств"),kolos);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ffhpz,"\n%s:%d\n",gettext("Количество основных средств"),kolos);

iceb_podpis(ff,data->window);
iceb_podpis(ffhpz,data->window);
iceb_podpis(ff_br,data->window);

fclose(ff);
fclose(ff_br);
fclose(ffhpz);


data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Ведомость остаточной стоимости c сортировкой"));
data->rk->imaf.plus(imaf_br);
data->rk->naim.plus(gettext("Ведомость остаточной стоимости без сортировки"));
data->rk->imaf.plus(imafhpz);
data->rk->naim.plus(gettext("Общий итог"));

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
