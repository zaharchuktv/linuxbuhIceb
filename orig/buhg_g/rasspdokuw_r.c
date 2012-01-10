/*$Id: rasspdokuw_r.c,v 1.16 2011-02-21 07:35:57 sasa Exp $*/
/*24.03.2010	22.12.2005	Белых А.И.	rasspdokuw_r.c
Расчёт отчёта по списку документов в учёте услуг
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "rasspdokuw.h"

class rasspdokuw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class rasspdokuw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  rasspdokuw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rasspdokuw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdokuw_r_data *data);
gint rasspdokuw_r1(class rasspdokuw_r_data *data);
void  rasspdokuw_r_v_knopka(GtkWidget *widget,class rasspdokuw_r_data *data);

void rasnaku(int skl,short d,short m,short g,char nn[],
double *suma,double *sumabn,double *sumav,double *sp,double *spbn,
short vt,double sumkor,int lnds,FILE *ff,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double   okrg1;
extern double	okrcn;

int rasspdokuw_r(class rasspdokuw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class rasspdokuw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Движение услуг"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rasspdokuw_r_key_press),&data);

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

repl.plus(gettext("Движение услуг"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rasspdokuw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)rasspdokuw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rasspdokuw_r_v_knopka(GtkWidget *widget,class rasspdokuw_r_data *data)
{
//printf("rasspdokuw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasspdokuw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdokuw_r_data *data)
{
// printf("rasspdokuw_r_key_press\n");
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

gint rasspdokuw_r1(class rasspdokuw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int tipzz=data->rk->metka_pr;

int vt=0;
memset(strsql,'\0',sizeof(strsql));
iceb_poldan("Код группы возвратная тара",strsql ,"matnast.alx",data->window);
vt=atoi(strsql);

sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d\n",gettext("Период с"),dn,mn,gn,gettext("по"),
dk,mk,gk);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int kolstr=0;
/*Готовим файл для сортировки*/
sprintf(strsql,"select * from Usldokum where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);
SQLCURSOR cur;
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
char imaf1[56];
sprintf(imaf1,"sp%d.tmp",getpid());
FILE *ff1;

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char		otsr[24];
SQL_str row;
float kolstr1=0.;
int kolstr2=0;
unsigned int    ndpp;
short d,m,g;
SQL_str row1;
SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
/*
  printw("%d.%d.%d %d %s %s\n",mk1.d,mk1.m,mk1.g,mk1.tp,mk1.kop,mk1.kor);
*/

  if(tipzz != 0)
   if(tipzz != atoi(row[0]))
       continue;

  if(data->rk->metka_dok == 0)
   if(atoi(row[12]) != 1 )
       continue;

  if(data->rk->metka_dok == 1)
   if(atoi(row[12]) != 0 )
       continue;
  if(iceb_u_proverka(data->rk->kontr.ravno(),row[3],0,0) != 0)
     continue;
  if(iceb_u_proverka(data->rk->kodop.ravno(),row[6],0,0) != 0)
     continue;
     
  if(iceb_u_proverka(data->rk->podr.ravno(),row[2],0,0) != 0)
     continue;

  if(data->rk->grupk.getdlinna() > 1)
   {
    if(iceb_u_SRAV(row[3],"00-",1) == 0)
     sprintf(strsql,"select grup from Kontragent where kodkon='%s'","00");
    else
     sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[3]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      if(iceb_u_proverka(data->rk->grupk.ravno(),row1[0],0,0) != 0)
       continue;
     }

   }

  /* Период*/

  if(iceb_u_pole(row[4],strsql,2,'-') != 0)
   if(iceb_u_pole(row[4],strsql,2,'+') != 0)
    strcpy(strsql,row[4]);

  ndpp=(unsigned int)atoi(strsql);
  memset(otsr,'\0',sizeof(otsr));
  if(iceb_u_rsdat(&d,&m,&g,row[9],2) == 0)
   sprintf(otsr,"%02d.%02d.%04d",d,m,g);

  iceb_u_rsdat(&d,&m,&g,row[1],2);
  kolstr2++;    
  fprintf(ff1,"%s|%d|%d|%d|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[2],g,m,d,ndpp,row[4],row[3],row[6],row[5],row[7],otsr,row[11],row[26],row[16],row[0]);

 }

