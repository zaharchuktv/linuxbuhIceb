/*$Id: zar_vedpz_r.c,v 1.11 2011-02-21 07:36:00 sasa Exp $*/
/*20.11.2009	08.12.2006	Белых А.И.	zar_vedpz_r.c
Распечатка ведомости производственных затрат
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_vedpz.h"

struct kk
 {
  char		fio[512];
  char		dolg[40];
  char          inn[20];
  int		podr;
  int		sovm;
  short		dn,mn,gn;
  short		dk,mk,gk;
 };

class zar_vedpz_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_vedpz_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_vedpz_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_vedpz_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_vedpz_r_data *data);
gint zar_vedpz_r1(class zar_vedpz_r_data *data);
void  zar_vedpz_r_v_knopka(GtkWidget *widget,class zar_vedpz_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_vedpz_r(class zar_vedpz_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_vedpz_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать карточки начислений/удержаний по работникам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_vedpz_r_key_press),&data);

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

repl.plus(gettext("Распечатать карточки начислений/удержаний по работникам"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_vedpz_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_vedpz_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_vedpz_r_v_knopka(GtkWidget *widget,class zar_vedpz_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_vedpz_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_vedpz_r_data *data)
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
/************************************/
/*Распечатка удержаний    */
/*************************************/

void veprz1r(short dn,short mn,short gn,
short dk,short mk,short gk,
struct tm *bf,
FILE *ff,
iceb_u_spisok *sp_shet_nah,
iceb_u_spisok *shet_nah,
class iceb_u_double *shet_nah_suma,const char *sheta)
{
SQL_str row;
SQLCURSOR curr;
char strsql[512];

char shet[32];
char shet1[24];
char knah[24];
char naimshet[512];
char naimnah[112];
double itogo=0.;
double itogos=0.;

int kol_par=shet_nah->kolih();
//printw("kol_par=%d\n",kol_par);
double suma=0.;

int nomerp=0;
int kol_shet_nah=sp_shet_nah->kolih();


fprintf(ff,"\
------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Счет  |     Наименование счета       |Код|  Наименование удержания      | Сумма    |\n"));

fprintf(ff,"\
------------------------------------------------------------------------------------\n");


for(int nomersh=0; nomersh < kol_shet_nah; nomersh++)
 {
  strncpy(shet,sp_shet_nah->ravno(nomersh),sizeof(shet)-1);

  if(iceb_u_proverka(sheta,shet,0,0) != 0)
   continue;

  nomerp=0;
  itogos=0.;
  for(int nomer=0; nomer < kol_par; nomer++)
   {
    
    iceb_u_polen(shet_nah->ravno(nomer),shet1,sizeof(shet1),1,'|');
//    printw("%s\n",shet1);
    if(iceb_u_SRAV(shet,shet1,0) != 0)
      continue;
    iceb_u_polen(shet_nah->ravno(nomer),knah,sizeof(knah),2,'|');

    //Читаем наименование начисления
    memset(naimnah,'\0',sizeof(naimnah));

    sprintf(strsql,"select naik from Uder where kod=%s",knah);

    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      strncpy(naimnah,row[0],sizeof(naimnah)-1);
    
    suma=shet_nah_suma->ravno(nomer);
    if(suma == 0.)
     continue;
    itogo+=suma;    
    itogos+=suma;    
    if(nomerp == 0)
     {
      //Читаем наименование счета
      memset(naimshet,'\0',sizeof(naimshet));
      sprintf(strsql,"select nais from Plansh where ns='%s'",shet1);                

      if(sql_readkey(&bd,strsql,&row,&curr) == 1)
       strncpy(naimshet,row[0],sizeof(naimshet)-1);

      fprintf(ff,"%-*s %-*.*s %3s %-*.*s %10.2f\n",
      iceb_u_kolbait(6,shet1),shet1,
      iceb_u_kolbait(30,naimshet),iceb_u_kolbait(30,naimshet),naimshet,
      knah,
      iceb_u_kolbait(30,naimnah),iceb_u_kolbait(30,naimnah),naimnah,
      suma);
     }
    else
     {
      fprintf(ff,"%6s %-30.30s %3s %-*.*s %10.2f\n",
      " "," ",knah,iceb_u_kolbait(30,naimnah),iceb_u_kolbait(30,naimnah),naimnah,suma);

     }
    nomerp++;
   }
  if(nomerp > 0)
   {
    fprintf(ff,"%*s%10.2f\n",iceb_u_kolbait(73,gettext("Итого")),gettext("Итого"),itogos);
    fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . \n");
   }
 }
