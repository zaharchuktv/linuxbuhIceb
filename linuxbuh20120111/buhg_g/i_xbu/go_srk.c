/*$Id: go_srk.c,v 1.28 2011-08-29 07:13:43 sasa Exp $*/
/*29.07.2011	25.03.2004	Белых А.И.	go_srk.c
Распечатка проводок по контрагентам
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "go.h"

class go_srk_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество выполненных проходов
  int    kolstr;  //Количество проходов при расчете


  class go_rr *rek_r;
  short dn,mn,gn;
  short dk,mk,gk;
  short  godn;
  time_t vremn;
  
  FILE      *ff;
  int kolstlst;
  int kollist;
  int metkasort;
  iceb_u_spisok SHET;
  iceb_u_double DEB;
  iceb_u_double  KRE;
  int voz;
  short metka_kr; //0-расчёт завершён
  //Конструктор  
  go_srk_r_data()
   {
    voz=1;
    metka_kr=1;
    kolstr1=0.;
    kolstr=0;
    kolstlst=0;
    kollist=0;
    metkasort=0;
   }

 };

gboolean   go_srk_r_key_press(GtkWidget *widget,GdkEventKey *event,class go_srk_r_data *data);
gint go_srk_r1(class go_srk_r_data *data);
void  go_srk_r_v_knopka(GtkWidget *widget,class go_srk_r_data *data);

void numlist(int *kolstrlist,int *kollist,FILE *ff);
void	prosmotrdo(short,short,short,const char*,const char*,short,double*,double*,
short*,short*,short*,double*,double*,short);

extern SQL_baza bd;
extern char *organ;
extern char *name_system;
extern short vplsh; /*0-двух порядковый план счетов 1-многпорядковый*/
extern short	startgodb; /*Стартовый год*/
extern short    koolk; /*Корректор отступа от левого края*/
extern int kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;

int go_srk(class go_rr *data_rr,int metkasort)
{
go_srk_r_data data;

data.rek_r=data_rr;
data.metkasort=metkasort;

char strsql[512];
iceb_u_str soob;



printf("%s\n",__FUNCTION__);
if(iceb_rsdatp(&data.dn,&data.mn,&data.gn,data.rek_r->datan.ravno(),
&data.dk,&data.mk,&data.gk,data.rek_r->datak.ravno(),NULL) != 0)
  return(1);
  
data.godn=startgodb;
if(startgodb == 0 || startgodb > data.gn)
 data.godn=data.gn;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Журнал ордер"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(go_srk_r_key_press),&data);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

sprintf(strsql,"%s %s",gettext("Расчет журнал-ордера по счету"),data.rek_r->shet.ravno());
soob.new_plus(strsql);

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d",gettext("Расчет за период"),
data.dn,data.mn,data.gn,
data.dk,data.mk,data.gk);
soob.ps_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),400,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(go_srk_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)go_srk_r1,&data);

gtk_main();

//iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);

