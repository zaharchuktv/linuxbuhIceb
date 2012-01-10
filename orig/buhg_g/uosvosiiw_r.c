/*$Id: uosvosiiw_r.c,v 1.18 2011-08-29 07:13:44 sasa Exp $*/
/*05.07.2011	07.01.2008	Белых А.И.	uosvosiiw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "uosvosiiw.h"
#define         KOLSTSH  6  /*Количество строк в шапке*/

class uosvosiiw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class uosvosiiw_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  int metka_r;  
  uosvosiiw_r_data()
   {
    metka_r=0;
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosvosiiw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvosiiw_r_data *data);
gint uosvosiiw_r1(class uosvosiiw_r_data *data);
void  uosvosiiw_r_v_knopka(GtkWidget *widget,class uosvosiiw_r_data *data);

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
extern int kol_strok_na_liste_l;
extern float    nemi; /*Необлагаемый минимум*/
extern char	*spgnu; //Список групп налогового учета
extern short    metkabo; //Если равна 1-бюджетная организация

int uosvosiiw_r(int metka_r,class uosvosiiw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class uosvosiiw_r_data data;
data.rk=datark;
data.metka_r=metka_r;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

if(metka_r == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Распечатать ведомость износа и остаточной стоимости (налоговый учёт)"));
if(metka_r == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Распечатать ведомость износа и остаточной стоимости (бух. учёт)"));

//sprintf(strsql,"%s %s",name_system,gettext("Распечатка ведомости аморт-отчислений для налогового учёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosvosiiw_r_key_press),&data);

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
  repl.plus(gettext("Распечатать ведомость износа и остаточной стоимости (налоговый учёт)"));
if(metka_r == 1)
  repl.plus(gettext("Распечатать ведомость износа и остаточной стоимости (бух. учёт)"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)uosvosiiw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosvosiiw_r_v_knopka(GtkWidget *widget,class uosvosiiw_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosvosiiw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvosiiw_r_data *data)
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
void            ithz(const char *sh,double k1,double k2,double k3,double s1,
double s2,double s3,
int mt,  //0-по подразделению 1- по шифру 2-общий итог 3-по счету учёта
int *kst,
double *iznos_per,
class bsiz_per *data_i,
FILE *ff,FILE *ffhpz,FILE *ff_f,FILE *ff_fi,FILE *ff_fis)
{
char		bros[512];
char		str[312];

memset(str,'\0',sizeof(str));
memset(str,'-',164);

if(mt == 2)
 {
  sprintf(bros,gettext("Общий итог"));
 }

if(mt == 1)
 {
  if(sh[0] == '\0')
   return;
  sprintf(bros,"%s %s",gettext("Итого по шифру"),sh);
fprintf(ffhpz,"%s\n\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f \
%15.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",
k1,s1,k1-s1,k2,s2,k3,s3,k3-s3);
 }

if(mt == 0)
 {
  if(k1+s1+(k1-s1)+k2+s2+k3+s3+(k3-s3) < 0.0001)
   return;
  sprintf(bros,"%s %s",gettext("Итого по подразделению"),sh);
 }

if(mt == 3)
 {
  if(k1+s1+(k1-s1)+k2+s2+k3+s3+(k3-s3) < 0.0001)
   return;
  sprintf(bros,"%s %s",gettext("Итого по счёту"),sh);
 }

fprintf(ff,"%s\n\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f \
%15.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",
k1,s1,k1-s1,k2,s2,k3,s3,k3-s3);

fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_f,"\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f\
%15.2f %12.2f %12.2f\n"," ",iceb_u_kolbait(30,bros),bros," ",
k1,s1,k1-s1,data_i->bs_prih_za_per,data_i->bs_rash_za_per,
iznos_per[0],iznos_per[1],k3,s3,k3-s3);

fprintf(ff_fi,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_fi,"\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f\
%15.2f %12.2f %12.2f\n"," ",iceb_u_kolbait(30,bros),bros," ",
k1,s1,k1-s1,data_i->bs_prih_za_per,data_i->bs_rash_za_per,
iznos_per[0],iznos_per[1],k3,s3,k3-s3);

if(mt != 0)
 {
  fprintf(ff_fis,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff_fis,"\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %10.2f \
%15.2f %12.2f %12.2f\n"," ",iceb_u_kolbait(30,bros),bros," ",
  k1,s1,k1-s1,data_i->bs_prih_za_per,data_i->bs_rash_za_per,
  iznos_per[0],iznos_per[1],data_i->bs_rash_za_per-iznos_per[1],k3,s3,k3-s3);
 }
 
if(mt == 2 || mt == 3)
  fprintf(ffhpz,"%s\n\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f \
%15.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",
k1,s1,k1-s1,k2,s2,k3,s3,k3-s3);

*kst=*kst+2;
/****************
printw("\n%s:\n\
%s %15.2f %15.2f %15.2f\n\
%s %15.2f %15.2f\n\
%s %15.2f %15.2f %15.2f\n",
bros,
gettext("Баланс. ст./Износ/Ост.ст.:"),
k1,s1,k1-s1,
gettext("Измен бал. ст./Износ     :"),
k2,s2,
gettext("Баланс. ст./Износ/Ост.ст.:"),
k3,s3,k3-s3);
************/
data_i->clear_data(); //обнуляем массив

}
/********/
/*Шапка*/
/*******/

void haphna(short dn,short mn,short gn,
short dk,short mk,short gk,
int sl, //Счетчик листов
int *kst,  //Счетчик строк
int skl,const char *nai,
int mt,  //0-по подразделению 1-по материально-ответств
FILE *ff,
FILE *ff_f,
FILE *ff_fi,FILE *ff_fis)
{
char str[1024];
char bros[312];

if(mt == 0)
 sprintf(bros,"%s %d %s",gettext("Подразделение"),skl,nai);

if(mt == 1)
 sprintf(bros,"%s %d %s",gettext("Мат.-ответственный"),skl,nai);

if(mt == 2)
 sprintf(bros,"%s",nai);

memset(str,'\0',sizeof(str));
memset(str,'-',164);

fprintf(ff,"\
%-*s %s N%d\n\%s\n",
iceb_u_kolbait(150,bros),bros,
gettext("Лист"),
sl,str);

fprintf(ff,gettext("\
Инвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Изменение  |    Износ   |   Балансова   |    Износ   | Остаточная |\n\
  номер    |                              |  Номер   | стоимость на  |    на      |стоимость на| балансовой |    за      | стоимость  на |    на      |стоимость на|\n\
	   |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  |   период   |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n"),
dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);

fprintf(ff,"%s\n",str);

if(ff_f != NULL)
 {
  fprintf(ff_f,"\
%-*s %s N%d\n",
  iceb_u_kolbait(150,bros),bros,
  gettext("Лист"),
  sl);

  fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff_f,"\
Инвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Приход     |  Расход    |Амортизац.|Изменение |   Балансовая  |   Износ    | Остаточная |\n");
  fprintf(ff_f,"\
  номер    |                              |  Номер   | стоимасть на  |    на      |стоимость на| балансовой | балансовой |   за     |износа за | стоимость  на |    на      |стоимость на|\n");
  fprintf(ff_f,"\
	   |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  | стоимости  | период   |  период  |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n",
  dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);


  fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
 }

if(ff_fi != NULL)
 {
  fprintf(ff_fi,"\
%-*s %s N%d\n",
  iceb_u_kolbait(150,bros),bros,
  gettext("Лист"),
  sl);

  fprintf(ff_fi,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff_fi,"\
Iнвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Приход     |  Расход    |Амортизац.|Изменение |   Балансовая  |   Износ    | Остаточная |\n");
  fprintf(ff_fi,"\
  номер    |                              |  Номер   | стоимость на  |    на      |стоимость на| балансовой | балансовой |   за     |износа за | стоимость  на |    на      |стоимость на|\n");
  fprintf(ff_fi,"\
	   |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  | стоимости  | период   |  период  |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n",
  dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);


  fprintf(ff_fi,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
 }

if(ff_fis != NULL)
 {
  fprintf(ff_fis,"\
%-*s %s N%d\n",
  iceb_u_kolbait(150,bros),bros,
  gettext("Лист"),
  sl);

  fprintf(ff_fis,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff_fis,"\
Инвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Приход     |  Расход    |Амортизац.|Изменение |Изменение |   Балансовая  |   Износ    | Остаточная |\n");
  fprintf(ff_fis,"\
  номер    |                              |  Номер   | стоимость на  |    на      |стоимость на| балансовой | балансовой |   за     |износа за |остаточной| стоимость  на |    на      |стоимость на|\n");
  fprintf(ff_fis,"\
	   |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  | стоимости  | период   |  период  |стоимости |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n",
  dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);


  fprintf(ff_fis,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
 }
 
*kst=*kst+KOLSTSH;

}

/*******/
/*Счетчик строк*/
/*******/
void gsapphna(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,  //Счетчик листов
int *kst, //Счетчик строк
int skl,char nai[],
short mt,   //0-по подразделению 1-по материально-ответств
FILE *ff,FILE *ff_f)
{

*kst=*kst+1;

if(*kst <= kol_strok_na_liste_l)
 return;

*sl=*sl+1;
*kst=1;
fprintf(ff,"\f");
fprintf(ff_f,"\f");

haphna(dn,mn,gn,dk,mk,gk,*sl,kst,skl,nai,mt,ff,ff_f,NULL,NULL);


}
/****************/
/*Распечатка одной группы налогового учёта для которой амартизация считается в целом по группе*/
/******************************************/

void porgup(char *kodgr,
short dn,short mn,short gn,
short dk,short mk,short gk,
double *suma_pogr,
double *iznos,
FILE *ff,FILE *ffhpz,FILE *ff_f,FILE *ff_fi,FILE *ff_fis,
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
bsiz23pw(kodgr,dn,mn,gn,dk,mk,gk,&data,wpredok);

fprintf(ff,"\
%11s %-*.*s %-10.10s %15.2f %12.2f %12.2f %12.2f %12.2f %15.2f %12.2f %12.2f\n",
kodgr,
iceb_u_kolbait(30,naigrup),iceb_u_kolbait(30,naigrup),naigrup,
"",

data.nu.bs_nah_per,
data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per,
data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per),

data.nu.bs_kon_per-data.nu.bs_nah_per,
data.nu.izmen_iznosa_raznica+data.nu.amort_raznica,

data.nu.bs_kon_per,
data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per,
data.nu.bs_kon_per-(data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per));


fprintf(ff_f,"\
%11s %-*.*s %-10.10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
kodgr,
iceb_u_kolbait(30,naigrup),iceb_u_kolbait(30,naigrup),naigrup,
"",

data.nu.bs_nah_per,
data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per,
data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per),
data.nu.bs_prih_za_per,
data.nu.bs_rash_za_per,
data.nu.amort_raznica,
data.nu.izmen_iznosa_raznica,

data.nu.bs_kon_per,
data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per,
data.nu.bs_kon_per-(data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per));

fprintf(ff_fi,"\
%11s %-*.*s %-10.10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
kodgr,
iceb_u_kolbait(30,naigrup),iceb_u_kolbait(30,naigrup),naigrup,
"",

data.nu.bs_nah_per,
data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per,
data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per),
data.nu.bs_prih_za_per,
data.nu.bs_rash_za_per,
data.nu.amort_raznica,
data.nu.izmen_iznosa_raznica,

data.nu.bs_kon_per,
data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per,
data.nu.bs_kon_per-(data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per));

fprintf(ff_fis,"\
%11s %-*.*s %-10.10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
kodgr,
iceb_u_kolbait(30,naigrup),iceb_u_kolbait(30,naigrup),naigrup,
"",

data.nu.bs_nah_per,
data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per,
data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per),
data.nu.bs_prih_za_per,
data.nu.bs_rash_za_per,
data.nu.amort_raznica,
data.nu.izmen_iznosa_raznica,
data.nu.bs_rash_za_per-data.nu.izmen_iznosa_raznica,

data.nu.bs_kon_per,
data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per,
data.nu.bs_kon_per-(data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per));


fprintf(ffhpz,"\
%11s %-*.*s %-10.10s %15.2f %12.2f %12.2f %12.2f %12.2f %15.2f %12.2f %12.2f\n",
kodgr,
iceb_u_kolbait(30,naigrup),iceb_u_kolbait(30,naigrup),naigrup,
"",

data.nu.bs_nah_per,
data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per,
data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per),

data.nu.bs_kon_per-data.nu.bs_nah_per,
data.nu.izmen_iznosa_raznica+data.nu.amort_raznica,

data.nu.bs_kon_per,
data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per,
data.nu.bs_kon_per-(data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per));