fprintf(ff,"\
------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s%10.2f\n",iceb_u_kolbait(73,gettext("Итого")),gettext("Итого"),itogo);


}

/************************************/
/*Распечатка начислений    */
/*************************************/

void veprz1rn(short dn,short mn,short gn,
short dk,short mk,short gk,
struct tm *bf,
FILE *ff,
iceb_u_spisok *sp_shet_nah,
iceb_u_spisok *shet_nah,
class iceb_u_double *shet_nah_suma,
class iceb_u_double *shet_nah_suma_os,
class iceb_u_double *shet_nah_suma_do,
class iceb_u_double *shet_nah_suma_dr,
class iceb_u_double *shet_nah_suma_fz,
const char *sheta,
GtkWidget *wpredok)
{
SQL_str row;
SQLCURSOR curr;
char strsql[512];

char shet[32];
char shet1[20];
char knah[20];
char naimshet[512];
char naimnah[100];
double itogo=0.;
double itogo_os=0.;
double itogo_do=0.;
double itogo_dr=0.;
double itogo_fz=0.;
double itogos=0.;
double itogos_os=0.;
double itogos_do=0.;
double itogos_dr=0.;
double itogos_fz=0.;

int kol_par=shet_nah->kolih();
//printw("kol_par=%d\n",kol_par);
double suma=0.;
double suma_os=0.;
double suma_do=0.;
double suma_dr=0.;
double suma_fz=0.;

int nomerp=0;
int kol_shet_nah=sp_shet_nah->kolih();


fprintf(ff,"\
----------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Счет  |Наименование счета  |Код|Наи-ние начисления  | Сумма    |Основная|Доп-ная |Другая  |Фонд з/п|\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------\n");


for(int nomersh=0; nomersh < kol_shet_nah; nomersh++)
 {
  strncpy(shet,sp_shet_nah->ravno(nomersh),sizeof(shet)-1);

  if(iceb_u_proverka(sheta,shet,0,0) != 0)
   continue;

  nomerp=0;
  itogos=0.;
  itogos_os=0.;
  itogos_do=0.;
  itogos_dr=0.;
  itogos_fz=0.;
  for(int nomer=0; nomer < kol_par; nomer++)
   {
    
    iceb_u_polen(shet_nah->ravno(nomer),shet1,sizeof(shet1),1,'|');
//    printw("%s\n",shet1);
    if(iceb_u_SRAV(shet,shet1,0) != 0)
      continue;
    iceb_u_polen(shet_nah->ravno(nomer),knah,sizeof(knah),2,'|');

    //Читаем наименование начисления
    memset(naimnah,'\0',sizeof(naimnah));

    sprintf(strsql,"select naik from Nash where kod=%s",knah);


    if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
      strncpy(naimnah,row[0],sizeof(naimnah)-1);
    
    suma=shet_nah_suma->ravno(nomer);
    if(suma == 0.)
     continue;
    suma_os=shet_nah_suma_os->ravno(nomer);
    suma_do=shet_nah_suma_do->ravno(nomer);
    suma_dr=shet_nah_suma_dr->ravno(nomer);
    suma_fz=shet_nah_suma_fz->ravno(nomer);

    itogo+=suma;    
    itogo_os+=suma_os;    
    itogo_do+=suma_do;    
    itogo_dr+=suma_dr;    
    itogo_fz+=suma_fz;    

    itogos+=suma;    
    itogos_os+=suma_os;    
    itogos_do+=suma_do;    
    itogos_dr+=suma_dr;    
    itogos_fz+=suma_fz;    
    if(nomerp == 0)
     {
      //Читаем наименование счета
      memset(naimshet,'\0',sizeof(naimshet));
      sprintf(strsql,"select nais from Plansh where ns='%s'",shet1);                

      if(sql_readkey(&bd,strsql,&row,&curr) == 1)
       strncpy(naimshet,row[0],sizeof(naimshet)-1);

      fprintf(ff,"%-*s %-*.*s %3s %-*.*s %10.2f %8.2f %8.2f %8.2f %8.2f\n",
      iceb_u_kolbait(6,shet1),shet1,
      iceb_u_kolbait(20,naimshet),iceb_u_kolbait(20,naimshet),naimshet,
      knah,
      iceb_u_kolbait(20,naimnah),iceb_u_kolbait(20,naimnah),naimnah,
      suma,suma_os,suma_do,suma_dr,suma_fz);
     }
    else
     {
      fprintf(ff,"%6s %-20.20s %3s %-*.*s %10.2f %8.2f %8.2f %8.2f %8.2f\n",
      " "," ",knah,iceb_u_kolbait(20,naimnah),iceb_u_kolbait(20,naimnah),naimnah,suma,suma_os,suma_do,suma_dr,suma_fz);

     }
    nomerp++;
   }
  if(nomerp > 0)
   {
    fprintf(ff,"%*s%10.2f %8.2f %8.2f %8.2f %8.2f\n",
    iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
    itogos,
    itogos_os,
    itogos_do,
    itogos_dr,
    itogos_fz);

    fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . \n");
   }
 }
fprintf(ff,"\
----------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s%10.2f %8.2f %8.2f %8.2f %8.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
itogo,
itogo_os,
itogo_do,
itogo_dr,
itogo_fz);



}
/****************************/
/*Печать реквизитов поиска*/
/*****************************/
void vedprz1_sap(class zar_vedpz_rek *rk,FILE *ff)
{
if(rk->podr.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),rk->podr.ravno());