fclose(ff1);

sprintf(strsql,"%s !\n",gettext("Сортировка"));
/*Сортировка файла*/

if(data->rk->metka_sort == 0)
 sprintf(strsql,"sort -o %s -t\\| -k2,6 %s",imaf1,imaf1);
// sprintf(strsql,"sort -o %s -t\\| +1n +2n +3n +4n +5n %s",imaf1,imaf1);
if(data->rk->metka_sort == 1)
 sprintf(strsql,"sort -o %s -t\\| -k1,2 -k5,6 %s",imaf1,imaf1);
// sprintf(strsql,"sort -o %s -t\\| +0n -1 +4n -5 %s",imaf1,imaf1);
system(strsql);

sprintf(strsql,"%s\n",gettext("Печатаем"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if((ff1 = fopen(imaf1,"r")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf[56];
sprintf(imaf,"sp%d.lst",getpid());
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Распечатка списка документов"));

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf2[56];
sprintf(imaf2,"spn%d.lst",getpid());
data->rk->imaf.plus(imaf2);
data->rk->naimf.plus(gettext("Распечатка содержимого документов"));

FILE *ff2;
if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Распечачатка документов"),dn,mn,gn,dk,mk,gk,organ,ff);

iceb_u_zagolov(gettext("Распечачатка документов"),dn,mn,gn,dk,mk,gk,organ,ff2);

if(tipzz == 1)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
 }
if(tipzz == 2)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
 }
if(data->rk->metka_dok == 0)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
 }
if(data->rk->metka_dok == 1)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
 }
if(data->rk->metka_dok == 3)
 {
  fprintf(ff,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
 }

if(data->rk->kodop.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
 }
if(data->rk->kontr.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
 }

if(data->rk->podr.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код подразделения"),data->rk->kontr.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код подразделения"),data->rk->kontr.ravno());
 }