suma_pogr[0]+=data.nu.bs_nah_per;
suma_pogr[1]+=data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per;
suma_pogr[2]+=data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per);
//suma_pogr[3]+=data.nu.bs_kon_per-data.nu.bs_nah_per;
suma_pogr[3]+=data.nu.bs_prih_za_per;
suma_pogr[4]+=data.nu.bs_rash_za_per;

suma_pogr[5]+=data.nu.izmen_iznosa_raznica+data.nu.amort_raznica;
suma_pogr[6]+=data.nu.bs_kon_per;
suma_pogr[7]+=data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per;
suma_pogr[8]+=data.nu.bs_kon_per-(data.nu.izmen_iznosa_kon_per+data.nu.amort_kon_per);

iznos[0]+=data.nu.amort_raznica;
iznos[1]+=data.nu.izmen_iznosa_raznica;

}

/********************************/
/*Распечатка в целом по группе по списку групп*/
/*******************************/

void pogrup_s(short dn,short mn,short gn,
short dk,short mk,short gk,
int sli,
int *kst,
const char *hnaz,
double *suma_pogr,
double *iznos,
FILE *ff,FILE *ffhpz,FILE *ff_f,FILE *ff_fi,FILE *ff_fis,
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
haphna(dn,mn,gn,dk,mk,gk,sli,kst,0,"Группы с амортизацией в целом по группе",2,ff,ff_f,NULL,NULL);

fprintf(ffhpz,"\n\n");
haphna(dn,mn,gn,dk,mk,gk,sli,kst,0,"Группы с амортизацией в целом по группе",2,ffhpz,NULL,NULL,NULL);
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(hnaz,row[0],0,0) != 0)
   continue;

  porgup(row[0],dn,mn,gn,dk,mk,gk,suma_pogr,iznos,ff,ffhpz,ff_f,ff_fi,ff_fis,wpredok);
 }