if(rk->kod_grup.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Группа"),rk->kod_grup.ravno());

if(rk->shet.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),rk->shet.ravno());

if(rk->metka_pr_ras == 0)
 fprintf(ff,"%s\n",gettext("Начисления и удержания"));
if(rk->metka_pr_ras == 1)
 fprintf(ff,"%s\n",gettext("Только начисления"));
if(rk->metka_pr_ras == 2)
 fprintf(ff,"%s\n",gettext("Только удержания"));

}

/******************************************/
/******************************************/

gint zar_vedpz_r1(class zar_vedpz_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short dn=1,mn,gn;
short dk=0,mk=0,gk=0;
iceb_u_rsdat1(&mn,&gn,data->rk->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->rk->datak.ravno());
if(dk == 0)
 {
  dk=1;
  mk=mn;
  gk=gn;
  iceb_u_dpm(&dk,&mk,&gk,5);
 }



char bros[512];
SQL_str   row,row1;
SQLCURSOR cur;
SQLCURSOR cur1;

char glavbuh[112];
memset(glavbuh,'\0',sizeof(glavbuh));

iceb_poldan("Табельный номер бухгалтера",bros,"zarnast.alx",data->window);

if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row,&cur1,data->window) != 1)
   {
    sprintf(strsql,"Не знайдено табельного номера %s глaвного бухгалтера!",bros);
    iceb_menu_soob(strsql,data->window);
   }
  else
   {
    strncpy(glavbuh,row[0],sizeof(glavbuh)-1);
   }
 }


//Определяем список счетов для начислений и удержаний
class iceb_u_spisok sp_shet_nah; //Список всех счетов начислений в порядке возростания
class iceb_u_spisok sp_shet_uder; //Список всех счетов удержаний в порядке возростания
class iceb_u_spisok shet_nah;     //Счет+начисление
class iceb_u_spisok shet_uder;    //Счет + удержание

class iceb_u_double shet_nah_suma; //Суммы для Счет+начисление
class iceb_u_double shet_nah_suma_os; //Суммы для Счет+начисление основная зарплата
class iceb_u_double shet_nah_suma_do; //Суммы для Счет+начисление дополнительная зарплата
class iceb_u_double shet_nah_suma_dr; //Суммы для Счет+начисление другая зарплата
class iceb_u_double shet_nah_suma_fz; //Суммы для Счет+начисление входящие в фонд зарплаты
class iceb_u_double shet_uder_suma; //Суммы для Счет+удержание

