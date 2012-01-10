/*$Id: gosssw_r.c,v 1.14 2011-02-21 07:35:51 sasa Exp $*/
/*09.10.2010	11.09.2009	Белых А.И.	gosssw_r.c
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

class gosssw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class go_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  gosssw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   gosssw_r_key_press(GtkWidget *widget,GdkEventKey *event,class gosssw_r_data *data);
gint gosssw_r1(class gosssw_r_data *data);
void  gosssw_r_v_knopka(GtkWidget *widget,class gosssw_r_data *data);

void buhmesshetw(class iceb_u_spisok *mes,class iceb_u_double *mes_shet,class iceb_u_spisok *sheta,FILE *ff);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int      kol_strok_na_liste;
extern short	startgodb; /*Стартовый год*/
extern short	vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/

int gosssw_r(class go_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class gosssw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт журнал-ордера по счёту"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(gosssw_r_key_press),&data);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(gosssw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)gosssw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  gosssw_r_v_knopka(GtkWidget *widget,class gosssw_r_data *data)
{
//printf("gosssw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   gosssw_r_key_press(GtkWidget *widget,GdkEventKey *event,class gosssw_r_data *data)
{
// printf("gosssw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}

/***********************************/
/*Распечатка свода по счетам корреспондентам*/
/*******************************************/

void gosss_rspsk(const char *shetg,
const char *naim_shet,
short dn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_spisok *mes_shet,
class iceb_u_double *mes_deb,
class iceb_u_double *mes_kre,
double debs,
double kres,
double debp,
double krep,
char *imaf,
GtkWidget *wpredok)
{
class iceb_u_spisok sheta;
class iceb_u_double deb;
class iceb_u_double kre;
char shet[32];
int nomer_sheta=0;
for(int nomer=0; nomer < mes_shet->kolih(); nomer++)
 {
  iceb_u_polen(mes_shet->ravno(nomer),shet,sizeof(shet),2,'|');
  if((nomer_sheta=sheta.find_r(shet)) < 0)
   sheta.plus(shet);
  deb.plus(mes_deb->ravno(nomer),nomer_sheta);
  kre.plus(mes_kre->ravno(nomer),nomer_sheta);
 }

sprintf(imaf,"spsk%d.lst",getpid());
FILE *ff;
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

iceb_u_zagolov(gettext("Журнал-ордер по счету"),dn,mn,gn,dk,mk,gk,organ,ff);
fprintf(ff,"\n%s:%s %s\n",gettext("Счет"),shetg,naim_shet);

fprintf(ff,"\
--------------------------------------------------------------\n");
fprintf(ff,gettext("\
  Счет  |     Наименование счета       |  Дебет   | Кредит   |\n"));
fprintf(ff,"\
--------------------------------------------------------------\n");

if(debs > kres)
 {
  fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(39,gettext("Сальдо начальное")),gettext("Сальдо начальное"),debs-kres);
 }
else
 {
  fprintf(ff,"%*s:%10s %10.2f\n",iceb_u_kolbait(39,gettext("Сальдо начальное")),gettext("Сальдо начальное"),"",kres-debs);
 }
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
char naimshet[512];
for(int nomer=0; nomer < sheta.kolih(); nomer++)
 {
  sprintf(strsql,"select nais from Plansh where ns='%s'",sheta.ravno(nomer));
  memset(naimshet,'\0',sizeof(naimshet));
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    strncpy(naimshet,row[0],sizeof(naimshet)-1);
  
  fprintf(ff,"%*s %-*.*s %10.2f %10.2f\n",
  iceb_u_kolbait(8,sheta.ravno(nomer)),sheta.ravno(nomer),
  iceb_u_kolbait(30,naimshet),iceb_u_kolbait(30,naimshet),naimshet,
  deb.ravno(nomer),kre.ravno(nomer));      
  
  if(iceb_u_strlen(naimshet) > 30)
   fprintf(ff,"%8s %s\n","",iceb_u_adrsimv(30,naimshet));
 }

fprintf(ff,"\
--------------------------------------------------------------\n");

fprintf(ff,"%*s:%10.2f %10.2f\n",
iceb_u_kolbait(39,gettext("Оборот за период")),
gettext("Оборот за период"),
debp,krep);

fprintf(ff,"%*s:%10.2f %10.2f\n",
iceb_u_kolbait(39,gettext("Сальдо развернутое")),gettext("Сальдо развернутое"),
debp,krep);

if(debs+debp > kres+krep)
 {
  fprintf(ff,"%*s:%10.2f\n",
  iceb_u_kolbait(39,gettext("Сальдо свернутое")),
  gettext("Сальдо свернутое"),debs+debp-kres-krep);
 }  
else
 {
  fprintf(ff,"%*s:%10s %10.2f\n",iceb_u_kolbait(39,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",kres+krep-debs-debp);
 }

fprintf(ff,"\n%s_______________________\n",gettext("Главный бухгалтер"));


iceb_podpis(ff,wpredok);

fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/

fclose(ff);

}
/*****************/
/*Шапка рапечатки*/
/*****************/

void gosss_sap_prov(int *nomlist,int *kolstrok,FILE *ff)
{

*nomlist+=1;
fprintf(ff,"%90s %s N%d\n"," ",gettext("Лист"),*nomlist);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("  Дата    |Сч.к-т|Конт.| Кто   |Номер док.|   Дебет  |  Кредит  |      К о м е н т а р и й       |Наименование контрагента|\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
*kolstrok+=4;

}

/***************/
/*Счетчик строк*/
/***************/
void gosss_shet_prov(int *kolstrok,int *nomlist,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok <= kol_strok_na_liste)
 return;

*kolstrok=1;
fprintf(ff,"\f");
gosss_sap_prov(nomlist,kolstrok,ff);

}

/***************/
/*Выдача итога*/
/**************/
void gosss_itogoprov(double debs,double kres,
double debp,double krep,
double dendebet,double denkredit,
int metka, //0-без дневного оборота
int *kolstrok,int *nomlist,
FILE *ff)
{

gosss_shet_prov(kolstrok,nomlist,ff);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
if(metka == 1)
 {
  gosss_shet_prov(kolstrok,nomlist,ff);
  fprintf(ff,"%*s:%10.2f %10.2f\n",iceb_u_kolbait(42,gettext("Итого за день")),gettext("Итого за день"),
  dendebet,denkredit);
 }
gosss_shet_prov(kolstrok,nomlist,ff);

fprintf(ff,"%*s:%10.2f %10.2f\n",iceb_u_kolbait(42,gettext("Оборот за период")),gettext("Оборот за период"),debp,krep);

double debet=debs+debp;
double kredit=kres+krep;

gosss_shet_prov(kolstrok,nomlist,ff);

fprintf(ff,"%*s:%10.2f %10.2f\n",
iceb_u_kolbait(42,gettext("Сальдо развернутое")),gettext("Сальдо развернутое"),debet,kredit);

gosss_shet_prov(kolstrok,nomlist,ff);
if(debet > kredit)
 {
  fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(42,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),debet-kredit);
 }  
else
 {
  fprintf(ff,"%*s:%10s %10.2f\n",iceb_u_kolbait(42,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",kredit-debet);
 }
gosss_shet_prov(kolstrok,nomlist,ff);
fprintf(ff,"\n");

}


/******************************************/
/******************************************/

gint gosssw_r1(class gosssw_r_data *data)
{
time_t vremn;
time(&vremn);
char shet_s_mdkz[32]; /*Cчёт с максимально-допустимым количеством знаков*/
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

short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int kolstr=0;
double debs=0.;
double kres=0.;
double debp=0.;
double krep=0.;


/*Узнаем начальное сальдо по счету*/

/*Отчет для многопорядкового плана счетов*/
if(vplsh == 1)
 sprintf(strsql,"select ns,deb,kre from Saldo where kkk='0' and gs=%d and \
ns like '%s%%'",godn,data->rk->shet.ravno());

/*Отчет по субсчету двух-порядковый план счетов*/
if(data->rk->vds == 1 && vplsh == 0)
 sprintf(strsql,"select ns,deb,kre from Saldo where kkk='0' and gs=%d and \
ns='%s'",godn,data->rk->shet.ravno());

/*Отчет по счету двух-порядковый план счетов*/
if(data->rk->vds == 0 && vplsh == 0)
 sprintf(strsql,"select ns,deb,kre from Saldo where kkk='0' and gs=%d and \
ns like '%s%%'",godn,data->rk->shet.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
while(cur.read_cursor(&row) != 0)
 {
  debs+=atof(row[1]);
  kres+=atof(row[2]);
 }


sprintf(strsql,"%s %.2f %.2f %d\n",gettext("Стартовое сальдо"),debs,kres,godn);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

char imaf_prov[50];
sprintf(imaf_prov,"goprov%d.lst",getpid());
FILE *ff_prov;
if((ff_prov=fopen(imaf_prov,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prov,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_prov_iden[50];
sprintf(imaf_prov_iden,"goprovi%d.lst",getpid());
FILE *ff_prov_iden;
if((ff_prov_iden=fopen(imaf_prov_iden,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prov_iden,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Расчет журнала-ордера по счету"),dn,mn,gn,dk,mk,gk,organ,ff_prov);
iceb_u_zagolov(gettext("Расчет журнала-ордера по счету"),dn,mn,gn,dk,mk,gk,organ,ff_prov_iden);

fprintf(ff_prov,"\n%s:%s %s\n",gettext("Счет"),data->rk->shet.ravno(),data->rk->naimshet.ravno());
fprintf(ff_prov_iden,"\n%s:%s %s\n",gettext("Счет"),data->rk->shet.ravno(),data->rk->naimshet.ravno());

int kolstr_prov_iden=5+2;
int kolstr_prov=5+2;
int kollist_prov=0;
int kollist_prov_iden=0;

gosss_sap_prov(&kollist_prov,&kolstr_prov,ff_prov);
gosss_sap_prov(&kollist_prov_iden,&kolstr_prov_iden,ff_prov_iden);

class iceb_u_spisok sheta_deb;
class iceb_u_spisok sheta_kre;

int kolih_mes_v_per=iceb_u_period(dn,mn,gn,dk,mk,gk,1);

class iceb_u_spisok den_shet[kolih_mes_v_per];
class iceb_u_double den_shet_deb[kolih_mes_v_per];
class iceb_u_double den_shet_kre[kolih_mes_v_per];


double deb=0.;
double kre=0.;
float kolstr1=0;
int kolih_den=iceb_u_period(1,1,godn,dk,mk,gk,0);
int nomer_den_shet=0;
int nomer_mes_shet=0;
int nomer_mes_v_per=-1;

char koment[512];
char naikon[512];

short d=1,m=1,g=godn;
int metka_perioda=0;
double ideb_den=0.;
double ikre_den=0.;
short den_zap=0;
short mes_zap=0;
char shet_zap[20];

sprintf(strsql,"%s %s\n",gettext("Реестр проводок по счёту"),data->rk->shet.ravno());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
//  strzag(LINES-1,0,kolih_den,++kolstr1);
  iceb_pbar(data->bar,kolih_den,++kolstr1);    
  
  
  //Для многопорядкового плана счетов
  if(vplsh == 1)
    sprintf(strsql,"select shk,kodkon,kto,nomd,oper,deb,kre,ktoi,komen from Prov \
where val=0 and datp = '%04d-%02d-%02d' and sh like '%s%%' order by shk asc",g,m,d,data->rk->shet.ravno());

  /*Отчет по субсчету для двухпорядкового плана счетов*/
  if(data->rk->vds == 1 && vplsh == 0)
    sprintf(strsql,"select shk,kodkon,kto,nomd,oper,deb,kre,ktoi,komen from Prov \
where val=0 and datp = '%04d-%02d-%02d' and sh='%s' order by shk asc",g,m,d,data->rk->shet.ravno());

  /*Отчет по счету для двухпорядкового плана счетов*/
  if(data->rk->vds == 0 && vplsh == 0)
    sprintf(strsql,"select shk,kodkon,kto,nomd,oper,deb,kre,ktoi,komen from Prov \
where val=0 and datp = '%04d-%02d-%02d' and sh like '%s%%' order by shk asc",g,m,d,data->rk->shet.ravno());


  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(1);
   }

  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0)
  if(mes_zap != m)
   {
    nomer_mes_v_per++;
    mes_zap=m;
   }            


  memset(shet_zap,'\0',sizeof(shet_zap));
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    deb=atof(row[5]);
    kre=atof(row[6]);
    
    if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0) /*До периода*/
     {
      debs+=deb;
      kres+=kre;
     }
    else /*Период*/
     {
      if(metka_perioda == 0)
       {
        gosss_shet_prov(&kolstr_prov,&kollist_prov,ff_prov);
        gosss_shet_prov(&kolstr_prov_iden,&kollist_prov_iden,ff_prov_iden);
        if(debs > kres)
         {
          fprintf(ff_prov,"%*s:%10.2f\n",
          iceb_u_kolbait(42,gettext("Сальдо начальное")),
          gettext("Сальдо начальное"),debs-kres);

          fprintf(ff_prov_iden,"%*s:%10.2f\n",
          iceb_u_kolbait(42,gettext("Сальдо начальное")),
          gettext("Сальдо начальное"),debs-kres);
         }
        else
         {
          fprintf(ff_prov,"%*s:%10s %10.2f\n",
          iceb_u_kolbait(42,gettext("Сальдо начальное")),
          gettext("Сальдо начальное")," ",kres-debs);

          fprintf(ff_prov_iden,"%*s:%10s %10.2f\n",
          iceb_u_kolbait(42,gettext("Сальдо начальное")),
          gettext("Сальдо начальное")," ",kres-debs);
         }        
        metka_perioda=1;
       }

      if(den_zap != d)
       {
        if(den_zap != 0)
         gosss_itogoprov(debs,kres,debp,krep,ideb_den,ikre_den,1,&kolstr_prov_iden,&kollist_prov_iden,ff_prov_iden);
        ideb_den=ikre_den=0.;
        den_zap=d;
       }


      debp+=deb;
      krep+=kre;
      ideb_den+=deb;
      ikre_den+=kre;
      if(data->rk->kolih_znak == 0)
       strncpy(shet_s_mdkz,row[0],sizeof(shet_s_mdkz));
      else
       sprintf(shet_s_mdkz,"%.*s",
       iceb_u_kolbait(data->rk->kolih_znak,row[0]),
       row[0]);

      sprintf(strsql,"%02d.%02d.%04d|%s",d,m,g,shet_s_mdkz);

      if(iceb_u_srav_r(shet_zap,shet_s_mdkz,0) != 0) /*День не меняется*/
       {
        if((nomer_den_shet=den_shet[nomer_mes_v_per].find_r(strsql)) < 0)
         den_shet[nomer_mes_v_per].plus(strsql);

        den_shet_deb[nomer_mes_v_per].plus(deb,nomer_den_shet);      
        den_shet_kre[nomer_mes_v_per].plus(kre,nomer_den_shet);      
        if(nomer_den_shet < 0) /*Вообщето такого не может быть*/
         nomer_den_shet=den_shet[nomer_mes_v_per].kolih()-1; /*Нужен номер куда записало для дальнейшего использования*/
        strcpy(shet_zap,shet_s_mdkz);
       }
      else
       {
        den_shet_deb[nomer_mes_v_per].plus(deb,nomer_den_shet);      
        den_shet_kre[nomer_mes_v_per].plus(kre,nomer_den_shet);      
       }
       


      if(fabs(deb) > 0.009)
       {
        if(sheta_deb.find_r(shet_s_mdkz,data->rk->metka_r) < 0)
         sheta_deb.plus(shet_s_mdkz);
       }
      else
       if(sheta_kre.find_r(shet_s_mdkz,data->rk->metka_r) < 0)
        sheta_kre.plus(shet_s_mdkz);

      memset(naikon,'\0',sizeof(naikon));;
      if(row[1][0] != '\0')
       {  
        sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[1]);
        if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
         strncpy(naikon,row1[0],sizeof(naikon)-1);
       }       

      sozkom(koment,sizeof(koment),row[8],row[2],row[4]);

      gosss_shet_prov(&kolstr_prov,&kollist_prov,ff_prov);

      fprintf(ff_prov,"%02d.%02d.%4d %-*s %-*s %-3s%4s %-*s %10s",
      d,m,g,
      iceb_u_kolbait(6,row[0]),row[0],
      iceb_u_kolbait(5,row[1]),row[1],
      row[2],
      row[7],
      iceb_u_kolbait(10,row[3]),row[3],
      iceb_u_prnbr(deb));

      fprintf(ff_prov," %10s %s %-32s %s\n",iceb_u_prnbr(kre),row[4],koment,naikon);

      gosss_shet_prov(&kolstr_prov_iden,&kollist_prov_iden,ff_prov_iden);

      fprintf(ff_prov_iden,"%02d.%02d.%4d %-*s %-*s %-3s%4s %-*s %10s",
      d,m,g,
      iceb_u_kolbait(6,row[0]),row[0],
      iceb_u_kolbait(5,row[1]),row[1],
      row[2],
      row[7],
      iceb_u_kolbait(10,row[3]),row[3],
      iceb_u_prnbr(deb));
      
      fprintf(ff_prov_iden," %10s %s %-*s %s\n",
      iceb_u_prnbr(kre),
      row[4],
      iceb_u_kolbait(32,koment),koment,
      naikon);
     }
     
   }



  iceb_u_dpm(&d,&m,&g,1);
 }


gosss_itogoprov(debs,kres,debp,krep,0.,0.,0,&kolstr_prov,&kollist_prov,ff_prov);
gosss_itogoprov(debs,kres,debp,krep,ideb_den,ikre_den,1,&kolstr_prov_iden,&kollist_prov_iden,ff_prov_iden);

fprintf(ff_prov,"\n%s_______________________\n",gettext("Главный бухгалтер"));
iceb_podpis(ff_prov,data->window);
fclose(ff_prov);

fprintf(ff_prov_iden,"\n%s___________________\n",gettext("Главный бухгалтер"));
iceb_podpis(ff_prov_iden,data->window);
fclose(ff_prov_iden);


int kolih_deb=sheta_deb.kolih();
int kolih_kre=sheta_kre.kolih();


class iceb_u_spisok den_shet_period;
d=dn;
m=mn;
g=gn;
while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  sprintf(strsql,"%02d.%02d.%04d",d,m,g);
  den_shet_period.plus(strsql);
  iceb_u_dpm(&d,&m,&g,1);
 }  

kolih_den=den_shet_period.kolih();

class iceb_u_double den_shet_deb_suma;
class iceb_u_double den_shet_kre_suma;

den_shet_deb_suma.make_class(kolih_den*kolih_deb);
den_shet_kre_suma.make_class(kolih_den*kolih_kre);


int nomer_den_shet_period=0;
int nomer_sheta=0;
char datad[20];
char shet_d[20];
char messhet[32];
class iceb_u_spisok mes_shet;
class iceb_u_double mes_shet_deb;
class iceb_u_double mes_shet_kre;

for(nomer_mes_v_per=0; nomer_mes_v_per < kolih_mes_v_per; nomer_mes_v_per++)
for(int nomer_den_shet=0; nomer_den_shet < den_shet[nomer_mes_v_per].kolih(); nomer_den_shet++)
 {
  iceb_u_polen(den_shet[nomer_mes_v_per].ravno(nomer_den_shet),datad,sizeof(datad),1,'|');    
  iceb_u_polen(den_shet[nomer_mes_v_per].ravno(nomer_den_shet),shet_d,sizeof(shet_d),2,'|');    

  nomer_den_shet_period=den_shet_period.find(datad);

  deb=den_shet_deb[nomer_mes_v_per].ravno(nomer_den_shet);
  kre=den_shet_kre[nomer_mes_v_per].ravno(nomer_den_shet);

  if((nomer_sheta=sheta_deb.find_r(shet_d)) >= 0)  
    den_shet_deb_suma.plus(deb,nomer_den_shet_period*kolih_deb+nomer_sheta);
  if((nomer_sheta=sheta_kre.find_r(shet_d)) >= 0)  
    den_shet_kre_suma.plus(kre,nomer_den_shet_period*kolih_kre+nomer_sheta);

   memset(messhet,'\0',sizeof(messhet));
   strcpy(messhet,&datad[3]);

   
   sprintf(strsql,"%s|%s",messhet,shet_d);
   if((nomer_mes_shet=mes_shet.find_r(strsql)) < 0)
     mes_shet.plus(strsql);

   mes_shet_deb.plus(deb,nomer_mes_shet);      
   mes_shet_kre.plus(kre,nomer_mes_shet);      
 }


int max_dlinna=(kolih_deb+2)*11;
int dlinna_kre=(kolih_kre+2)*11;
if(max_dlinna < dlinna_kre)
 max_dlinna=dlinna_kre;

FILE *ff_dni;
char imaf_dni[50];
sprintf(imaf_dni,"gpd%d.lst",getpid());
if((ff_dni = fopen(imaf_dni,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_dni,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
fprintf(ff_dni,"\x1B%s\x1E\n","3"); /*Изменение межстрочного интервала*/

sprintf(strsql,"%s\n",gettext("Распечатка по дням"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


struct  tm      *bf;
time_t vr=time(NULL);
bf=localtime(&vr);

fprintf(ff_dni,"%s\n%s %s %s\n",organ,
gettext("Журнал-ордер по счёту"),data->rk->shet.ravno(),data->rk->naimshet.ravno());

sprintf(strsql,"%s %s %s %d.%d.%d%s %s: %d:%d",
gettext("Оборот по дебету счета"),data->rk->shet.ravno(),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);



buhdenshetw(strsql,&den_shet_period,&sheta_deb,&den_shet_deb_suma,ff_dni);
iceb_podpis(ff_dni,data->window);

fprintf(ff_dni,"\f");

fprintf(ff_dni,"%s\n%s %s %s\n",organ,
gettext("Журнал-ордер по счёту"),data->rk->shet.ravno(),data->rk->naimshet.ravno());

sprintf(strsql,"%s %s %s %d.%d.%d%s %s: %d:%d",
gettext("Оборот по кредиту счета"),data->rk->shet.ravno(),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

buhdenshetw(strsql,&den_shet_period,&sheta_kre,&den_shet_kre_suma,ff_dni);


fprintf(ff_dni,"\
                           %s           %s\n",
gettext("Дебет"),
gettext("Кредит"));

int kolb=iceb_u_kolbait(18,gettext("Сальдо начальное"));

if(debs >= kres)
 fprintf(ff_dni,"\
%*s:%15.2f\n",kolb,gettext("Сальдо начальное"),debs-kres);
else
  fprintf(ff_dni,"\
%*s:%15s  %15.2f\n",kolb,gettext("Сальдо начальное")," ",kres-debs);

kolb=iceb_u_kolbait(18,gettext("Оборот за период"));
fprintf(ff_dni,"\
%*s:%15.2f  %15.2f\n",kolb,gettext("Оборот за период"),debp,krep);

kolb=iceb_u_kolbait(18,gettext("Сальдо конечное"));
if(debs+debp >=  kres+krep)
  fprintf(ff_dni,"\
%*s:%15.2f\n",kolb,gettext("Сальдо конечное"),(debs+debp)-(kres+krep));
else
  fprintf(ff_dni,"\
%*s:%15s  %15.2f\n",kolb,gettext("Сальдо конечное")," ",(kres+krep)-(debs+debp));

fprintf(ff_dni,"\n%s________________  %s________________\n",
gettext("Главный бухгалтер"),
gettext("Исполнитель"));


fclose(ff_dni);


/*Распечатка месячных итогов*/
/******************************************************************/


class iceb_u_spisok mes_shet_period;
d=dn;
m=mn;
g=gn;
while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  sprintf(strsql,"%02d.%04d",m,g);
  mes_shet_period.plus(strsql);
  iceb_u_dpm(&d,&m,&g,3);
 }  


class iceb_u_double mes_shet_deb_suma;
class iceb_u_double mes_shet_kre_suma;

int kolih_mes=mes_shet_period.kolih();
mes_shet_deb_suma.make_class(kolih_mes*kolih_deb);
mes_shet_kre_suma.make_class(kolih_mes*kolih_kre);
int nomer_mes_shet_period=0;

for(int nomer_mes_shet=0; nomer_mes_shet < mes_shet.kolih(); nomer_mes_shet++)
 {
  iceb_u_polen(mes_shet.ravno(nomer_mes_shet),datad,sizeof(datad),1,'|');    
  iceb_u_polen(mes_shet.ravno(nomer_mes_shet),shet_d,sizeof(shet_d),2,'|');    

  nomer_mes_shet_period=mes_shet_period.find(datad);
  
  if((nomer_sheta=sheta_deb.find_r(shet_d)) >= 0)  
    mes_shet_deb_suma.plus(mes_shet_deb.ravno(nomer_mes_shet),nomer_mes_shet_period*kolih_deb+nomer_sheta);
  if((nomer_sheta=sheta_kre.find_r(shet_d)) >= 0)  
    mes_shet_kre_suma.plus(mes_shet_kre.ravno(nomer_mes_shet),nomer_mes_shet_period*kolih_kre+nomer_sheta);
 }


max_dlinna=(kolih_deb+1)*11+8;
dlinna_kre=(kolih_kre+1)*11+8;
if(max_dlinna < dlinna_kre)
 max_dlinna=dlinna_kre;

FILE *ff_mes;
char imaf_mes[56];
sprintf(imaf_mes,"gpdm%d.lst",getpid());
if((ff_mes = fopen(imaf_mes,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_mes,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
fprintf(ff_mes,"\x1B%s\x1E\n","3"); /*Изменение межстрочного интервала*/

sprintf(strsql,"%s %s\n",gettext("Месячные итоги по счёту"),data->rk->shet.ravno());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


fprintf(ff_mes,"%s\n%s %s %s\n",organ,
gettext("Месячные итоги по счёту"),data->rk->shet.ravno(),data->rk->naimshet.ravno());

fprintf(ff_mes,"%s %s ",gettext("Оборот по дебету"),data->rk->shet.ravno());

fprintf(ff_mes,"%s %d.%d.%d%s %s: %d:%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);



buhmesshetw(&mes_shet_period,&mes_shet_deb_suma,&sheta_deb,ff_mes);

fprintf(ff_mes,"\n%s %s \n",gettext("Оборот по кредиту"),data->rk->shet.ravno());

buhmesshetw(&mes_shet_period,&mes_shet_kre_suma,&sheta_kre,ff_mes);


iceb_podpis(ff_mes,data->window);
fclose(ff_mes);

sprintf(strsql,"%s %s\n",gettext("Итоги по счетам корреспондентам для счёта"),data->rk->shet.ravno());
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
char imaf_shk[56];
gosss_rspsk(data->rk->shet.ravno(),data->rk->naimshet.ravno(),dn,mn,gn,dk,mk,gk,&mes_shet,&mes_shet_deb,&mes_shet_kre,debs,kres,debp,krep,imaf_shk,data->window);




/*На экран тоже самое*/

sprintf(strsql,"\
                               %s           %s\n",
gettext("Дебет"),
gettext("Кредит"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
char bros[512];
sprintf(bros,"%s %02d.%02d.%d",gettext("Сальдо на"),dn,mn,gn);
int dlina=iceb_u_strlen(bros);
deb=kre=0.;
if(debs >= kres)
 {
  deb=debs-kres;
  sprintf(strsql,"%s:%15s\n",bros,iceb_u_prnbr(deb));
 }
else
 {
  kre=kres-debs;
  sprintf(strsql,"%s:%15s  %15s\n",bros," ",iceb_u_prnbr(kre));
 }
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%*s:%15s",iceb_u_kolbait(dlina,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(debp));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"  %15s\n",iceb_u_prnbr(krep));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%*s:%15s",iceb_u_kolbait(dlina,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),iceb_u_prnbr(deb+debp));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"  %15s\n",iceb_u_prnbr(kre+krep));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(bros,"%s %02d.%02d.%d",gettext("Сальдо на"),dk,mk,gk);
if(debs+debp >=  kres+krep)
 {
  deb=(debs+debp)-(kres+krep);
  sprintf(strsql,"%s:%15s\n",bros,iceb_u_prnbr(deb));
 }
else
 {
  kre=(kres+krep)-(debs+debp);
  sprintf(strsql,"%s:%15s  %15s\n",bros," ",iceb_u_prnbr(kre));
 }
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


data->rk->imaf.new_plus(imaf_dni);
sprintf(strsql,"%s %s",gettext("Журнал-ордер по счёту"),data->rk->shet.ravno());
data->rk->naimf.new_plus(strsql);

data->rk->imaf.plus(imaf_mes);
sprintf(strsql,"%s %s",gettext("Месячные итоги по счёту"),data->rk->shet.ravno());
data->rk->naimf.plus(strsql);

data->rk->imaf.plus(imaf_shk);
sprintf(strsql,"%s %s",gettext("Итоги по счетам корреспондентам для счёта"),data->rk->shet.ravno());
data->rk->naimf.plus(strsql);

data->rk->imaf.plus(imaf_prov);
sprintf(strsql,"%s %s",gettext("Реестр проводок по счёту"),data->rk->shet.ravno());
data->rk->naimf.plus(strsql);

data->rk->imaf.plus(imaf_prov_iden);
sprintf(strsql,"%s %s",gettext("Реестр проводок с дневными итогами по счёту"),data->rk->shet.ravno());
data->rk->naimf.plus(strsql);



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