char str[1024];
memset(str,'\0',sizeof(str));
memset(str,'-',164);

fprintf(ff,"%s\n",str);
fprintf(ff,"\
%*s %15.2f %12.2f %12.2f %12.2f %12.2f %15.2f %12.2f %12.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
suma_pogr[0],
suma_pogr[1],
suma_pogr[2],
suma_pogr[3]+suma_pogr[4],
suma_pogr[5],
suma_pogr[6],
suma_pogr[7],
suma_pogr[8]);

fprintf(ff_f,"\
%*s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
suma_pogr[0],
suma_pogr[1],
suma_pogr[2],
suma_pogr[3],
suma_pogr[4],
iznos[0],
iznos[1],
suma_pogr[6],
suma_pogr[7],
suma_pogr[8]);

fprintf(ff_fi,"\
%*s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
suma_pogr[0],
suma_pogr[1],
suma_pogr[2],
suma_pogr[3],
suma_pogr[4],
iznos[0],
iznos[1],
suma_pogr[6],
suma_pogr[7],
suma_pogr[8]);

fprintf(ff_fis,"\
%*s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
suma_pogr[0],
suma_pogr[1],
suma_pogr[2],
suma_pogr[3],
suma_pogr[4],
iznos[0],
iznos[1],
suma_pogr[4]-iznos[1],
suma_pogr[6],
suma_pogr[7],
suma_pogr[8]);
 
fprintf(ffhpz,"%s\n",str);
fprintf(ffhpz,"\
%*s %15.2f %12.2f %12.2f %12.2f %12.2f %15.2f %12.2f %12.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
suma_pogr[0],
suma_pogr[1],
suma_pogr[2],
suma_pogr[3]+suma_pogr[4],
suma_pogr[5],
suma_pogr[6],
suma_pogr[7],
suma_pogr[8]);
 


}
/**********************************/
/*Начало отчётов*/
/***********************************/