class iceb_u_double shet_nah_suma_bt; //Суммы для Счет+начисление бюджет
class iceb_u_double shet_nah_suma_os_bt; //Суммы для Счет+начисление основная зарплата бюджет
class iceb_u_double shet_nah_suma_do_bt; //Суммы для Счет+начисление дополнительная зарплата бюджет
class iceb_u_double shet_nah_suma_dr_bt; //Суммы для Счет+начисление другая зарплата  бюджет
class iceb_u_double shet_nah_suma_fz_bt; //Суммы для Счет+начисление входящие в фонд зарплаты  бюджет
class iceb_u_double shet_uder_suma_bt; //Суммы для Счет+удержание  бюджет

class iceb_u_double shet_nah_suma_ht; //Суммы для Счет+начисление хозрасчет
class iceb_u_double shet_nah_suma_os_ht; //Суммы для Счет+начисление основная зарплата хозрасчет
class iceb_u_double shet_nah_suma_do_ht; //Суммы для Счет+начисление дополнительная зарплата хозрасчет
class iceb_u_double shet_nah_suma_dr_ht; //Суммы для Счет+начисление другая зарплата хозрасчет
class iceb_u_double shet_nah_suma_fz_ht; //Суммы для Счет+начисление входящие в фонд зарплаты хозрасчет
class iceb_u_double shet_uder_suma_ht; //Суммы для Счет+удержание  хозрасчет

//int       koldtr=0;
int       kol_shet_nah=0;
int       kol_shet_uder=0;