if(data->rk->grupk.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код группы контрагента"),data->rk->grupk.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код группы контрагента"),data->rk->grupk.ravno());
 }

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("  Дата    |Дата опла.|Ном.док. |Нал. накл.|  Сумма  |Сум.кop.|  НДС   |Сумма с НДС|Контр.|  Наименование контрагента    |Код оп.|   Телефон   |\n"));

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
double itog=0.,itogsk=0.,itognds=0.;
double sp=0.,spbn=0.;
kolstr1=0.;
SQLCURSOR curr;
double nds=0.;
double itogo_nds=0.;
float pnds=0.;
double bb=0.,bb1=0.;
int skl=0;
char str[1024];
char            kop[24]; /*код операции*/
char            kor[24]; /*код контрагента*/
char            nn[24],nnn[24];  /*номер накладной*/
char            telefon[56];
char naior[512];
int		lnds;
double		sumkor;
double suma=0.;
double sumabn=0.,sumav=0.;
class iceb_u_str nomdokp("");
int tipz=0;
while(fgets(str,sizeof(str)-1,ff1) != NULL)
 {
//  printw("%s",str);
  
  iceb_pbar(data->bar,kolstr2,++kolstr1);    

  iceb_u_pole(str,strsql,1,'|');
  skl=atoi(strsql);

  iceb_u_pole(str,strsql,2,'|');
  g=atoi(strsql);

  iceb_u_pole(str,strsql,3,'|');
  m=atoi(strsql);

  iceb_u_pole(str,strsql,4,'|');
  d=atoi(strsql);

  iceb_u_pole(str,nn,6,'|');
  iceb_u_pole(str,kor,7,'|');
  iceb_u_pole(str,kop,8,'|');
  iceb_u_pole(str,nnn,9,'|');
  iceb_u_pole(str,strsql,10,'|');
  lnds=atoi(strsql);
  iceb_u_pole(str,otsr,11,'|');
  iceb_u_pole(str,strsql,12,'|');
  sumkor=atof(strsql);

  iceb_u_pole(str,strsql,13,'|');
  pnds=atof(strsql);
  iceb_u_polen(str,&nomdokp,14,'|');
  iceb_u_polen(str,&tipz,15,'|');
    
  memset(naior,'\0',sizeof(naior));
  memset(telefon,'\0',sizeof(telefon));

  if(kor[0] == '0' && kor[1] == '0' && iceb_u_pole(kor,strsql,1,'-') == 0)
   strcpy(kor,strsql);
      
  sprintf(strsql,"select naikon,telef from Kontragent where kodkon='%s'",kor);
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    fprintf(ff,"%s %s !\n",gettext("Не нашли код контрагента"),kor);

    sprintf(strsql,"%s %s !",gettext("Не нашли код контрагента"),kor);
    iceb_menu_soob(strsql,data->window);
   
   }
  else
   {
    strncpy(naior,row1[0],sizeof(naior)-1);
    strncpy(telefon,row1[1],sizeof(telefon)-1);
   }



  /*Узнаем сумму записей по накладной*/
  
  fprintf(ff2,"\n%d.%d.%d %s(%s) %s %s\n",d,m,g,nn,nomdokp.ravno(),kor,naior);

  rasnaku(skl,d,m,g,nn,&suma,&sumabn,&sumav,&sp,&spbn,vt,sumkor,lnds,ff2,data->window);

  itogsk+=sumkor;
  bb=suma+sumkor+sumabn+(suma+sumkor)*pnds/100.;
  bb=iceb_u_okrug(bb,okrg1);

  bb1=suma+sumabn;
  itog+=bb1;
  itognds+=bb;
  nds=bb-sumkor-bb1;
  itogo_nds+=nds;

  sprintf(strsql,"%02d.%02d.%d %-*s %-*.*s %-*s %8.2f %8.8g %10.2f\n",
  d,m,g,
  iceb_u_kolbait(5,kor),kor,
  iceb_u_kolbait(20,naior),iceb_u_kolbait(20,naior),naior,
  iceb_u_kolbait(9,nn),nn,
  bb1,sumkor,bb);

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
  fprintf(ff,"%02d.%02d.%d %10.10s %-*s %-*s %9.2f %8.8g %8.2f %11.2f %-*s %-*.*s %-*s %s\n",
  d,m,g,otsr,
  iceb_u_kolbait(9,nn),nn,
  iceb_u_kolbait(10,nnn),nnn,
  bb1,sumkor,nds,bb,
  iceb_u_kolbait(6,kor),kor,
  iceb_u_kolbait(30,naior),iceb_u_kolbait(30,naior),naior,
  iceb_u_kolbait(7,kop),kop,
  telefon);

  if(tipz == 1)  
   {
    sprintf(strsql,"select vido from Usloper1 where kod='%s'",kop);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     if(atoi(row1[0]) == 0)    
      if(nomdokp.getdlinna() > 1)
        fprintf(ff,"%10s %10s %-*s\n","","",iceb_u_kolbait(9,nomdokp.ravno()),nomdokp.ravno());
   }

 }

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n\
%*s: %10.2f %8.8g %8.2f %11.2f\n",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),itog,itogsk,itogo_nds,itognds);

sprintf(strsql,"%*s: %8.2f %8.8g %10.2f\n",iceb_u_kolbait(45,gettext("Итого")),gettext("Итого"),itog,itogsk,itognds);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


fprintf(ff2,"%s:\n\
-----------------------------------------------------------------------------------------------\n",
gettext("Общий итог"));

fprintf(ff2,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("Итого")),gettext("Итого"),itog," ",sp+spbn);

bb=sp+spbn+sp*pnds/100.;
bb=iceb_u_okrug(bb,okrg1);
fprintf(ff2,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("Итого с НДС")),gettext("Итого с НДС"),itognds," ",bb);

iceb_podpis(ff,data->window);
iceb_podpis(ff2,data->window);

fclose(ff);
fclose(ff1);
fclose(ff2);
unlink(imaf1);



for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/*********************************************************************/