void uosvosii_start(short dn,short mn,short gn,
short dk,short mk,short gk,
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
const char *kod_op,
int metkaost,
FILE *ff)
{

*kst=5;

iceb_u_zagolov(gettext("Ведомость износа и остаточной стоимости основных средств"),dn,mn,gn,dk,mk,gk,organ,ff);

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

if(kod_op[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Код операции"),kod_op);
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
/**********************************/
/*Проверка реквизитов*/
/**************************/
int uosvosiiw_r_rek(int in,
const char *naim,
short mn,short gn,
class uosvosiiw_r_data *data,int *kolstr3,
int poddz,
int kodotl,
FILE *ff)
{
class poiinpdw_data rekin;

poiinpdw(in,mn,gn,&rekin,data->window);

//if(data->metka_r == 0 && spgnunpa[0] != '\0') //Пропускаем все основные средства если для них амортизазия считается в целом по группе
// if(iceb_u_proverka(spgnunpa,rekin.hna.ravno(),0,0) == 0)
if(data->metka_r == 0) //Пропускаем все основные средства если для них амортизазия считается в целом по группе
 if(uosprovgrw(rekin.hna.ravno(),NULL) == 1)
  return(1);

if(data->metka_r == 0 ) //Налоговый учёт
 {
  if(rekin.soso == 0 || rekin.soso == 3) //амортизация считается
   if(data->rk->sost_ob == 2)
    return(1);
  if(rekin.soso == 1 || rekin.soso == 2) //амортизация не считаестя
   if(data->rk->sost_ob == 1)
    return(1);
  
 }
if(data->metka_r == 1 ) //Бух.учёт
 {
  if(rekin.soso == 0 || rekin.soso == 2) //амортизация считается
   if(data->rk->sost_ob == 2)
    return(1);
  if(rekin.soso == 1 || rekin.soso == 3) //амортизация не считаестя
   if(data->rk->sost_ob == 1)
    return(1);
  
 }
if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
  return(1);

if(iceb_u_proverka(data->rk->hzt.ravno(),rekin.hzt.ravno(),0,0) != 0)
  return(1);

if(iceb_u_proverka(data->rk->grupa_nu.ravno(),rekin.hna.ravno(),0,0) != 0)
  return(1);

if(iceb_u_proverka(data->rk->grupa_bu.ravno(),rekin.hnaby.ravno(),0,0) != 0)
  return(1);

if(iceb_u_proverka(data->rk->hau.ravno(),rekin.hau.ravno(),0,0) != 0)
  return(1);

*kolstr3+=1;
char pole1[56];
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
      fprintf(ff,"%s|%d|%d|%s|%d|%s|\n",pole1,poddz,in,naim,kodotl,rekin.nomz.ravno());
     }
    if(data->rk->metka_sort == 1)
      fprintf(ff,"%s|%d|%d|%s|%d|%s|\n",rekin.shetu.ravno(),poddz,in,naim,kodotl,rekin.nomz.ravno());
  
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

      fprintf(ff,"%s|%d|%d|%s|%d|%s|\n",pole1,kodotl,in,naim,poddz,rekin.nomz.ravno());
     }
    if(data->rk->metka_sort == 1)
      fprintf(ff,"%s|%d|%d|%s|%d|%s|\n",rekin.shetu.ravno(),kodotl,in,naim,poddz,rekin.nomz.ravno());
 }
return(0);
}
/**************************************/
/*                                  */
/***********************************/
int uosvosiiw_r_poi(int in,
const char *naim,
short dn,short mn,short gn,
short dk,short mk,short gk,
class uosvosiiw_r_data *data,int *kolstr3,FILE *ff)
{
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
char strsql[512];
int poddz=0;
int kodotl=0;

if(poiinw(in,dn,mn,gn,&poddz,&kodotl,data->window) == 0) 
 {
  if(iceb_u_proverka(data->rk->podr.ravno(),poddz,0,0) == 0)
   if(iceb_u_proverka(data->rk->mat_ot.ravno(),kodotl,0,0) == 0)
     uosvosiiw_r_rek(in,naim,mn,gn,data,kolstr3,poddz,kodotl,ff);
 }

/*просмотриваем период -может быть куча внутренних перемещений*/

sprintf(strsql,"select datd,podr,nomd from Uosdok1 where \
datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' and innom=%d and podt=1 \
order by datd asc, tipz desc",gn,mn,dn,gk,mk,dk,in);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rk->podr.ravno(),row[1],0,0) != 0)
   continue;
  poddz=atoi(row[1]);
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  /**************Читаем документ*/
  sprintf(strsql,"select kodol from Uosdok where datd='%s' and nomd='%s'",row[0],row[2]);

  //printw("%s\n",strsql);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"Не найден документ N%s от %s !",row[2],row[0]);
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  kodotl=atoi(row1[0]);
  if(iceb_u_proverka(data->rk->mat_ot.ravno(),row1[0],0,0) != 0)
   continue;

  uosvosiiw_r_rek(in,naim,m,g,data,kolstr3,poddz,kodotl,ff);
//  uosvosiiw_r_rek(in,naim,viduh,metr,metka_sort,m,g,data->rk->podr.ravno(),data->rk->mat_ot.ravno(),shetupoi,hztz,hnaz,hnabyz,hauz,sosob,kolstr3,poddz,kodotl,ff);
 }