//Определяем список счетов для начислений
sprintf(strsql,"select distinct shet from Zarp where datz >= '%d-%d-%d' and \
datz <= '%d-%d-%d' and prn='1' and suma <> 0. order by shet asc",gn,mn,dn,gk,mk,dk);
printf("strsql=%s\n",strsql);
if((kol_shet_nah=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kol_shet_nah == 0)
 {
  iceb_menu_soob(gettext("Не найдена ни одна запись !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
  sp_shet_nah.plus(row[0]);

//Определяем список счетов для удержаний
sprintf(strsql,"select distinct shet from Zarp where datz >= '%d-%d-%d' and \
datz <= '%d-%d-%d' and prn='2' and suma <> 0. order by shet asc",gn,mn,dn,gk,mk,dk);

if((kol_shet_uder=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


while(cur.read_cursor(&row) != 0)
  sp_shet_uder.plus(row[0]);


sprintf(strsql,"select prn,knah,suma,shet,podr from Zarp where datz >= '%d-%d-%d' and \
datz <= '%d-%d-%d' and suma <> 0.",gn,mn,dn,gk,mk,dk);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
//printw("%s\n",strsql);

float kolstr1=0;
int nomer=0;
double suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
    
  if(iceb_u_proverka(data->rk->podr.ravno(),row[4],0,0) != 0)
   continue;
  if(data->rk->kod_grup.getdlinna() > 1)
   {
    sprintf(strsql,"select grup from Podr where kod=%s",row[4]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     if(iceb_u_proverka(data->rk->kod_grup.ravno(),row1[0],0,0) != 0)
      continue;
   }
  
  
  suma=atof(row[2]);
  if(atoi(row[0]) ==  1) //Начисление
   {
    sprintf(strsql,"%s|%s",row[3],row[1]);
    if((nomer = shet_nah.find(strsql)) < 0)
      shet_nah.plus(strsql);

    shet_nah_suma.plus(suma,nomer);

    if(iceb_u_proverka(shetb,row[3],0,0) == 0)
     {
      shet_nah_suma_bt.plus(suma,nomer);
      shet_nah_suma_ht.plus(0.,nomer);
     }
    else
     {
      shet_nah_suma_ht.plus(suma,nomer);
      shet_nah_suma_bt.plus(0.,nomer);
     }
    
    //читаем начисление
    short vn=0;
    short fz=0;
    
    sprintf(strsql,"select vidn,fz from Nash where kod=%s",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      vn=atoi(row1[0]);
      fz=atoi(row1[1]);
     }     

    if(vn == 0)
      shet_nah_suma_os.plus(suma,nomer);
    else
      shet_nah_suma_os.plus(0.,nomer);
    
    if(vn == 1)
      shet_nah_suma_do.plus(suma,nomer);
    else
      shet_nah_suma_do.plus(0.,nomer);

    if(vn == 2)
      shet_nah_suma_dr.plus(suma,nomer);
    else
      shet_nah_suma_dr.plus(0.,nomer);

    if(fz == 0)
      shet_nah_suma_fz.plus(suma,nomer);
    else
      shet_nah_suma_fz.plus(0.,nomer);

    if(iceb_u_proverka(shetb,row[3],0,0) == 0)
     {
      if(vn == 0)
        shet_nah_suma_os_bt.plus(suma,nomer);
      else
        shet_nah_suma_os_bt.plus(0.,nomer);
      shet_nah_suma_os_ht.plus(0.,nomer);
      
      if(vn == 1)
        shet_nah_suma_do_bt.plus(suma,nomer);
      else
        shet_nah_suma_do_bt.plus(0.,nomer);
      shet_nah_suma_do_ht.plus(0.,nomer);

      if(vn == 2)
        shet_nah_suma_dr_bt.plus(suma,nomer);
      else
        shet_nah_suma_dr_bt.plus(0.,nomer);
      shet_nah_suma_dr_ht.plus(0.,nomer);

      if(fz == 0)
        shet_nah_suma_fz_bt.plus(suma,nomer);
      else
        shet_nah_suma_fz_bt.plus(0.,nomer);
      shet_nah_suma_fz_ht.plus(0.,nomer);
      

     }
    else
     {
      if(vn == 0)
        shet_nah_suma_os_ht.plus(suma,nomer);
      else
        shet_nah_suma_os_ht.plus(0.,nomer);
      shet_nah_suma_os_bt.plus(0.,nomer);
      
      if(vn == 1)
        shet_nah_suma_do_ht.plus(suma,nomer);
      else
        shet_nah_suma_do_ht.plus(0.,nomer);
      shet_nah_suma_do_bt.plus(0.,nomer);

      if(vn == 2)
        shet_nah_suma_dr_ht.plus(suma,nomer);
      else
        shet_nah_suma_dr_ht.plus(0.,nomer);
      shet_nah_suma_dr_bt.plus(0.,nomer);

      if(fz == 0)
        shet_nah_suma_fz_ht.plus(suma,nomer);
      else
        shet_nah_suma_fz_ht.plus(0.,nomer);
      shet_nah_suma_fz_bt.plus(0.,nomer);

     }          
   }   
  else  //Удержание
   {
    sprintf(strsql,"%s|%s",row[3],row[1]);
    if((nomer = shet_uder.find(strsql)) < 0)
      shet_uder.plus(strsql);
    shet_uder_suma.plus(suma,nomer);

    if(iceb_u_proverka(shetbu,row[3],0,0) == 0)
     {
      shet_uder_suma_bt.plus(suma,nomer);
      shet_uder_suma_ht.plus(0.,nomer);
     }
    else
     {
      shet_uder_suma_ht.plus(suma,nomer);
      shet_uder_suma_bt.plus(0.,nomer);
     }
   }

 }
FILE *ffbt=NULL;
FILE *ffht=NULL;
char imafbt[30];
char imafht[30];

if(shetb != NULL)
 { 
  sprintf(imafbt,"vpzbt%d.lst",getpid());
  sprintf(imafht,"vpzht%d.lst",getpid());

  if((ffbt = fopen(imafbt,"w")) == NULL)
   {
    iceb_er_op_fil(imafbt,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  iceb_u_zagolov(gettext("Ведомость производственных затрат (бюджет)"),dn,mn,gn,dk,mk,gk,organ,ffbt);
  fprintf(ffbt,"%s\n",gettext("Бюджет"));
  
  if((ffht = fopen(imafht,"w")) == NULL)
   {
    iceb_er_op_fil(imafht,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  iceb_u_zagolov(gettext("Ведомость производственных затрат (хозрасчёт)"),dn,mn,gn,dk,mk,gk,organ,ffht);
  fprintf(ffht,"%s\n",gettext("Хозрасчет"));
 }
 
char imaf[30];
sprintf(imaf,"vpz%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
    iceb_er_op_fil(imaf,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
 }
 
iceb_u_zagolov(gettext("Ведомость производственных затрат"),dn,mn,gn,dk,mk,gk,organ,ff);

vedprz1_sap(data->rk,ff);


if(data->rk->metka_pr_ras == 0 || data->rk->metka_pr_ras == 1)
 {
  fprintf(ff,"\n%s\n",gettext("Начисления"));
  veprz1rn(dn,mn,gn,dk,mk,gk,bf,ff,&sp_shet_nah,&shet_nah,&shet_nah_suma,&shet_nah_suma_os,&shet_nah_suma_do,
  &shet_nah_suma_dr,&shet_nah_suma_fz,data->rk->shet.ravno(),data->window);
 }
if(data->rk->metka_pr_ras == 0 || data->rk->metka_pr_ras == 2)
 {
  fprintf(ff,"\n%s\n",gettext("Удержания"));
  veprz1r(dn,mn,gn,dk,mk,gk,bf,ff,&sp_shet_uder,&shet_uder,&shet_uder_suma,data->rk->shet.ravno());
 }

fprintf(ff,"%s____________________%s\n",gettext("Главный бухгалтер"),
glavbuh);

iceb_podpis(ff,data->window);
fclose(ff);


if(ffbt != NULL)
 {
 
   vedprz1_sap(data->rk,ffbt);

  if(data->rk->metka_pr_ras == 0 || data->rk->metka_pr_ras == 1)
   {
    fprintf(ffbt,"\n%s\n",gettext("Начисления"));
    veprz1rn(dn,mn,gn,dk,mk,gk,bf,ffbt,&sp_shet_nah,&shet_nah,&shet_nah_suma_bt,
    &shet_nah_suma_os_bt,&shet_nah_suma_do_bt,&shet_nah_suma_dr_bt,&shet_nah_suma_fz_bt,data->rk->shet.ravno(),data->window);
   }
  if(data->rk->metka_pr_ras == 0 || data->rk->metka_pr_ras == 2)
   {
    fprintf(ffbt,"\n%s\n",gettext("Удержания"));
    veprz1r(dn,mn,gn,dk,mk,gk,bf,ffbt,&sp_shet_uder,&shet_uder,&shet_uder_suma_bt,data->rk->shet.ravno());
   }

  fprintf(ffbt,"%s____________________%s\n",gettext("Главный бухгалтер"),
  glavbuh);
  iceb_podpis(ffbt,data->window);
  fclose(ffbt);

 }

if(ffht != NULL)
 {
 
   vedprz1_sap(data->rk,ffht);

  if(data->rk->metka_pr_ras == 0 || data->rk->metka_pr_ras == 1)
   {
    fprintf(ffht,"\n%s\n",gettext("Начисления"));
    veprz1rn(dn,mn,gn,dk,mk,gk,bf,ffht,&sp_shet_nah,&shet_nah,&shet_nah_suma_ht,
    &shet_nah_suma_os_ht,&shet_nah_suma_do_ht,&shet_nah_suma_dr_ht,&shet_nah_suma_fz_ht,data->rk->shet.ravno(),data->window);
   }
  if(data->rk->metka_pr_ras == 0 || data->rk->metka_pr_ras == 2)
   {
    fprintf(ffht,"\n%s\n",gettext("Удержания"));
    veprz1r(dn,mn,gn,dk,mk,gk,bf,ffht,&sp_shet_uder,&shet_uder,&shet_uder_suma_ht,data->rk->shet.ravno());
   }
  fprintf(ffht,"%s____________________%s\n",gettext("Главный бухгалтер"),
  glavbuh);
  iceb_podpis(ffht,data->window);
  fclose(ffht);

 }



data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Ведомость производственных затрат"));
if(ffbt != NULL)
 {
  data->rk->imaf.plus(imafbt);
  data->rk->imaf.plus(imafht);
  data->rk->naimf.plus(gettext("Ведомость производственных затрат (бюджет)"));
  data->rk->naimf.plus(gettext("Ведомость производственных затрат (хозрасчёт)"));
 }   
for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