return(data.voz);



}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  go_srk_r_v_knopka(GtkWidget *widget,class go_srk_r_data *data)
{
// printf("go_srk_r_v_knopka\n");
if(data->metka_kr != 0)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   go_srk_r_key_press(GtkWidget *widget,GdkEventKey *event,class go_srk_r_data *data)
{
// printf("go_srk_r_key_press\n");
 
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

gint go_srk_r1(class go_srk_r_data *data)
{
char strsql[512];
iceb_u_str soob;
class SQLCURSOR cur,cur1;
SQL_str row,row1;


time(&data->vremn);

class iceb_u_spisok skontr; /*Список контрагентов по которым были проводки от стартового года до конца периода*/
if(data->rek_r->vds == 0 || vplsh == 1)
 sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d \
and ns like '%s%%'",data->godn,data->rek_r->shet.ravno());

if(data->rek_r->vds == 1 && vplsh == 0)
 sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d \
and ns='%s'",data->godn,data->rek_r->shet.ravno());

//printw("%s\n",strsql);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(skontr.find_r(row[0]) < 0)
    skontr.plus(row[0]);   

 }

short d=1;
short m=1;
short g=data->godn;
int kolih_den=iceb_u_period(1,1,data->godn,data->dk,data->mk,data->gk,0);
float kolstr1=0;
while(iceb_u_sravmydat(d,m,g,data->dk,data->mk,data->gk) <= 0)
 {
  
//  strzag(LINES-1,0,kolih_den,++kolstr1);
  iceb_pbar(data->bar,kolih_den,++kolstr1);    

  if(data->rek_r->vds == 0 || vplsh == 1)
   sprintf(strsql,"select distinct kodkon from Prov \
 where val=0 and datp='%d-%02d-%02d' and sh like '%s%%'",g,m,d,data->rek_r->shet.ravno());

  if(data->rek_r->vds == 1 && vplsh == 0)
   sprintf(strsql,"select distinct kodkon from Prov \
 where val=0 and datp='%d-%02d-%02d' and sh='%s'",g,m,d,data->rek_r->shet.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(1);
   }

  
  while(cur.read_cursor(&row) != 0)
   {
    if(row[0][0] == '\0')
     {
      iceb_menu_soob(gettext("В проводке нет кода контрагента !"),data->window);
      continue;
     }

    if(skontr.find_r(row[0]) < 0)
      skontr.plus(row[0]);

   }

  iceb_u_dpm(&d,&m,&g,1);
 }

if(iceb_sortkk(data->metkasort,&skontr,NULL,data->window) != 0)
 return(1);

char imaf[30];

sprintf(imaf,"gokp%d.lst",getpid());

sprintf(strsql,"%s %s",gettext("Распечатка проводок по контрагентах"),data->rek_r->shet.ravno());
data->rek_r->naimf.new_plus(strsql);

data->rek_r->imaf.new_plus(imaf);

if((data->ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
  data->metka_kr=0;
  return(FALSE);
 }



iceb_u_zagolov(gettext("Распечатка проводок по контрагентам"),
data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,organ,data->ff);

fprintf(data->ff,"%s: %s %s\n",gettext("Счет"),
data->rek_r->shet.ravno(),data->rek_r->naimshet.ravno());
data->kolstlst=8;
numlist(&data->kolstlst,&data->kollist,data->ff);


double  debet=0.,kredit=0.;
double  stdebet=0.,stkredit=0.;
double  dodebet=0.,dokredit=0.;
double  prdebet=0.,prkredit=0.;
class iceb_u_str naimkontr("");
short   dpp,mpp,gpp;
double  debpp,krepp;
int     nomer=0;
char    koment[512];
int kolih_kontr=skontr.kolih();
kolstr1=0.;
//while(cur.read_cursor(&row) != 0)
for(int nomer_kontr=0; nomer_kontr < kolih_kontr; nomer_kontr++)
 {
  iceb_pbar(data->bar,kolih_kontr,++kolstr1);    

  //Читаем наименование контрагента
  sprintf(strsql,"select naikon,grup from Kontragent where kodkon='%s'",skontr.ravno(nomer_kontr));   
  naimkontr.new_plus("");
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    naimkontr.new_plus(row1[0]);

  sprintf(strsql,"%-5s %s\n",skontr.ravno(nomer_kontr),naimkontr.ravno());  
  iceb_printw(strsql,data->buffer,data->view);


  if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row1[1],0,0) != 0)
    continue;
    
  stdebet=stkredit=0.;
  dodebet=dokredit=0.;
  prdebet=prkredit=0.;

  //Узнаем стартовое сальдо по контрагенту
  /*Отчет для многопорядкового плана счетов*/
  if(vplsh == 1)
   sprintf(strsql,"select deb,kre from Saldo where kkk='1' and ns like '%s%%' \
  and gs=%d and kodkon='%s'",
   data->rek_r->shet.ravno(),data->godn,skontr.ravno(nomer_kontr));

  /*Отчет по субсчету двух-порядковый план счетов*/
  if(data->rek_r->vds == 1 && vplsh == 0)
   sprintf(strsql,"select deb,kre from Saldo where kkk='1' and ns='%s' \
and gs=%d and kodkon='%s'",
   data->rek_r->shet.ravno(),data->godn,skontr.ravno(nomer_kontr));

  /*Отчет по счету двух-порядковый план счетов*/
  if(data->rek_r->vds == 0 && vplsh == 0)
   sprintf(strsql,"select deb,kre from Saldo where kkk='1' and ns like '%s%%' \
and gs=%d and kodkon='%s'",
   data->rek_r->shet.ravno(),data->godn,skontr.ravno(nomer_kontr));
  SQLCURSOR cur1;
  if(cur1.make_cursor(&bd,strsql) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  while(cur1.read_cursor(&row1) != 0)
   {
    stdebet+=atof(row1[0]);
    stkredit+=atof(row1[1]);
   }  

  prosmotrdo(data->dn,data->mn,data->gn,data->rek_r->shet.ravno(),skontr.ravno(nomer_kontr),data->rek_r->vds,
  &dodebet,&dokredit,&dpp,&mpp,&gpp,&debpp,&krepp,data->godn);

  //Смотрим проводки за период
  /*Отчет для многопорядкового плана счетов*/
  if(vplsh == 1)
    sprintf(strsql,"select datp,shk,kto,nomd,oper,deb,kre,ktoi,komen from Prov where val=0 and \
datp >= '%04d-%d-%d' and datp <= '%04d-%d-%d' and sh like '%s%%' and kodkon='%s' order by datp asc",
    data->gn,data->mn,data->dn,data->gk,data->mk,data->dk,data->rek_r->shet.ravno(),skontr.ravno(nomer_kontr));

  /*Отчет по субсчету двух-порядковый план счетов*/
  if(data->rek_r->vds == 1 && vplsh == 0)
    sprintf(strsql,"select datp,shk,kto,nomd,oper,deb,kre,ktoi,komen from Prov where val=0 and \
datp >= '%04d-%d-%d' and datp <= '%04d-%d-%d' and sh='%s' and kodkon='%s' order by datp asc",
    data->gn,data->mn,data->dn,data->gk,data->mk,data->dk,data->rek_r->shet.ravno(),skontr.ravno(nomer_kontr));

  /*Отчет по счету двух-порядковый план счетов*/
  if(data->rek_r->vds == 0 && vplsh == 0)
    sprintf(strsql,"select datp,shk,kto,nomd,oper,deb,kre,ktoi,komen from Prov where val=0 and \
datp >= '%04d-%d-%d' and datp <= '%04d-%d-%d' and sh like '%s%%' and kodkon='%s' order by datp asc",
    data->gn,data->mn,data->dn,data->gk,data->mk,data->dk,data->rek_r->shet.ravno(),skontr.ravno(nomer_kontr));
  int kolstr2;
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  debet=stdebet+dodebet;
  kredit=stkredit+dokredit;

  if(kolstr2 == 0)
   if(fabs(debet - kredit) < 0.01)
     continue;

  data->kolstlst+=2;    
  if(data->kolstlst > kol_strok_na_liste-6)
   {
    numlist(&data->kolstlst,&data->kollist,data->ff);
    data->kolstlst++; //Единичка назначена в подпрограме
   }
  fprintf(data->ff,"\n%s %s %s %02d.%02d.%d => %02d.%02d.%d\n",
  skontr.ravno(nomer_kontr),naimkontr.ravno(),
  gettext("Период с"),
  data->dn,data->mn,data->gn,data->dk,data->mk,data->gk);

  data->kolstlst+=3;    
  fprintf(data->ff,"\
-----------------------------------------------------------------------------\n");
    fprintf(data->ff,gettext("\
  Дата    |Счет кор|  Дебет   |  Кредит  |КТО ввел|Номер д.|Код оп.|Коментарий\n"));
    fprintf(data->ff,"\
-----------------------------------------------------------------------------\n");

  data->kolstlst++;    

  if(data->kolstlst > kol_strok_na_liste)
    numlist(&data->kolstlst,&data->kollist,data->ff);

  memset(strsql,'\0',sizeof(strsql));
  if(dpp != 0)
   {
    if(debpp > 0.009)
     sprintf(strsql,"%s:%02d.%02d.%d %s %.2f",
     gettext("Последняя проводка"),
     dpp,mpp,gpp,
     gettext("Дебет"),debpp);
    else
     sprintf(strsql,"%s:%02d.%02d.%d %s %.2f",
     gettext("Последняя проводка"),
     dpp,mpp,gpp,
     gettext("Кредит"),krepp);
    
   }   

  if(debet > kredit)
    fprintf(data->ff,"%*s:%10.2f %10s %s\n",
    iceb_u_kolbait(19,gettext("Сальдо")),gettext("Сальдо"),debet-kredit," ",strsql);   
  else
    fprintf(data->ff,"%*s:%10s %10.2f %s\n",
    iceb_u_kolbait(19,gettext("Сальдо")),gettext("Сальдо")," ",kredit-debet,strsql);
     
  while(cur1.read_cursor(&row1) != 0)
   {
    debet=atof(row1[5]);
    kredit=atof(row1[6]);
    iceb_u_rsdat(&d,&m,&g,row1[0],2);

    if((nomer=data->SHET.find(row1[1])) != -1)
     {
      data->DEB.plus(debet,nomer);
      data->KRE.plus(kredit,nomer);
     }  
    else
     {
      data->SHET.plus(row1[1]);
      data->DEB.plus(debet,-1);
      data->KRE.plus(kredit,-1);
     }

    memset(koment,'\0',sizeof(koment));
    sozkom(koment,sizeof(koment),row1[8],row1[2],row1[4]);

    data->kolstlst++;    
    if(data->kolstlst > kol_strok_na_liste)
      numlist(&data->kolstlst,&data->kollist,data->ff);

    fprintf(data->ff,"%02d.%02d.%04d %-*s %10.2f %10.2f %-3s %-4s %-*s %-*s%.*s\n",
    d,m,g,
    iceb_u_kolbait(8,row1[1]),row1[1],
    debet,kredit,row1[2],row1[7],
    iceb_u_kolbait(8,row1[3]),row1[3],
    iceb_u_kolbait(7,row1[4]),row1[4],
    iceb_u_kolbait(23,koment),koment);
 
    prdebet+=debet;
    prkredit+=kredit;

   }

  data->kolstlst++;    
  if(data->kolstlst > kol_strok_na_liste)
      numlist(&data->kolstlst,&data->kollist,data->ff);
  fprintf(data->ff,"\
-----------------------------------------------------------------------------\n");

  data->kolstlst++;    
  if(data->kolstlst > kol_strok_na_liste)
      numlist(&data->kolstlst,&data->kollist,data->ff);
  fprintf(data->ff,"%*s:%10.2f %10.2f\n",
  iceb_u_kolbait(19,gettext("Оборот за период")),gettext("Оборот за период"),
  prdebet,prkredit);

  debet=stdebet+dodebet+prdebet;
  kredit=stkredit+dokredit+prkredit;


  data->kolstlst++;    
  if(data->kolstlst > kol_strok_na_liste)
      numlist(&data->kolstlst,&data->kollist,data->ff);
  fprintf(data->ff,"%*s:%10.2f %10.2f\n",
  iceb_u_kolbait(19,gettext("Сальдо разв-тое")),gettext("Сальдо разв-тое"),
  debet,kredit);


  data->kolstlst++;    
  if(data->kolstlst > kol_strok_na_liste)
      numlist(&data->kolstlst,&data->kollist,data->ff);
  if(debet > kredit)
    fprintf(data->ff,"%*s:%10.2f\n",
    iceb_u_kolbait(19,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),
    debet-kredit);
  else
    fprintf(data->ff,"%*s:%10s %10.2f\n",
    iceb_u_kolbait(19,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),
    " ",
    kredit-debet);

 }


iceb_podpis(data->ff,data->window);

fclose(data->ff);


  /*******************************/
  //Распечатка свода по счетам
//  char imaf[30];
sprintf(imaf,"gokps%d.lst",getpid());

sprintf(strsql,"%s %s",gettext("Свод по корреспондирующим счетам"),data->rek_r->shet.ravno());
data->rek_r->naimf.plus(strsql);

data->rek_r->imaf.plus(imaf);

if((data->ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
  data->metka_kr=0;
  return(FALSE);
 }


iceb_u_zagolov(gettext("Свод по корреспондирующим счетам"),
data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,organ,data->ff);

fprintf(data->ff,"%s: %s %s\n",gettext("Счет"),
data->rek_r->shet.ravno(),data->rek_r->naimshet.ravno());

fprintf(data->ff,"\n");
fprintf(data->ff,"\
--------------------------------------------------------------\n");
fprintf(data->ff,gettext("\
  Счет  |   Наименование счета         |  Дебет   |  Кредит  |\n"));
fprintf(data->ff,"\
--------------------------------------------------------------\n");

int kolelem=data->SHET.kolih();
debet=kredit=0.;
for(nomer=0; nomer < kolelem; nomer++)
 {

  //Читаем наименование счета
  naimkontr.new_plus("");

  sprintf(strsql,"select nais from Plansh where ns='%s'",
  data->SHET.ravno(nomer));
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    naimkontr.new_plus(row1[0]);
    
  fprintf(data->ff,"%-*s %-*.*s %10.2f %10.2f\n",
  iceb_u_kolbait(8,data->SHET.ravno(nomer)),data->SHET.ravno(nomer),
  iceb_u_kolbait(30,naimkontr.ravno()),iceb_u_kolbait(30,naimkontr.ravno()),naimkontr.ravno(),
  data->DEB.ravno(nomer),
  data->KRE.ravno(nomer));

  debet+=data->DEB.ravno(nomer),
  kredit+=data->KRE.ravno(nomer);
  
 }
fprintf(data->ff,"\
--------------------------------------------------------------\n");
fprintf(data->ff,"%*s %10.2f %10.2f\n",
iceb_u_kolbait(39,gettext("Итого")),gettext("Итого"),debet,kredit);

iceb_podpis(data->ff,data->window);
fclose(data->ff);


iceb_printw_vr(data->vremn,data->buffer,data->view);

data->metka_kr=0;
data->voz=0;

for(int nomer=0; nomer < data->rek_r->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rek_r->imaf.ravno(nomer),1,data->window);
 
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);


return(FALSE);

}

/**********************************/
void prosmotrdo(short dn,short mn,short gn,const char *shet,const char *kontr,
short vds, //0-счет 1-субсчет
double *dodebet,double *dokredit,
short *dpp,short *mpp,short *gpp,//Дата последней проводки
double *debpp,double *krepp,short godn)
{
short	d,m,g;
char	strsql[512];
SQL_str	row1;

*dpp=0;
*mpp=0;
*gpp=0;
*debpp=0.;
*krepp=0.;

d=1;
m=1;
g=godn;

SQLCURSOR cur1;

//Смотрим проводки до периода
for(;iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0;)
 {
//  printw("%02d.%02d.%d\n",d,m,g);
//  refresh();
  /*Отчет для многопорядкового плана счетов*/
  if(vplsh == 1)
  sprintf(strsql,"select deb,kre from Prov where val=0 and \
datp = '%04d-%d-%d' and sh like '%s%%' and kodkon='%s' order by datp asc",
  g,m,d,shet,kontr);

  /*Отчет по субсчету двух-порядковый план счетов*/
  if(vds == 1 && vplsh == 0)
    sprintf(strsql,"select deb,kre from Prov where val=0 and \
datp = '%04d-%d-%d' and sh='%s' and kodkon='%s' order by datp asc",
    g,m,d,shet,kontr);

  /*Отчет по счету двух-порядковый план счетов*/
  if(vds == 0 && vplsh == 0)
    sprintf(strsql,"select deb,kre from Prov where val=0 and \
datp = '%04d-%d-%d' and sh like '%s%%' and kodkon='%s' order by datp asc",
    g,m,d,shet,kontr);

  if(cur1.make_cursor(&bd,strsql) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

  while(cur1.read_cursor(&row1) != 0)
   {
    *dodebet+=atof(row1[0]);
    *dokredit+=atof(row1[1]);

    *dpp=d; *mpp=m; *gpp=g;

    *debpp=atof(row1[0]);
    *krepp=atof(row1[1]);
   }
  iceb_u_dpm(&d,&m,&g,1);  
 }

}

/**************************/
/*Печать нумерации листов */
/**************************/

void numlist(int *kolstrlist,int *kollist,FILE *ff)
{

if(*kollist > 0 )
 {
  fprintf(ff,"\f");
  *kolstrlist=0;
 }

*kollist+=1;
*kolstrlist+=2;
fprintf(ff,"%60s N%d\n",gettext("Страница"),*kollist);

}