/******************
if(poiinw(in,dn,mn,gn,&poddz,&kodotl,data->window) != 0) 
  return(2);
    
if(iceb_u_proverka(data->rk->podr.ravno(),poddz,0,0) != 0)
  return(1);

if(iceb_u_proverka(data->rk->mat_ot.ravno(),kodotl,0,0) != 0)
  return(1);

return(uosvosiiw_r_rek(in,naim,dn,mn,gn,poddz,kodotl,data,kolstr3,ff));
******************/
return(0);
}


/******************************************/
/******************************************/

gint uosvosiiw_r1(class uosvosiiw_r_data *data)
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




sprintf(strsql,"select innom,naim from Uosin");
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
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaftmp[54];
sprintf(imaftmp,"uos%d.tmp",getpid());
FILE *ff;
if((ff = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int kolstr3=0;
float kolstr1=0;
int in=0;
int pod=0,podz=0;
SQL_str row1;
class poiinpdw_data rekin;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
    continue;
    
  in=atoi(row[0]);

  uosvosiiw_r_poi(in,row[1],dn,mn,gn,dk,mk,gk,data,&kolstr3,ff);

  
 }
fclose(ff);

//sprintf(strsql,"sort -o %s -t\\| +0 -1 +1 -2 %s",imaftmp,imaftmp);
sprintf(strsql,"sort -o %s -t\\| -u -k1,3 %s",imaftmp,imaftmp);
system(strsql);
FILE *ff1;

if((ff1 = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf[56];
if(data->rk->metka_mat_podr == 0)
 sprintf(imaf,"iznosp%d.lst",getpid());
if(data->rk->metka_mat_podr == 1)
 sprintf(imaf,"iznosm%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_f[56];

if(data->rk->metka_mat_podr == 0)
 sprintf(imaf_f,"viiosp%d.lst",getpid());
if(data->rk->metka_mat_podr == 1)
 sprintf(imaf_f,"viiosm%d.lst",getpid());

FILE *ff_f=NULL;
if((ff_f = fopen(imaf_f,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_f,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


char imaf_fi[56];

if(data->rk->metka_mat_podr == 0)
 sprintf(imaf_fi,"viiospi%d.lst",getpid());
if(data->rk->metka_mat_podr == 1)
 sprintf(imaf_fi,"viiosmi%d.lst",getpid());

FILE *ff_fi=NULL;
if((ff_fi = fopen(imaf_fi,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_fi,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


//Отчёт без итогов по подразделению, только по шифрам затрат
char imaf_fis[56];

if(data->rk->metka_mat_podr == 0)
 sprintf(imaf_fis,"visn%d.lst",getpid());
if(data->rk->metka_mat_podr == 1)
 sprintf(imaf_fis,"visb%d.lst",getpid());

FILE *ff_fis=NULL;
if((ff_fis = fopen(imaf_fis,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_fis,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


char imafhpz[56];

if(data->rk->metka_mat_podr == 0)
 sprintf(imafhpz,"iznospi%d.lst",getpid());
if(data->rk->metka_mat_podr == 1)
 sprintf(imafhpz,"iznosmi%d.lst",getpid());
FILE *ffhpz;
if((ffhpz = fopen(imafhpz,"w")) == NULL)
 {
  iceb_er_op_fil(imafhpz,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kst=0;


uosvosii_start(dn,mn,gn,dk,mk,gk,data->metka_r,data->rk->metka_mat_podr,data->rk->metka_sort,&kst,data->rk->podr.ravno(),
data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->mat_ot.ravno(),data->rk->hau.ravno(),data->rk->shetu.ravno(),
data->rk->innom.ravno(),data->rk->hzt.ravno(),data->rk->sost_ob,data->rk->kod_op.ravno(),data->rk->metka_ost,ff);

uosvosii_start(dn,mn,gn,dk,mk,gk,data->metka_r,data->rk->metka_mat_podr,data->rk->metka_sort,&kst,data->rk->podr.ravno(),
data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->mat_ot.ravno(),data->rk->hau.ravno(),data->rk->shetu.ravno(),data->rk->innom.ravno(),data->rk->hzt.ravno(),data->rk->sost_ob,data->rk->kod_op.ravno(),data->rk->metka_ost,ffhpz);

uosvosii_start(dn,mn,gn,dk,mk,gk,data->metka_r,data->rk->metka_mat_podr,data->rk->metka_sort,&kst,data->rk->podr.ravno(),
data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->mat_ot.ravno(),data->rk->hau.ravno(),data->rk->shetu.ravno(),data->rk->innom.ravno(),data->rk->hzt.ravno(),data->rk->sost_ob,data->rk->kod_op.ravno(),data->rk->metka_ost,ff_f);

uosvosii_start(dn,mn,gn,dk,mk,gk,data->metka_r,data->rk->metka_mat_podr,data->rk->metka_sort,&kst,data->rk->podr.ravno(),
data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->mat_ot.ravno(),data->rk->hau.ravno(),data->rk->shetu.ravno(),data->rk->innom.ravno(),data->rk->hzt.ravno(),data->rk->sost_ob,data->rk->kod_op.ravno(),data->rk->metka_ost,ff_fi);

uosvosii_start(dn,mn,gn,dk,mk,gk,data->metka_r,data->rk->metka_mat_podr,data->rk->metka_sort,&kst,data->rk->podr.ravno(),
data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->mat_ot.ravno(),data->rk->hau.ravno(),data->rk->shetu.ravno(),data->rk->innom.ravno(),data->rk->hzt.ravno(),data->rk->sost_ob,data->rk->kod_op.ravno(),data->rk->metka_ost,ff_fis);

 
int kstt=0;
haphna(dn,mn,gn,dk,mk,gk,1,&kstt,0,"",data->rk->metka_mat_podr,ffhpz,NULL,ff_fi,ff_fis);

int mvsi=0,mvsh=0;
int sli=1;
podz=0;
kolstr1=0.;
char hna1[24],hna[24];
memset(hna1,'\0',sizeof(hna1));
char naipod[112];
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
int kolos=0;
double bss1=0.,bss2=0.,bss3=0.,ss1=0.,ss2=0.,ss3=0.;
double bs1=0.,bs2=0.,bs3=0.,s1=0.,s2=0.,s3=0.;
double bs11=0.,bs21=0.,bs31=0.,s11=0.,s21=0.,s31=0.;
char strr[1024];
int podrp=0;
char naimos[200];
char nomz[112];
class SQLCURSOR curr;
char naigrup[512];
while(fgets(strr,sizeof(strr),ff1) != NULL)
 {

  iceb_pbar(data->bar,kolstr3,++kolstr1);    

  iceb_u_polen(strr,hna,sizeof(hna),1,'|');
  iceb_u_polen(strr,strsql,sizeof(strsql),2,'|');
  podrp=pod=atoi(strsql);

  iceb_u_polen(strr,strsql,sizeof(strsql),3,'|');
  in=atol(strsql);
  iceb_u_polen(strr,naimos,sizeof(naimos),4,'|');
  if(data->rk->metka_mat_podr == 1) /*По мат-отв*/
   {
    iceb_u_polen(strr,&podrp,5,'|');
   }

  iceb_u_polen(strr,nomz,sizeof(nomz),6,'|');

  memset(&iznos_per,'\0',sizeof(iznos_per));

  double bs_prih_za_per=0.;
  double bs_rash_za_per=0.;
  
  class bsiz_data data_per;
  
  bsizpw(in,podrp,dn,mn,gn,dk,mk,gk,&data_per,data->window);
  double pr1=0.,pr2=0.,pr11=0.,pr22=0.;  
  
  if(data->metka_r == 0)
   {
    pr1=data_per.nu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.nu.izmen_iznosa_nah_per+data_per.nu.amort_nah_per;  //Износ на начало периода
      
    pr11=data_per.nu.bs_kon_per; //Балансовая стоимость на конец периода
    
    pr22=data_per.nu.izmen_iznosa_kon_per+data_per.nu.amort_kon_per;  //Износ на конец периода
    
    iznos_per[0]+=data_per.nu.amort_raznica;     //Амортизация за период
    iznos_per[1]+=data_per.nu.izmen_iznosa_raznica; //Изменение износа за период
    bs_prih_za_per=data_per.nu.bs_prih_za_per;
    bs_rash_za_per=data_per.nu.bs_rash_za_per;
   }
  
  if(data->metka_r == 1)
   {
    pr1=data_per.bu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.bu.izmen_iznosa_nah_per+data_per.bu.amort_nah_per;  //Износ на начало периода
    
    pr11=data_per.bu.bs_kon_per; //Балансовая стоимость на конец периода
    
    pr22=data_per.bu.izmen_iznosa_kon_per+data_per.bu.amort_kon_per;  //Износ на конец периода
    
    iznos_per[0]+=data_per.bu.amort_raznica;     //Амортизация за период
    iznos_per[1]+=data_per.bu.izmen_iznosa_raznica; //Изменение износа за период
    bs_prih_za_per=data_per.bu.bs_prih_za_per;
    bs_rash_za_per=data_per.bu.bs_rash_za_per;

   }
    
  
  
  if(fabs(pr1) < 0.01 && fabs(pr11) < 0.01) //Значит уже продан или списан или передан в другое подразделение
   {
    continue;
   }

  if(data->rk->metka_mat_podr == 0 && data->rk->mat_ot.ravno()[0] != '\0') /*По подразделениям*/
   {
    //Может быть движение внутри подразделения по разным материально-ответственным лицам
    //в этом случае балансовая стоимость не равна нулю
    //Проверяем числился ли у этого мат. ответственного
    int pp=0,kk=0;
    if(poiinw(in,dk,mk,gk,&pp,&kk,data->window) != 0)
      continue;
    sprintf(strsql,"%d",kk);
    if(iceb_u_proverka(data->rk->mat_ot.ravno(),strsql,0,0) != 0)
      continue;
   }
  
  if(data->rk->metka_mat_podr == 1) /*По мат. ответственным лицам*/
   {
    
    //Проверяем есть числился ли у этого мат. ответственного
    int pp=0,kk=0;
    int metkapr=0; //метка присутствия в периоде
     //Должно быть на начало или на конец периода
    if(poiinw(in,dn,mn,gn,&pp,&kk,data->window) == 0)
     {
      metkapr++;
      //На начало периода должен числиться у материальноответственного
      if(pod != kk)
       continue; 
     }

    if(poiinw(in,dk,mk,gk,&pp,&kk,data->window) == 0)
     {
      metkapr++;
     }
    if(metkapr == 0)
      continue;
      
   }



  if(podz != pod)
   {
    if(podz != 0)
     {
      sprintf(strsql,"%d",podz);
      ithz(strsql,bss1,bss2,bss3,ss1,ss2,ss3,0,&kst,iznos_per_pod,&data_podr,ff,ffhpz,ff_f,ff_fi,ff_fis);
      memset(&iznos_per_pod,'\0',sizeof(iznos_per_pod));
      bss1=bss2=bss3=0.;
      ss1=ss2=ss3=0.;
     }

    if(data->rk->metka_mat_podr == 0)
     {
      /*Читаем наименование подразделения*/
      memset(naipod,'\0',sizeof(naipod));
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        fprintf(ff,"%s %d !\n",gettext("Не найден код подразделения"),pod);
        sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),pod);
        iceb_menu_soob(strsql,data->window);
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
        fprintf(ff,"%s %d !\n",gettext("Не найден мат.-ответственный"),pod);
        sprintf(strsql,"%s %d !",gettext("Не найден мат.-ответственный"),pod);
        iceb_menu_soob(strsql,data->window);
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
       ithz(hna1,bs1,bs2,bs3,s1,s2,s3,1,&kst,iznos_per_hna,&data_hz,ff,ffhpz,ff_f,ff_fi,ff_fis);
      if(data->rk->metka_sort == 1)
       ithz(hna1,bs1,bs2,bs3,s1,s2,s3,3,&kst,iznos_per_hna,&data_shet,ff,ffhpz,ff_f,ff_fi,ff_fis);
      memset(&iznos_per_hna,'\0',sizeof(iznos_per_hna));
      bs1=bs2=bs3=s1=s2=s3=0.;
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

        if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
         {
          fprintf(ff,"%s %s !\n",gettext("Не найдено группу"),hna);
          sprintf(strsql,"%s %s !",gettext("Не найдено группу"),hna);
          iceb_menu_soob(strsql,data->window);
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
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
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
    if(kst > kol_strok_na_liste_l-KOLSTSH-5)
     {
      fprintf(ff,"\f");
      fprintf(ff_f,"\f");
      sli++;
      kst=0;
     }
    if(mvsi > 0)
     {
      if(data->rk->metka_sort == 0)
       {
        fprintf(ff,"\n%s %s \"%s\"\n",gettext("Группа"),hna,naigrup);
        fprintf(ff_f,"\n%s %s \"%s\"\n",gettext("Группа"),hna,naigrup);
       }
      if(data->rk->metka_sort == 1)
       {
        fprintf(ff,"\n%s %s \"%s\"\n",
        gettext("Счёт учёта"),hna,naigrup);
        fprintf(ff_f,"\n%s %s \"%s\"\n",
        gettext("Счёт учёта"),hna,naigrup);
       }
      mvsi=0;
      kst+=2;
     } 
    haphna(dn,mn,gn,dk,mk,gk,sli,&kst,pod,naipod,data->rk->metka_mat_podr,ff,ff_f,NULL,NULL);
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

  i_iznos_per[0]+=iznos_per[0];  
  i_iznos_per[1]+=iznos_per[1];  

  iznos_per_pod[0]+=iznos_per[0];  
  iznos_per_pod[1]+=iznos_per[1];  

  iznos_per_hna[0]+=iznos_per[0];  
  iznos_per_hna[1]+=iznos_per[1];  

  bss1+=bsn;
  bss2+=pr11-pr1;
  bss3+=bsn+(pr11-pr1);
  ss1+=izn;
  ss2+=pr22-pr2;
  ss3+=izn+pr22-pr2;

  bs1+=bsn;
  bs2+=pr11-pr1;
  bs3+=bsn+(pr11-pr1);
  s1+= izn;
  s2+= pr22-pr2;
  s3+= izn+pr22-pr2;

  bs11+=bsn;
  bs21+=pr11-pr1;
  bs31+=bsn+(pr11-pr1);
  s11+= izn;
  s21+= pr22-pr2;
  s31+= izn+pr22-pr2;


  double izpr=pr22-pr2;


  if(fabs(bsn+izn+osts+pr11-pr1+pr22-pr2+bsn+pr11-pr1+izn+pr22+pr2+(bsn+pr11-pr1)+(izn+pr22-pr2)) > 0.009)
   {
    gsapphna(dn,mn,gn,dk,mk,gk,&sli,&kst,pod,naipod,data->rk->metka_mat_podr,ff,ff_f);

    fprintf(ff,"\
%11d %-*.*s %-*.*s %15.2f %12.2f %12.2f %12.2f %12.2f %15.2f %12.2f %12.2f\n",
    in,
    iceb_u_kolbait(30,naimos),iceb_u_kolbait(30,naimos),naimos,
    iceb_u_kolbait(10,nomz),iceb_u_kolbait(10,nomz),nomz,
    bsn,izn,osts,pr11-pr1,izpr,bsn+pr11-pr1,izn+pr22-pr2,(bsn+pr11-pr1)-(izn+pr22-pr2));

    fprintf(ff_f,"\
%11d %-*.*s %-*.*s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
    in,
    iceb_u_kolbait(30,naimos),iceb_u_kolbait(30,naimos),naimos,
    iceb_u_kolbait(10,nomz),iceb_u_kolbait(10,nomz),nomz,
    bsn,izn,osts,
    bs_prih_za_per,
    bs_rash_za_per,
    iznos_per[0],
    iznos_per[1],
    bsn+pr11-pr1,
    izn+pr22-pr2,
    (bsn+pr11-pr1)-(izn+pr22-pr2));

    if(iceb_u_strlen(naimos) > 30)
     {
      gsapphna(dn,mn,gn,dk,mk,gk,&sli,&kst,pod,naipod,data->rk->metka_mat_podr,ff,ff_f);
      fprintf(ff,"%11s %s\n","",iceb_u_adrsimv(30,naimos));
      fprintf(ff_f,"%11s %s\n","",iceb_u_adrsimv(30,naimos));
     }
    kolos++;
   }
 }


fclose(ff1);

unlink(imaftmp);


sprintf(strsql,"%d",pod);
ithz(strsql,bss1,bss2,bss3,ss1,ss2,ss3,0,&kst,iznos_per_pod,&data_podr,ff,ffhpz,ff_f,ff_fi,ff_fis);

if(data->rk->metka_sort == 0)
  ithz(hna,bs1,bs2,bs3,s1,s2,s3,1,&kst,iznos_per_hna,&data_hz,ff,ffhpz,ff_f,ff_fi,ff_fis);
if(data->rk->metka_sort == 1)
  ithz(hna1,bs1,bs2,bs3,s1,s2,s3,3,&kst,iznos_per_hna,&data_shet,ff,ffhpz,ff_f,ff_fi,ff_fis);

/*Итог Общий*/
ithz(" ",bs11,bs21,bs31,s11,s21,s31,2,&kst,i_iznos_per,&data_oi,ff,ffhpz,ff_f,ff_fi,ff_fis);

if(iceb_u_sravmydat(dn,mn,gn,1,4,2011) < 0)
if(data->metka_r == 0) //распечатка по группам с общегрупповой амортизацией
 {
  double suma_pogr[9];
  double iznos[2];
  memset(suma_pogr,'\0',sizeof(suma_pogr));
  memset(iznos,'\0',sizeof(iznos));

  pogrup_s(dn,mn,gn,dk,mk,gk,sli,&kst,data->rk->grupa_nu.ravno(),suma_pogr,iznos,ff,ffhpz,ff_f,ff_fi,ff_fis,data->window);

  suma_pogr[0]+=bs11;
  suma_pogr[1]+=s11;
  suma_pogr[2]+=bs11-s11;
  //suma_pogr[3]+=bs21;
  suma_pogr[3]+=data_oi.bs_prih_za_per;
  suma_pogr[4]+=data_oi.bs_rash_za_per;
  
  suma_pogr[5]+=s21;
  suma_pogr[6]+=bs31;
  suma_pogr[7]+=s31;
  suma_pogr[8]+=bs31-s31;
  
  iznos[0]+=i_iznos_per[0];
  iznos[1]+=i_iznos_per[1];
  
  char str[1024];
  memset(str,'\0',sizeof(str));
  memset(str,'-',164);

  fprintf(ff,"%s\n",str);
  fprintf(ff,"\
%*s %15.2f %12.2f %12.2f %12.2f %12.2f %15.2f %12.2f %12.2f\n",
  iceb_u_kolbait(53,gettext("Общий итог")),gettext("Общий итог"),
  suma_pogr[0],
  suma_pogr[1],
  suma_pogr[2],
  suma_pogr[3]+suma_pogr[4],
  suma_pogr[5],
  suma_pogr[6],
  suma_pogr[7],
  suma_pogr[8]);

  fprintf(ff_f,"\
%*s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
  iceb_u_kolbait(53,gettext("Общий итог")),gettext("Общий итог"),
  suma_pogr[0],
  suma_pogr[1],
  suma_pogr[2],
  suma_pogr[3],
  suma_pogr[4],
  iznos[0],
  iznos[1],
  suma_pogr[6],
  suma_pogr[7],
  suma_pogr[8]);

  fprintf(ff_fi,"\
%*s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
  iceb_u_kolbait(53,gettext("Общий итог")),gettext("Общий итог"),
  suma_pogr[0],
  suma_pogr[1],
  suma_pogr[2],
  suma_pogr[3],
  suma_pogr[4],
  iznos[0],
  iznos[1],
  suma_pogr[6],
  suma_pogr[7],
  suma_pogr[8]);

  fprintf(ff_fis,"\
%*s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
  iceb_u_kolbait(53,gettext("Общий итог")),gettext("Общий итог"),
  suma_pogr[0],
  suma_pogr[1],
  suma_pogr[2],
  suma_pogr[3],
  suma_pogr[4],
  iznos[0],
  iznos[1],
  suma_pogr[4]-iznos[1],
  suma_pogr[6],
  suma_pogr[7],
  suma_pogr[8]);

  fprintf(ffhpz,"%s\n",str);
  fprintf(ffhpz,"\
%*s %15.2f %12.2f %12.2f %12.2f %12.2f %15.2f %12.2f %12.2f\n",
  iceb_u_kolbait(53,gettext("Общий итог")),gettext("Общий итог"),
  suma_pogr[0],
  suma_pogr[1],
  suma_pogr[2],
  suma_pogr[3]+suma_pogr[4],
  suma_pogr[5],
  suma_pogr[6],
  suma_pogr[7],
  suma_pogr[8]);

 }





sprintf(strsql,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ff_f,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ff_fi,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ff_fis,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ffhpz,"\n%s:%d\n",gettext("Количество основных средств"),kolos);

iceb_podpis(ff,data->window);
iceb_podpis(ff_f,data->window);
iceb_podpis(ff_fi,data->window);
iceb_podpis(ff_fis,data->window);
iceb_podpis(ffhpz,data->window);

fclose(ff);
fclose(ff_f);
fclose(ff_fi);
fclose(ff_fis);
fclose(ffhpz);



data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Ведомость износа и остаточной стоимости основных средств"));
data->rk->imaf.plus(imafhpz);
data->rk->naim.plus(gettext("Общий итог"));
data->rk->imaf.plus(imaf_f);
data->rk->naim.plus(gettext("Ведомость с расшифровкой изменения балансовой стоимости"));
data->rk->imaf.plus(imaf_fi);
data->rk->naim.plus(gettext("Общие итоги по ведомости с расшифровкой"));
data->rk->imaf.plus(imaf_fis);
data->rk->naim.plus(gettext("Общие итоги по ведомости с расшифровкой+остаточная бал.стоимость"));

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
