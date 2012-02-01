/*$Id: gosrsw_r.c,v 1.10 2011-02-21 07:35:51 sasa Exp $*/
/*14.12.2009	10.03.2006	Белых А.И.	gosrsw_r.c
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
#include "go.h"

class gosrsw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class go_rr *rk;
  int metkasort;
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  gosrsw_r_data()
   {
    metkasort=0;
    voz=1;
    kon_ras=1;
   }
 };
gboolean   gosrsw_r_key_press(GtkWidget *widget,GdkEventKey *event,class gosrsw_r_data *data);
gint gosrsw_r1(class gosrsw_r_data *data);
void  gosrsw_r_v_knopka(GtkWidget *widget,class gosrsw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int      kol_strok_na_liste;
extern short	startgodb; /*Стартовый год*/
extern short	vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/

int gosrsw_r(class go_rr *datark,int metkasort,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class gosrsw_r_data data;

data.rk=datark;
data.metkasort=metkasort;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт журнал-ордера по счёту"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(gosrsw_r_key_press),&data);

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

repl.plus(gettext("Расчёт журнал-ордера по счёту"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(gosrsw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)gosrsw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  gosrsw_r_v_knopka(GtkWidget *widget,class gosrsw_r_data *data)
{
//printf("gosrsw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   gosrsw_r_key_press(GtkWidget *widget,GdkEventKey *event,class gosrsw_r_data *data)
{
// printf("gosrsw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}

/****************************/
/*Шапка журнала ордера*/
/*****************************/
void gosrs_hapgo(short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,
int *kolstr,
FILE *ff)
{
*kollist+=1;
*kolstr+=6;
fprintf(ff,"%100s%s N%d\n","",gettext("Лист"),*kollist);
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("  Код     |    Наименование    | C а л ь д о  на  %02d.%02d.%d |   Счет   |      Оборот за период       | С а л ь д о  на  %02d.%02d.%d |\n"),
dn,mn,gn,dk,mk,gk);
fprintf(ff,gettext("контраген-|    контрагента     ------------------------------|кореспон- |------------------------------------------------------------\n"));
fprintf(ff,gettext("   та     |                    |   Дебет      |   Кредит     | дент     |   Дебет      |    Кредит    |   Дебет      |   Кредит     |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------\n");

}
/************************************/
/*Шапка конечного сальдо*/
/**********************************/
void gossrs_sapsk(short dk,short gk,short mk,int *kollist_sk,int *kolstr_sk,FILE *ff)
{
*kollist_sk+=1;
*kolstr_sk+=5;
fprintf(ff,"%68s%s N%d\n","",gettext("Лист"),*kollist_sk);

fprintf(ff,"\
----------------------------------------------------------------------------------\n");

fprintf(ff,gettext("  Код     |    Наименование    | C а л ь д о  на  %02d.%02d.%d |                    \n"),
dk,mk,gk);
fprintf(ff,gettext("контраген-|    контрагента     -------------------------------     Т е л е ф о н  \n"));
fprintf(ff,gettext("   та     |                    |   Дебет      |   Кредит     |                    \n"));

fprintf(ff,"\
----------------------------------------------------------------------------------\n");
}
/******************/
/*Счётчик строк*/
/*****************/
void gosrs_ss(int metka, /*0-оборотный баланс  1-журнал ордер*/
short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,int *kolstr,FILE *ff)
{
*kolstr+=1;

if(*kolstr <= kol_strok_na_liste)
  return;

fprintf(ff,"\f");
*kolstr=1;

if(metka == 0)
  sapgorsw(dn,mn,gn,dk,mk,gk,kollist,kolstr,ff);
if(metka == 1)
  gosrs_hapgo(dn,mn,gn,dk,mk,gk,kollist,kolstr,ff);
if(metka == 2)
  gossrs_sapsk(dk,gk,mk,kollist,kolstr,ff);
 
}

/**************************************/
/*Распечатка свода по счетам корреспондентам для журнала ордера*/
/**************************************************************/

void gosrs_itoggo(const char *shet,
short dn,short mn,short gn, //Дата начала периода
short dk,short mk,short gk, //Дата конца периода
const char *naim_shet,
class iceb_u_spisok *oss,
class iceb_u_spisok *kontr_shet,
class iceb_u_double *kontr_shet_deb,
class iceb_u_double *kontr_shet_kre,
FILE *ff,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
struct  tm      *bf;
time_t tmm=time(NULL);
bf=localtime(&tmm);

fprintf(ff,"\f\%s\n\n\
%s %s %s\n\
%s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s %s - %d:%d\n",
organ,
gettext("Счет"),
shet,naim_shet,
gettext("Всего по счетам корреспондентам"),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,gettext("  Счет    |   Наименование счета         |   Дебет       |   Кредит      |\n"));
fprintf(ff,"\
--------------------------------------------------------------------------\n");
double ideb=0.;
double ikre=0.;
double deb=0.;
double kre=0.;
class iceb_u_str shet_kor("");
for(int nomer_sheta=0; nomer_sheta < oss->kolih(); nomer_sheta++)
 {
  deb=kre=0.;
  for(int nomer_kontr_shet=0; nomer_kontr_shet < kontr_shet->kolih(); nomer_kontr_shet++)
   {
     
    iceb_u_polen(kontr_shet->ravno(nomer_kontr_shet),&shet_kor,2,'|');
    if(iceb_u_srav_r(shet_kor.ravno(),oss->ravno(nomer_sheta),0) != 0)
      continue;
    deb+=kontr_shet_deb->ravno(nomer_kontr_shet);
    kre+=kontr_shet_kre->ravno(nomer_kontr_shet);

   }

  sprintf(strsql,"select nais from Plansh where ns='%s'",oss->ravno(nomer_sheta));

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден счет"),oss->ravno(nomer_sheta));
    iceb_menu_soob(strsql,wpredok);
    continue;
   }
  strncpy(strsql,row[0],sizeof(strsql)-1);
  
  fprintf(ff,"%-*s %-*.*s %15s",
  iceb_u_kolbait(10,oss->ravno(nomer_sheta)),oss->ravno(nomer_sheta),
  iceb_u_kolbait(30,strsql),iceb_u_kolbait(30,strsql),strsql,
  iceb_u_prnbr(deb));
  
  fprintf(ff," %15s\n",iceb_u_prnbr(kre));
  ideb+=deb;
  ikre+=kre;
                  
 }
fprintf(ff,"\
--------------------------------------------------------------------------\n");
fprintf(ff,"%*s %15s",iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(ideb));

fprintf(ff," %15s\n",iceb_u_prnbr(ikre));

}
/************************/
/*проверка контрагента*/
/************************/
int gorsrs_provk(int nomer_kontr,
class iceb_u_double *debs,
class iceb_u_double *kres,
class iceb_u_double *debp,
class iceb_u_double *krep,
class iceb_u_spisok *kontr_shet,
class iceb_u_spisok *skontr)
{
int   metka_prop=0;  
int kolih_kontr_shet=kontr_shet->kolih();
class iceb_u_str kod_kontr("");

if(fabs(debs->ravno(nomer_kontr) - kres->ravno(nomer_kontr)) < 0.009)
 if(fabs(debp->ravno(nomer_kontr)) < 0.01 && fabs(krep->ravno(nomer_kontr)) < 0.009)
  {
   int nomer_kontr_shet=0;
   for(nomer_kontr_shet=0 ; nomer_kontr_shet < kolih_kontr_shet; nomer_kontr_shet++)
    {
      iceb_u_polen(kontr_shet->ravno(nomer_kontr_shet),&kod_kontr,1,'|');
      if(iceb_u_srav_r(kod_kontr.ravno(),skontr->ravno(nomer_kontr),0) == 0)
       {
        break;
       }
    }
   if(nomer_kontr_shet == kolih_kontr_shet)
    metka_prop=1;
  }
return(metka_prop);
}

/*******************************************/
/*Журнал ордер горизонтальный*/
/******************************************/
void gosrs_goriz(const char *shet,
short dn,short mn,short gn, //Дата начала периода
short dk,short mk,short gk, //Дата конца периода
class iceb_u_spisok *skontr, /*Список контрагентов*/
class iceb_u_spisok *skontr_sort, /*Список контрагентов отсортированный*/
class iceb_u_spisok *ssd, /*Список счетов дебета*/
class iceb_u_spisok *ssk, /*Список счетов кредита*/
class iceb_u_spisok *kontr_shet, /*Список код контрагента|счёт корреспондент*/
class iceb_u_double *kontr_shet_deb, /*суммы по дебету по списку */
class iceb_u_double *kontr_shet_kre, /*суммы по кредику по списку*/
class iceb_u_double *debs, /*Дебеты стартовые на начало периода*/
class iceb_u_double *kres, /*Кредиты стартовые на начало периода*/
class iceb_u_double *debp, /*Дебеты за период*/
class iceb_u_double *krep, /*Кредиты за период*/
char *imaf_gor,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naim_kontr("");
char strsql[512];
sprintf(imaf_gor,"gorgo%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf_gor,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_gor,"",errno,wpredok);
  return;
 }
int kolih_kre=ssk->kolih();
int kolih_deb=ssd->kolih();


class iceb_u_str liniq("----------------------------------------------------");
for(int nomer=0; nomer < kolih_deb+1; nomer++)
 liniq.plus("-----------");


int kolih_kontr=skontr->kolih();

class iceb_u_double kontr_shet_suma_deb;
class iceb_u_double kontr_shet_suma_kre;
kontr_shet_suma_deb.make_class(kolih_kontr*kolih_deb);
kontr_shet_suma_kre.make_class(kolih_kontr*kolih_kre);

char kod_kontr[20];
char shet_k[20];
int nomer_kontr=0;
int nomer_sheta=0;

for(int nomer_kontr_shet=0; nomer_kontr_shet < kontr_shet->kolih(); nomer_kontr_shet++)
 {
  iceb_u_polen(kontr_shet->ravno(nomer_kontr_shet),kod_kontr,sizeof(kod_kontr),1,'|');
  iceb_u_polen(kontr_shet->ravno(nomer_kontr_shet),shet_k,sizeof(shet_k),2,'|');
  nomer_kontr=skontr_sort->find_r(kod_kontr);

  nomer_sheta=ssd->find_r(shet_k);
  kontr_shet_suma_deb.plus(kontr_shet_deb->ravno(nomer_kontr_shet),nomer_kontr*kolih_deb+nomer_sheta);

  nomer_sheta=ssk->find_r(shet_k);
  kontr_shet_suma_kre.plus(kontr_shet_kre->ravno(nomer_kontr_shet),nomer_kontr*kolih_kre+nomer_sheta);

 }
            
//fprintf(ff,gettext("Д е б е т ы по счету %s за период с %d.%d.%d г. по %d.%d.%d г.\%s\n"),
fprintf(ff,"%s %s %s %d.%d.%d %s %d.%d.%d\n%s\n",
gettext("Дебет"),
shet,
gettext("за период з"),
dn,mn,gn,
gettext("по"),
dk,mk,gk,liniq.ravno());

fprintf(ff,"  Код   |   Наименование     |Сальдо на %02d.%02d.%04dг|",dn,mn,gn);
for(int kolih_deb=0; kolih_deb < ssd->kolih() ; kolih_deb++)
 fprintf(ff,"%10s|",ssd->ravno(kolih_deb));
fprintf(ff,"%-10.10s|\n",gettext("Итого"));


fprintf(ff,"контраг.|   контрагента      |  дебет   |  кредит  |");
for(int kolih_deb=0; kolih_deb < ssd->kolih() ; kolih_deb++)
 fprintf(ff,"%10s|","");
fprintf(ff,"%-10.10s|\n","");


fprintf(ff,"%s\n",liniq.ravno());

double deb=0.,kre=0.;
double itogo_gor=0.;
double idebs=0.;
double ikres=0.;
class iceb_u_double itogo_kol;
itogo_kol.make_class(kolih_deb);

for(int nomer_kontr_sort=0; nomer_kontr_sort < kolih_kontr; nomer_kontr_sort++)
 {
  nomer_kontr=skontr->find_r(skontr_sort->ravno(nomer_kontr_sort));
  if(gorsrs_provk(nomer_kontr,debs,kres,debp,krep,kontr_shet,skontr) != 0)
     continue;

  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",skontr->ravno(nomer_kontr));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
      continue;
  naim_kontr.new_plus(row[0]);

  deb=kre=0.;
  if(debs->ravno(nomer_kontr) > kres->ravno(nomer_kontr))
     deb=debs->ravno(nomer_kontr)-kres->ravno(nomer_kontr);
  else
     kre=kres->ravno(nomer_kontr)-debs->ravno(nomer_kontr);

  idebs+=deb;
  ikres+=kre;

  if(fabs(deb) > 0.009)
     fprintf(ff,"%-*s|%-*.*s|%10.2f|%10s|",
     iceb_u_kolbait(8,skontr->ravno(nomer_kontr)),skontr->ravno(nomer_kontr),
     iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
     deb,"");
  else
     fprintf(ff,"%-*s|%-*.*s|%10s|%10.2f|",
     iceb_u_kolbait(8,skontr->ravno(nomer_kontr)),skontr->ravno(nomer_kontr),
     iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
     "",kre);

  itogo_gor=0.;
  for(int nomer_sheta=0; nomer_sheta < kolih_deb; nomer_sheta++)
   {
    deb=kontr_shet_suma_deb.ravno(nomer_kontr_sort*kolih_deb+nomer_sheta);
    if(fabs(deb) > 0.009)
     {
      fprintf(ff,"%10.2f|",deb);
      itogo_gor+=deb;
      itogo_kol.plus(deb,nomer_sheta);
     }     
    else
     {
      fprintf(ff,"%10s|","");
     }
   }
  if(fabs(itogo_gor) > 0.009)
   fprintf(ff,"%10.2f|\n",itogo_gor);
  else
   fprintf(ff,"%10s|\n","");
  fprintf(ff,"%s\n",liniq.ravno());
 }

fprintf(ff,"%*s|%10.2f|%10.2f|",iceb_u_kolbait(29,gettext("Итого")),gettext("Итого"),idebs,ikres);
double idebp=0.;
for(int nomer_deb=0; nomer_deb < itogo_kol.kolih(); nomer_deb++)
 {
  fprintf(ff,"%10.2f|",itogo_kol.ravno(nomer_deb));
  idebp+=itogo_kol.ravno(nomer_deb);
 }

fprintf(ff,"%10.2f|\n",idebp);

/*******************************************/

itogo_kol.free_class();
itogo_kol.make_class(kolih_kre);

liniq.new_plus("------------------------------");
for(int nomer=0; nomer < kolih_kre+3; nomer++)
 liniq.plus("-----------");


fprintf(ff,"\f");


//fprintf(ff,gettext("К р е д и т ы по счету %s за период с %d.%d.%dр. по %d.%d.%dг.\n%s\n"),
fprintf(ff,"%s %s %s %d.%d.%d %s %d.%d.%d\n%s\n",
gettext("Кредит"),
shet,
gettext("за период з"),
dn,mn,gn,
gettext("по"),
dk,mk,gk,liniq.ravno());

fprintf(ff,"  Код   |   Наименование     |");
for(int nomer=0; nomer < kolih_kre ; nomer++)
 fprintf(ff,"%*s|",iceb_u_kolbait(10,ssk->ravno(nomer)),ssk->ravno(nomer));

fprintf(ff,"%-*.*s|Сальдо на %02d.%02d.%04dг|\n",
iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),dk,mk,gk);


fprintf(ff,"контраг.|   контрагента      |");
for(int nomer=0; nomer < ssk->kolih() ; nomer++)
 fprintf(ff,"%10s|","");
fprintf(ff,"%-10.10s|  дебет   |  кредит  |\n","");

fprintf(ff,"%s\n",liniq.ravno());

/*Печатаем Кредиты*/
double debk=0.;
double krek=0.;
double idebk=0.;
double ikrek=0.;

for(int nomer_kontr_sort=0; nomer_kontr_sort < kolih_kontr; nomer_kontr_sort++)
 {
  nomer_kontr=skontr->find_r(skontr_sort->ravno(nomer_kontr_sort));
  if(gorsrs_provk(nomer_kontr,debs,kres,debp,krep,kontr_shet,skontr) != 0)
   continue;

  debk=krek=0.;
  if(debs->ravno(nomer_kontr)+debp->ravno(nomer_kontr) > kres->ravno(nomer_kontr)+krep->ravno(nomer_kontr))
   debk=debs->ravno(nomer_kontr)+debp->ravno(nomer_kontr) - (kres->ravno(nomer_kontr)+krep->ravno(nomer_kontr));
  else
   krek=kres->ravno(nomer_kontr)+krep->ravno(nomer_kontr)-(debs->ravno(nomer_kontr)+debp->ravno(nomer_kontr));

  idebk+=debk;
  ikrek+=krek;
    
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",skontr->ravno(nomer_kontr));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
    continue;
  naim_kontr.new_plus(row[0]);
  
  
  fprintf(ff,"%-*s|%-*.*s|",
  iceb_u_kolbait(8,skontr->ravno(nomer_kontr)),skontr->ravno(nomer_kontr),
  iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno());
  

  itogo_gor=0.;
  for(int nomer_sheta=0; nomer_sheta < kolih_kre; nomer_sheta++)
   {
    kre=kontr_shet_suma_kre.ravno(nomer_kontr_sort*kolih_kre+nomer_sheta);
    if(kre != 0.)
     {
      fprintf(ff,"%10.2f|",kre);
      itogo_gor+=kre;
      itogo_kol.plus(kre,nomer_sheta);
     }     
    else
     {
      fprintf(ff,"%10s|","");
     }
   }

  if(fabs(itogo_gor) > 0.009)
    fprintf(ff,"%10.2f|",itogo_gor);
  else
    fprintf(ff,"%10s|","");

  if(fabs(debk) > 0.009)
    fprintf(ff,"%10.2f|",debk);
  else
    fprintf(ff,"%10s|","");

  if(fabs(krek) > 0.009)
    fprintf(ff,"%10.2f|\n",krek);
  else
    fprintf(ff,"%10s|\n","");

  fprintf(ff,"%s\n",liniq.ravno());
 }
fprintf(ff,"%*s|",iceb_u_kolbait(29,gettext("Итого")),gettext("Итого"));
double ikrep=0.;
for(int nomer_kre=0; nomer_kre < itogo_kol.kolih(); nomer_kre++)
 {
  fprintf(ff,"%10.2f|",itogo_kol.ravno(nomer_kre));
  ikrep+=itogo_kol.ravno(nomer_kre);
 }

fprintf(ff,"%10.2f|%10.2f|%10.2f|\n",ikrep,idebk,ikrek);




fprintf(ff,"\n%s__________________________\n",
gettext("Главный бухгалтер"));

iceb_podpis(ff,wpredok);
fclose(ff);



}
/*********************/
/*Распечатка по дням*/
/***********************/
void gorsrs_podn(const char *shet,
const char *naim_shet,
short dn,short mn,short gn, //Дата начала периода
short dk,short mk,short gk, //Дата конца периода

class iceb_u_spisok *ssd, /*Список счетов дебета*/
class iceb_u_spisok *ssk, /*Список счетов кредита*/

class iceb_u_spisok *den_shet_deb, /*Список день|счёт для дебетов*/
class iceb_u_spisok *den_shet_kre, /*Список день|счёт для кредитов*/
class iceb_u_double *den_shet_deb_suma, /*Сумма по дню дебета*/
class iceb_u_double *den_shet_kre_suma, /*Сумма по дню кредита*/
double debs,double kres,
double debp,double krep,
double debk,double krek,
char *imaf,
GtkWidget *wpredok)
{
char strsql[512];
sprintf(imaf,"gopd%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
int kolih_deb=ssd->kolih();
int kolih_kre=ssk->kolih();


class iceb_u_str liniq;
for(int nomer=0; nomer < kolih_deb+2; nomer++)
 liniq.plus("-----------");




class iceb_u_spisok dni;
int kolih_dni=iceb_u_period(dn,mn,gn,dk,mk,gk,0);
int kolih_mes_v_per=iceb_u_period(dn,mn,gn,dk,mk,gk,1);
short d=dn,m=mn,g=gn;

for(int nomer=0; nomer < kolih_dni; nomer++)
 {
  sprintf(strsql,"%02d.%02d.%04d",d,m,g);
  dni.plus(strsql);
  iceb_u_dpm(&d,&m,&g,1);
 }

class iceb_u_double den_shet_suma;
den_shet_suma.make_class(kolih_dni*kolih_deb);
char den_mes_god[20];
char shet_d[20];
int nomer_sheta=0;
int nomer_data=0;
for(int nomer_mes_v_per=0; nomer_mes_v_per < kolih_mes_v_per; nomer_mes_v_per++)
for(int nomer_den_shet=0; nomer_den_shet < den_shet_deb[nomer_mes_v_per].kolih(); nomer_den_shet++)
 {
  iceb_u_polen(den_shet_deb[nomer_mes_v_per].ravno(nomer_den_shet),den_mes_god,sizeof(den_mes_god),1,'|');
  iceb_u_polen(den_shet_deb[nomer_mes_v_per].ravno(nomer_den_shet),shet_d,sizeof(shet_d),2,'|');
  nomer_data=dni.find(den_mes_god);  
  nomer_sheta=ssd->find_r(shet_d);
  den_shet_suma.plus(den_shet_deb_suma[nomer_mes_v_per].ravno(nomer_den_shet),nomer_data*kolih_deb+nomer_sheta);
 }

struct  tm      *bf;
time_t vr=time(NULL);
bf=localtime(&vr);

fprintf(ff,"\x1B%s\x1E\n","3"); /*Изменение межстрочного интервала*/

fprintf(ff,"%s\n%s %s %s\n",organ,
gettext("Журнал-ордер по счёту"),shet,naim_shet);

sprintf(strsql,"%s %s %s %d.%d.%d%s %s: %d:%d",
gettext("Оборот по дебету счета"),shet,
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

buhdenshetw(strsql,&dni,ssd,&den_shet_suma,ff);

iceb_podpis(ff,wpredok);

/************************************************************/





den_shet_suma.free_class();
den_shet_suma.make_class(kolih_dni*kolih_kre);

for(int nomer_mes_v_per=0; nomer_mes_v_per < kolih_mes_v_per; nomer_mes_v_per++)
for(int nomer_den_shet=0; nomer_den_shet < den_shet_kre[nomer_mes_v_per].kolih(); nomer_den_shet++)
 {
  iceb_u_polen(den_shet_kre[nomer_mes_v_per].ravno(nomer_den_shet),den_mes_god,sizeof(den_mes_god),1,'|');
  iceb_u_polen(den_shet_kre[nomer_mes_v_per].ravno(nomer_den_shet),shet_d,sizeof(shet_d),2,'|');
  nomer_data=dni.find(den_mes_god);  
  nomer_sheta=ssk->find_r(shet_d);
  den_shet_suma.plus(den_shet_kre_suma[nomer_mes_v_per].ravno(nomer_den_shet),nomer_data*kolih_kre+nomer_sheta);
 }

fprintf(ff,"\f");
fprintf(ff,"%s\n%s %s %s\n",organ,
gettext("Журнал-ордер по счёту"),shet,naim_shet);

sprintf(strsql,"%s %s %s %d.%d.%d%s %s: %d:%d",
gettext("Оборот по кредиту счета"),shet,
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

buhdenshetw(strsql,&dni,ssk,&den_shet_suma,ff);


fprintf(ff,"\
        		   %s           %s\n",
gettext("Дебет"),
gettext("Кредит"));

fprintf(ff,"\
%*s:%15.2f %15.2f\n",
iceb_u_kolbait(18,gettext("Сальдо начальное")),gettext("Сальдо начальное"),debs,kres);

fprintf(ff,"\
%*s:%15.2f  %15.2f\n",iceb_u_kolbait(18,gettext("Оборот за период")),gettext("Оборот за период"),debp,krep);

fprintf(ff,"\
%*s:%15.2f %15.2f\n",iceb_u_kolbait(18,gettext("Сальдо конечное")),gettext("Сальдо конечное"),debk,krek);

fprintf(ff,"\n%s__________________________\n",
gettext("Главный бухгалтер"));

fclose(ff);

}


/******************************************/
/******************************************/

gint gosrsw_r1(class gosrsw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
SQLCURSOR cur,cur1;

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
double deb=0.,kre=0.;
class iceb_u_spisok skontr; /*Список контрагентов*/
class iceb_u_spisok skontr_sort; /*Список контрагентов отсортированный*/
class iceb_u_spisok ssd; /*Список счетов дебета*/
class iceb_u_spisok ssk; /*Список счетов кредита*/
class iceb_u_spisok oss; /*Общий список счетов*/


class iceb_u_spisok kontr_shet; /*Список код контрагента|счёт корреспондент*/
class iceb_u_double kontr_shet_deb; /*суммы по дебету по списку */
class iceb_u_double kontr_shet_kre; /*суммы по кредиту по списку*/

class iceb_u_double hach_kontr_shet_deb; /*шахматка контрагент-счёт по дебету*/
class iceb_u_double hach_kontr_shet_kre; /*шахматка контрагент-счёт по кредиту*/

class iceb_u_double debs; /*Дебеты стартовые на начало периода*/
class iceb_u_double kres; /*Кредиты стартовые на начало периода*/
class iceb_u_double debp; /*Дебеты за период*/
class iceb_u_double krep; /*Кредиты за период*/

short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;


/*Узнаём наименование счёта*/
class iceb_u_str naim_shet("");
sprintf(strsql,"select nais from Plansh where ns='%s'",data->rk->shet.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 naim_shet.new_plus(row[0]);

sprintf(strsql,"%s %s\n",data->rk->shet.ravno(),naim_shet.ravno());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
sprintf(strsql,"%02d.%02d.%d -> %02d.%02d.%d\n",dn,mn,gn,dk,mk,gk);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

/*Узнаем начальное сальдо по каждой организации*/
sprintf(strsql,"%s\n",gettext("Вычисляем стартовое сальдо по контрагентам"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(data->rk->vds == 0 || vplsh == 1)
 sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d \
and ns like '%s%%'",godn,data->rk->shet.ravno());

if(data->rk->vds == 1 && vplsh == 0)
 sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d \
and ns='%s'",godn,data->rk->shet.ravno());

//printw("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
int nomer_kontr=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(data->rk->kodgr.ravno()[0] != '\0')
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден код контрагента"),row[0]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    if(iceb_u_proverka(data->rk->kodgr.ravno(),row1[0],0,0) != 0)
      continue;  
   }

  if((nomer_kontr=skontr.find_r(row[0])) < 0)
   {
    skontr.plus(row[0]);   
    debp.plus(0.,-1);
    krep.plus(0.,-1);
   }
  debs.plus(atof(row[1]),nomer_kontr);
  kres.plus(atof(row[2]),nomer_kontr);


 }
int kolih_mes_v_per=iceb_u_period(1,mn,gn,1,mk,gk,1);
class iceb_u_spisok den_shet_deb[kolih_mes_v_per]; /*Список день|счёт для дебетов*/
class iceb_u_spisok den_shet_kre[kolih_mes_v_per]; /*Список день|счёт для кредитов*/
class iceb_u_double den_shet_deb_suma[kolih_mes_v_per]; /*Сумма по дню дебета*/
class iceb_u_double den_shet_kre_suma[kolih_mes_v_per]; /*Сумма по дню кредита*/

/*смотрим проводки заполняем массивы*/
int nomer_mes_v_per=-1;
int nomer_den_shet=0;
int nomer_kontr_shet=0;
short d=1,m=1,g=godn;
int kolih_den=iceb_u_period(1,1,godn,dk,mk,gk,0);
float kolstr1=0.;
short mes_zap=0;
while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  
//  strzag(LINES-1,0,kolih_den,++kolstr1);
  iceb_pbar(data->bar,kolih_den,++kolstr1);    

  if(data->rk->vds == 0 || vplsh == 1)
   sprintf(strsql,"select sh,shk,kodkon,deb,kre from Prov \
 where val=0 and datp='%d-%02d-%02d' and sh like '%s%%'",g,m,d,data->rk->shet.ravno());

  if(data->rk->vds == 1 && vplsh == 0)
   sprintf(strsql,"select sh,shk,kodkon,deb,kre from Prov \
 where val=0 and datp='%d-%02d-%02d' and sh='%s'",g,m,d,data->rk->shet.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(1);
   }

  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0) /*период*/
  if(mes_zap != m)
   {
    nomer_mes_v_per++;
    mes_zap=m;
   }            
  
  while(cur.read_cursor(&row) != 0)
   {
/********
    printw("%02d.%02d.%04d %6s %6s %6s %10s %10s\n",d,m,g,row[0],row[1],row[2],row[3],row[4]);
    refresh();
************/
    if(data->rk->kodgr.ravno()[0] != '\0')
     {
      sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[2]);
      if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
       {
        sprintf(strsql,"%s %s!",gettext("Не найден код контрагента"),row[2]);
        iceb_menu_soob(strsql,data->window);
        continue;
       }
      if(iceb_u_proverka(data->rk->kodgr.ravno(),row1[0],0,0) != 0)
        continue;  
     }
    if(row[2][0] == '\0')
     {
      sprintf(strsql,"%d.%d.%d %s %s %s %s %s\n%s",d,m,g,row[9],row[1],row[2],row[3],row[4],
      gettext("В проводке нет кода контрагента !"));
      iceb_menu_soob(strsql,data->window);
      continue;
     }

    deb=atof(row[3]);
    kre=atof(row[4]);
    if((nomer_kontr=skontr.find_r(row[2])) < 0)
     {
      skontr.plus(row[2]);
      debs.plus(0.,-1);
      kres.plus(0.,-1);
      debp.plus(0.,-1);
      krep.plus(0.,-1);
      /*узнаём под каким номером записан*/
      nomer_kontr=skontr.kolih()-1;    
     }

    if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0) /*До периода*/
     {
      debs.plus(deb,nomer_kontr);
      kres.plus(kre,nomer_kontr);
     }
    else /*Период*/
     {
      if(oss.find_r(row[1]) < 0)
         oss.plus(row[1]);
      debp.plus(deb,nomer_kontr);
      krep.plus(kre,nomer_kontr);

      sprintf(strsql,"%s|%s",row[2],row[1]);
      if((nomer_kontr_shet=kontr_shet.find_r(strsql)) < 0)
       kontr_shet.plus(strsql);

      /*Всегда дебет и кредит независимо от их значений*/
      kontr_shet_deb.plus(deb,nomer_kontr_shet);
      kontr_shet_kre.plus(kre,nomer_kontr_shet);


      if(fabs(deb) > 0.009)
       {
        if(ssd.find_r(row[1]) < 0)
         ssd.plus(row[1]);

        sprintf(strsql,"%02d.%02d.%04d|%s",d,m,g,row[1]);
        if((nomer_den_shet=den_shet_deb[nomer_mes_v_per].find_r(strsql)) < 0)
         den_shet_deb[nomer_mes_v_per].plus(strsql);
        den_shet_deb_suma[nomer_mes_v_per].plus(deb,nomer_den_shet);
       }
      else
       {
        if(ssk.find_r(row[1]) < 0)
          ssk.plus(row[1]);

        sprintf(strsql,"%02d.%02d.%04d|%s",d,m,g,row[1]);
        if((nomer_den_shet=den_shet_kre[nomer_mes_v_per].find_r(strsql)) < 0)
         den_shet_kre[nomer_mes_v_per].plus(strsql);
        den_shet_kre_suma[nomer_mes_v_per].plus(kre,nomer_den_shet);
       }
     }
   }

  iceb_u_dpm(&d,&m,&g,1);
 }

if(iceb_sortkk(data->metkasort,&skontr,&skontr_sort,data->window) != 0)
 {
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
 }

int kolih_kontr=skontr.kolih();



int kolih_shet=oss.kolih();
int nomer_sheta=0;
char kod_kontr0[20];
char shet_k[20];
/*заряжаем шахматки*/
hach_kontr_shet_deb.make_class(kolih_kontr*kolih_shet);
hach_kontr_shet_kre.make_class(kolih_kontr*kolih_shet);
for(int nomer_kontr_shet=0; nomer_kontr_shet < kontr_shet.kolih(); nomer_kontr_shet++)
 {
  iceb_u_polen(kontr_shet.ravno(nomer_kontr_shet),kod_kontr0,sizeof(kod_kontr0),1,'|');  
  iceb_u_polen(kontr_shet.ravno(nomer_kontr_shet),shet_k,sizeof(shet_k),2,'|');  

  nomer_kontr=skontr_sort.find_r(kod_kontr0);

  nomer_sheta=oss.find_r(shet_k);

  hach_kontr_shet_deb.plus(kontr_shet_deb.ravno(nomer_kontr_shet),nomer_kontr*kolih_shet+nomer_sheta);

  hach_kontr_shet_kre.plus(kontr_shet_kre.ravno(nomer_kontr_shet),nomer_kontr*kolih_shet+nomer_sheta);
 }

sprintf(strsql,"%s\n",gettext("Вертикальна форма распечатки по контрагентам"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int metka_naim_shet=0;
memset(strsql,'\0',sizeof(strsql));
if(iceb_poldan("Печатать наименование счёта корреспондента в журналах-ордерах для счетов с развёрнутым сальдо",strsql,"nastrb.alx",data->window) == 0)
 if(iceb_u_SRAV("Вкл",strsql,1) == 0)
  metka_naim_shet=1;

char imaf_obor[50];
char imaf_gur[50];
char imaf_sk[50];
FILE *ff_obor;
FILE *ff_gur;
FILE *ff_sk;

sprintf(imaf_obor,"obor%d.lst",getpid());
sprintf(imaf_gur,"gurrs%d.lst",getpid());
sprintf(imaf_sk,"salk%d.lst",getpid());

if((ff_obor = fopen(imaf_obor,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_obor,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if((ff_gur = fopen(imaf_gur,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_gur,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if((ff_sk = fopen(imaf_sk,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_sk,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


struct  tm      *bf;
time_t tmm=time(NULL);

bf=localtime(&tmm);
fprintf(ff_obor,"%s\n\n\
			      %s %s %s\n\
				  %s %d.%d.%d%s %s %d.%d.%d%s\n\
			      %s %d.%d.%d%s  %s: %d.%d\n",
organ,
gettext("Оборот по счету"),
data->rk->shet.ravno(),naim_shet.ravno(),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

int kolstr_obor=5; 

fprintf(ff_gur,"%s\n\n\
			       %s %s %s\n\
				  %s %d.%d.%d%s %s %d.%d.%d%s\n\
			       %s %d.%d.%d  %s - %d:%d\n",
organ,
gettext("Журнал ордер по счету"),
data->rk->shet.ravno(),naim_shet.ravno(),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

int kolstr_gur=5;

fprintf(ff_sk,"%s\n\n\
%s %d.%d.%d%s %s %s %s\n\
%s %d.%d.%d%s  %s: %d.%d\n",
organ,
gettext("Конечное сальдо на"),
dk,mk,gk,
gettext("г."),
gettext("по счету"),
data->rk->shet.ravno(),naim_shet.ravno(),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

int kolstr_sk=4;

int klst=1;
if(data->rk->kodgr.ravno()[0] != '\0')
 {
  fprintf(ff_obor,"%s: %s\n",gettext("Список контрагентов"),data->rk->kodgr.ravno());
  iceb_printcod(ff_obor,"Gkont","kod","naik",0,data->rk->kodgr.ravno(),&klst);

  klst=0;
  fprintf(ff_gur,"%s: %s\n",gettext("Список контрагентов"),data->rk->kodgr.ravno());
  iceb_printcod(ff_gur,"Gkont","kod","naik",0,data->rk->kodgr.ravno(),&klst);

  klst=0;
  fprintf(ff_sk,"%s: %s\n",gettext("Список контрагентов"),data->rk->kodgr.ravno());
  iceb_printcod(ff_sk,"Gkont","kod","naik",0,data->rk->kodgr.ravno(),&klst);
 }

kolstr_gur+=klst;
kolstr_obor+=klst;
kolstr_sk+=klst;
int kollist_gur=0;
int kollist_obor=0;
int kollist_sk=0;
/*Шапка оборотного баланса*/
sapgorsw(dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor);
/*Шапка журнал-ордера*/
gosrs_hapgo(dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur);
/*Шапка конечного сальдо*/
gossrs_sapsk(dk,gk,mk,&kollist_sk,&kolstr_sk,ff_sk);

//int metka_prop=0;
class iceb_u_str kod_kontr;
class iceb_u_str naim_kontr;
class iceb_u_str telef;
int grupa=0;
int grupaz=0;
double   sdgr=0.,skgr=0.,sdngr=0.,skngr=0.,dnsgr=0.,knsgr=0.;
double idebs=0.,ikres=0.;
double idebp=0.,ikrep=0.;
double idebk=0.,ikrek=0.;
for(int nomer_kontr_sort=0; nomer_kontr_sort < kolih_kontr; nomer_kontr_sort++)
 {
  nomer_kontr=skontr.find_r(skontr_sort.ravno(nomer_kontr_sort));
  if(gorsrs_provk(nomer_kontr,&debs,&kres,&debp,&krep,&kontr_shet,&skontr) != 0)
   continue;

  sprintf(strsql,"select kodkon,naikon,telef,grup from Kontragent where kodkon='%s'",skontr.ravno(nomer_kontr));
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    fprintf(ff_obor,"\n\n%s %s!!!\n\n",gettext("Не найден код контрагента"),skontr.ravno(nomer_kontr));
    fprintf(ff_gur,"\n\n%s %s!!!\n\n",gettext("Не найден код контрагента"),skontr.ravno(nomer_kontr));
    sprintf(strsql,"%s %s",gettext("Не найден код контрагента"),skontr.ravno(nomer_kontr));
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  naim_kontr.new_plus(row1[1]);
  telef.new_plus(row1[2]);
  grupa=atoi(row1[3]); 

  if(data->metkasort == 4)
   if(grupaz != grupa)
    {
     if(grupaz != 0)
      {
     gosrs_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor);
     fprintf(ff_obor,"\
 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
     class iceb_u_str naimgrup("");       
     sprintf(strsql,"select naik from Gkont where kod=%d",grupaz);
     if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
         naimgrup.new_plus(row1[0]);

     gosrs_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor);

     sprintf(strsql,"%s %d %s",gettext("Итого по группе"),grupaz,naimgrup.ravno());
     fprintf(ff_obor,"\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
     iceb_u_kolbait(31,strsql),iceb_u_kolbait(31,strsql),strsql,
     dnsgr,knsgr,sdngr,skngr,sdgr,skgr);

     /*****************************************/

     gosrs_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur);
     fprintf(ff_gur,"\
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");


     sprintf(strsql,"%s %d %s",gettext(" Итого по группе"),grupaz,naimgrup.ravno());

     gosrs_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur);
     fprintf(ff_gur,"\
%-*.*s %14.2f %14.2f %10s %14.2f %14.2f %14.2f %14.2f\n",
     iceb_u_kolbait(31,strsql),iceb_u_kolbait(31,strsql),strsql,dnsgr,knsgr," ",sdngr,skngr,sdgr,skgr);
     }

     sdgr=skgr=sdngr=skngr=dnsgr=knsgr=0.;
     grupaz=grupa;
    }

  deb=kre=0.;
  if(debs.ravno(nomer_kontr) > kres.ravno(nomer_kontr))
   deb=debs.ravno(nomer_kontr)-kres.ravno(nomer_kontr);
  else
   kre=kres.ravno(nomer_kontr)-debs.ravno(nomer_kontr);

  gosrs_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor);
  gosrs_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur);
  gosrs_ss(2,dn,mn,gn,dk,mk,gk,&kollist_sk,&kolstr_sk,ff_sk);
  if(deb > 0.)
   {
    idebs+=deb;
    dnsgr+=deb;
    fprintf(ff_obor,"%*s %-*.*s %14.2f %14s %14.2f %14.2f ",
    iceb_u_kolbait(10,skontr.ravno(nomer_kontr)),skontr.ravno(nomer_kontr),
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    deb," ",debp.ravno(nomer_kontr),krep.ravno(nomer_kontr));

    fprintf(ff_gur,"%*s %-*.*s %14.2f %14s %10s %14.2f %14.2f ",
    iceb_u_kolbait(10,skontr.ravno(nomer_kontr)),skontr.ravno(nomer_kontr),
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    deb," "," ",debp.ravno(nomer_kontr),krep.ravno(nomer_kontr));
   }
  else
   {
    ikres+=kre;
    knsgr+=kre;
    fprintf(ff_obor,"%*s %-*.*s %14s %14.2f %14.2f %14.2f ",
    iceb_u_kolbait(10,skontr.ravno(nomer_kontr)),skontr.ravno(nomer_kontr),
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    " ",kre,debp.ravno(nomer_kontr),krep.ravno(nomer_kontr));

    fprintf(ff_gur,"%*s %-*.*s %14s %14.2f %10s %14.2f %14.2f ",
    iceb_u_kolbait(10,skontr.ravno(nomer_kontr)),skontr.ravno(nomer_kontr),
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    " ",kre," ",debp.ravno(nomer_kontr),krep.ravno(nomer_kontr));
   }


  idebp+=debp.ravno(nomer_kontr);
  ikrep+=krep.ravno(nomer_kontr);

  deb=kre=0.;
  if(debs.ravno(nomer_kontr)+debp.ravno(nomer_kontr) > kres.ravno(nomer_kontr)+krep.ravno(nomer_kontr))
   deb=debs.ravno(nomer_kontr)+debp.ravno(nomer_kontr) - (kres.ravno(nomer_kontr)+krep.ravno(nomer_kontr));
  else
   kre=kres.ravno(nomer_kontr)+krep.ravno(nomer_kontr)-(debs.ravno(nomer_kontr)+debp.ravno(nomer_kontr));

  if(deb > 0.)
   {
    idebk+=deb;
    sdgr+=deb;
    fprintf(ff_obor,"%14.2f\n",deb);
    fprintf(ff_gur,"%14.2f\n",deb);

    fprintf(ff_sk,"%*s %-*.*s %14.2f %14s %s\n",
    iceb_u_kolbait(10,skontr.ravno(nomer_kontr)),skontr.ravno(nomer_kontr),
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    deb," ",telef.ravno());

   }
  else
   {
    fprintf(ff_obor,"%14s %14.2f\n"," ",kre);
    fprintf(ff_gur,"%14s %14.2f\n"," ",kre);
    fprintf(ff_sk,"%*s %-*.*s %14s %14.2f %s\n",
    iceb_u_kolbait(10,skontr.ravno(nomer_kontr)),skontr.ravno(nomer_kontr),
    iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
    " ",kre,telef.ravno());
    ikrek+=kre;
    skgr+=kre;
   }

  if(iceb_u_strlen(naim_kontr.ravno()) > 20)
   {
    gosrs_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor);
    fprintf(ff_obor,"%-10s %s\n"," ",iceb_u_adrsimv(20,naim_kontr.ravno()));

    gosrs_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur);
    fprintf(ff_gur,"%-10s %s\n"," ",iceb_u_adrsimv(20,naim_kontr.ravno()));

    gosrs_ss(2,dn,mn,gn,dk,mk,gk,&kollist_sk,&kolstr_sk,ff_sk);
    fprintf(ff_sk,"%-10s %s\n"," ",iceb_u_adrsimv(20,naim_kontr.ravno()));
   }

 for(int nomer_shet=0; nomer_shet < kolih_shet; nomer_shet++)
  {
   deb=hach_kontr_shet_deb.ravno(nomer_kontr_sort*kolih_shet+nomer_shet);
   kre=hach_kontr_shet_kre.ravno(nomer_kontr_sort*kolih_shet+nomer_shet);

   if(fabs(deb) < 0.009 && fabs(kre) < 0.009)
    continue;
    if(metka_naim_shet == 1)
     {
        class iceb_u_str naim_shet_kor("");
        sprintf(strsql,"select nais from Plansh where ns='%s'",oss.ravno(nomer_shet));
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
         naim_shet_kor.new_plus(row[0]);
        sprintf(strsql,"%s %s",naim_shet_kor.ravno(),oss.ravno(nomer_shet));
     }
    else
      sprintf(strsql,"%s",oss.ravno(nomer_shet));
      
    gosrs_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur);

    fprintf(ff_gur,"%*.*s %14.2f %14.2f\n",
    iceb_u_kolbait(72,strsql),iceb_u_kolbait(72,strsql),strsql,
    deb,kre);


  }

  gosrs_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur);
  fprintf(ff_gur,"\
.....................................................................................................................................\n");

  sdngr+=debp.ravno(nomer_kontr);
  skngr+=krep.ravno(nomer_kontr);
 }
if(data->metkasort == 4)
  {
     gosrs_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor);
     fprintf(ff_obor,"\
 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
     class iceb_u_str naimgrup("");       
     sprintf(strsql,"select naik from Gkont where kod=%d",grupa);
     if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
         naimgrup.new_plus(row1[0]);

     gosrs_ss(0,dn,mn,gn,dk,mk,gk,&kollist_obor,&kolstr_obor,ff_obor);

     sprintf(strsql,"%s %d %s",gettext(" Итого по группе"),grupa,naimgrup.ravno());
     fprintf(ff_obor,"\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
     iceb_u_kolbait(31,strsql),iceb_u_kolbait(31,strsql),strsql,dnsgr,knsgr,sdngr,skngr,sdgr,skgr);

     /*****************************************/

     gosrs_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur);
     fprintf(ff_gur,"\
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");


     sprintf(strsql,"%s %d %s",gettext(" Итого по группе"),grupa,naimgrup.ravno());

     gosrs_ss(1,dn,mn,gn,dk,mk,gk,&kollist_gur,&kolstr_gur,ff_gur);
     fprintf(ff_gur,"\
%-*.*s %14.2f %14.2f %10s %14.2f %14.2f %14.2f %14.2f\n",
     iceb_u_kolbait(31,strsql),iceb_u_kolbait(31,strsql),strsql,dnsgr,knsgr," ",sdngr,skngr,sdgr,skgr);

  }

fprintf(ff_obor,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
iceb_u_kolbait(31,gettext(" И т о г о")),iceb_u_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),
idebs,ikres,idebp,ikrep,idebk,ikrek);

fprintf(ff_obor,"\n%s__________________________\n",
gettext("Главный бухгалтер"));

iceb_podpis(ff_obor,data->window);
fclose(ff_obor);


fprintf(ff_gur,"\
-------------------------------------------------------------------------------------------------------------------------------------\n\
%-*.*s %14.2f %14.2f %10s %14.2f %14.2f %14.2f %14.2f\n",
iceb_u_kolbait(31,gettext(" И т о г о")),iceb_u_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),idebs,ikres," ",idebp,ikrep,idebk,ikrek);

gosrs_itoggo(data->rk->shet.ravno(),dn,mn,gn,dk,mk,gk,naim_shet.ravno(),&oss,&kontr_shet,&kontr_shet_deb,&kontr_shet_kre,ff_gur,data->window);

fprintf(ff_gur,"\n%s__________________________\n",
gettext("Главный бухгалтер"));
iceb_podpis(ff_gur,data->window);

fclose(ff_gur);

fprintf(ff_sk,"\
----------------------------------------------------------------------------------\n");
fprintf(ff_sk,"%-*.*s %14.2f %14.2f\n",
iceb_u_kolbait(31,gettext(" И т о г о")),iceb_u_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),idebk,ikrek);

fprintf(ff_sk,"\n%s__________________________\n",
gettext("Главный бухгалтер"));
iceb_podpis(ff_sk,data->window);
fclose(ff_sk);


/*Распечатка горизонтального отчёта*/
sprintf(strsql,"%s\n",gettext("Горизонтальна форма распечатки по контрагентам"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
char imaf_gor[50];

gosrs_goriz(data->rk->shet.ravno(),dn,mn,gn,dk,mk,gk,&skontr,&skontr_sort,&ssd,&ssk,&kontr_shet,&kontr_shet_deb,&kontr_shet_kre,&debs,&kres,&debp,&krep,imaf_gor,data->window);

/*Распечатка по дням*/
sprintf(strsql,"%s\n",gettext("Распечатка по дням"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

char imaf_dn[50];
gorsrs_podn(data->rk->shet.ravno(),naim_shet.ravno(),dn,mn,gn,dk,mk,gk,&ssd,&ssk,den_shet_deb,den_shet_kre,den_shet_deb_suma,den_shet_kre_suma,idebs,ikres,idebp,ikrep,idebk,ikrek,imaf_dn,data->window);


sprintf(strsql,"\n%*s:%20s",iceb_u_kolbait(18,gettext("Сальдо начальное")),gettext("Сальдо начальное"),iceb_u_prnbr(idebs));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql," %20s\n",iceb_u_prnbr(ikres));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%*s:%20s",iceb_u_kolbait(18,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(idebp));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql," %20s\n",iceb_u_prnbr(ikrep));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%*s:%20s",iceb_u_kolbait(18,gettext("Сальдо конечное")),gettext("Сальдо конечное"),iceb_u_prnbr(idebk));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql," %20s\n\n",iceb_u_prnbr(ikrek));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


data->rk->imaf.new_plus(imaf_obor);

sprintf(strsql,"%s %s",gettext("Оборотный баланс по счёту"),data->rk->shet.ravno());
data->rk->naimf.new_plus(strsql);

data->rk->imaf.plus(imaf_gur);
sprintf(strsql,"%s %s",gettext("Журнал-ордер по счёту"),data->rk->shet.ravno());
data->rk->naimf.plus(strsql);

data->rk->imaf.plus(imaf_sk);
sprintf(strsql,"%s %s",gettext("Конечное сальдо по счёту"),data->rk->shet.ravno());
data->rk->naimf.plus(strsql);

data->rk->imaf.plus(imaf_gor);
data->rk->naimf.plus(gettext("Горизонтальная форма распечатки по контрагентам"));

data->rk->imaf.plus(imaf_dn);
data->rk->naimf.plus(gettext("Распечатка по дням"));

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),3,data->window);





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