void rasnaku(int skl,short d,short m,short g,char nn[],
double *suma,double *sumabn,double *sumav,double *sp,double *spbn,
short vt,double sumkor,int lnds,FILE *ff,GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
double		bb,bb1;
short		ks;
double		kol;
double		itog[2];
double		sumap,sumapbn;
char		naim[512];
char		ei[32];
double		kolih,cena;
int		kodzap,tipz;
short		metka;


//iceb_poldan("Н.Д.С.",strsql,"matnast.alx",wpredok);
//float pnds=iceb_u_atof(strsql);
float pnds=iceb_pnds(wpredok);

/*Узнаем сумму всех записей*/
*suma=*sumabn=*sumav=0.;
sumap=sumapbn=0.;

sprintf(strsql,"select tp,metka,kodzap,kolih,cena,ei,shetu,nz from Usldokum1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' order by kodzap asc",
g,m,d,skl,nn);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;

fprintf(ff,"-----------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 N |       Наименование           |Ед. |Количество|  Ц е н а | С у м м а|Количество|Сумма под-|\n\
   |         услуг                |изм.|          |          |          |подтвержд.|тверждена |\n"));

fprintf(ff,"-----------------------------------------------------------------------------------------------\n");
SQLCURSOR curr;
ks=0;
itog[0]=itog[1]=0.;
while(cur.read_cursor(&row) != 0)
 {
  tipz=atoi(row[0]);
  metka=atoi(row[1]);  
  kodzap=atoi(row[2]);
  kolih=atof(row[3]);
  cena=atof(row[4]);
  cena=iceb_u_okrug(cena,okrcn);
  strncpy(ei,row[5],sizeof(ei)-1);
  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  if(metka == 0)
   {
    /*Узнаем наименование материалла*/
    sprintf(strsql,"select naimat from Material where kodm=%s",
    row[2]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {

      fprintf(ff,"%s %s !\n",gettext("Не найден код материалла"),row[2]);
      sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),row[2]);
      iceb_menu_soob(strsql,wpredok);
      continue;
     }
   }

  if(metka == 1)
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius from Uslugi where kodus=%s",
    row[2]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      fprintf(ff,"%s %s !\n",gettext("Не найден код услуги"),row[2]);
      sprintf(strsql,"%s %s !\n",gettext("Не найден код услуги"),row[2]);
      iceb_menu_soob(strsql,wpredok);
      continue;
     }
   }
  strncpy(naim,row1[0],sizeof(naim)-1);
  
  if(lnds > 0)
    *sumabn=*sumabn+bb;
  else
    *suma=*suma+bb;
  kol=readkoluw(d,m,g,skl,nn,tipz,metka,kodzap,atoi(row[7]),wpredok);
    
  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  bb1=cena*kol;
  bb1=iceb_u_okrug(bb1,okrg1);

  ks++;
  fprintf(ff,"%3d %-*.*s %-*.*s %10.10g %10.10g %10.2f %10.10g %10.2f\n",
  ks,
  iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,
  iceb_u_kolbait(4,ei),iceb_u_kolbait(4,ei),ei,
  kolih,cena,bb,kol,bb1);

  if(lnds > 0)
    sumapbn+=bb1;
  else
    sumap+=bb1;

 }


itog[0]=*suma+*sumabn;
itog[1]=sumap+sumapbn;
fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("Итого")),gettext("Итого"),itog[0]," ",itog[1]);

if(sumkor != 0)
 {
  bb=sumkor*100./itog[0];
  bb=iceb_u_okrug(bb,0.01);

  if(sumkor < 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Снижка"),bb*(-1));
  if(sumkor > 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Надбавка"),bb);

  fprintf(ff,"%*s %10.2f            %10.2f\n",iceb_u_kolbait(61,strsql),strsql,sumkor,sumkor);

  if(sumkor < 0)
      sprintf(strsql,gettext("Итого со снижкой :"));
  if(sumkor > 0)
      sprintf(strsql,gettext("Итого с надбавкой :"));

   fprintf(ff,"%*s %10.2f            %10.2f\n",iceb_u_kolbait(61,strsql),strsql,sumkor+itog[0],sumkor+itog[1]);

 }

bb=*suma+sumkor+*sumabn+( *suma + sumkor)*pnds/100.;
bb=iceb_u_okrug(bb,okrg1);
bb1=sumap+sumkor+sumapbn+(sumap+sumkor)*pnds/100.;
bb1=iceb_u_okrug(bb1,okrg1);

fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("НДС")),gettext("НДС"),bb-sumkor-itog[0]," ",bb1-sumkor-itog[1]);

fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("Итого с НДС")),gettext("Итого с НДС"),bb," ",bb1);
*sp=*sp+sumap;
*spbn=*spbn+sumapbn;
}
