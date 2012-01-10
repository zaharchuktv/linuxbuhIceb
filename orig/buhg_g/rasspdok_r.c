/*$Id: rasspdok_r.c,v 1.19 2011-02-21 07:35:56 sasa Exp $*/
/*24.03.2010	01.02.2005	Белых А.И.	rasspdok_r.c
Расчёт отчёта по списку документов в материальном учёте
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "rasspdok.h"

class rasspdok_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class rasspdok_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  rasspdok_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rasspdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdok_r_data *data);
gint rasspdok_r1(class rasspdok_r_data *data);
void  rasspdok_r_v_knopka(GtkWidget *widget,class rasspdok_r_data *data);

void		rasnak(int skl,short d,short m,short g,char nn[],
double *suma,double *sumabn,double *sp,double *spbn,
short vt,double *sumkor,int lnds,FILE *ff,FILE *ffnp,double *sumsnds,
float pnds,
GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double   okrg1;
extern double	okrcn;

int rasspdok_r(class rasspdok_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class rasspdok_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечетать движение товарно-материальных ценностей (форма 1)"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rasspdok_r_key_press),&data);

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

repl.plus(gettext("Распечетать движение товарно-материальных ценностей (форма 1)"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rasspdok_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)rasspdok_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rasspdok_r_v_knopka(GtkWidget *widget,class rasspdok_r_data *data)
{
//printf("rasspdok_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasspdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdok_r_data *data)
{
// printf("rasspdok_r_key_press\n");
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

gint rasspdok_r1(class rasspdok_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);


struct tm *bf;
bf=localtime(&vremn);

int vt=0;

memset(strsql,'\0',sizeof(strsql));
iceb_poldan("Код группы возвратная тара",strsql,"matnast.alx",data->window);
vt=atoi(strsql);
short dn,mn,gn;
short dk,mk,gk;
iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);

sprintf(strsql,"%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Распечатка документов за период c"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


/*Готовим файл для сортировки*/
sprintf(strsql,"select * from Dokummat where datd >= '%d-%02d-%02d' \
and datd <= '%d-%02d-%02d'",gn,mn,dn,gk,mk,dk);
int kolstr;
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  data->kon_ras=0;
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  data->kon_ras=0;
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }

char imaf1[32];
FILE *ff1;

sprintf(imaf1,"sp%d.tmp",getpid());

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,NULL);
  data->kon_ras=0;
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }
SQL_str row;
char data_nal_nak[16];
short dnn,mnn,gnn;
SQL_str row1;
SQLCURSOR cur1;

float kolstr1=0.;
int kolstr2=0;
int ndpp;
short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(data->rk->metka_pr != 0)
   if(data->rk->metka_pr != atoi(row[0]))
       continue;

  if(data->rk->metka_dok == 0)
   if(atoi(row[7]) != 1 )
       continue;

  if(data->rk->metka_dok == 1)
   if(atoi(row[7]) != 0 )
       continue;

  if(iceb_u_proverka(data->rk->kontr.ravno(),row[3],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->kodop.ravno(),row[6],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[2],0,0) != 0)
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
  ndpp=atoi(strsql);

  iceb_u_rsdat(&dnn,&mnn,&gnn,row[12],2);
  memset(data_nal_nak,'\0',sizeof(data_nal_nak));
  if(dnn > 0)
   sprintf(data_nal_nak,"%02d.%02d.%d",dnn,mnn,gnn);
  
  iceb_u_rsdat(&d,&m,&g,row[1],2);
  kolstr2++;    
  fprintf(ff1,"%s|%d|%d|%d|%d|%s|%s|%s|%s|%s|%s|%s|%s|\n",
  row[2],g,m,d,ndpp,row[4],row[3],row[6],row[5],row[13],data_nal_nak,row[11],row[0]);

 }

fclose(ff1);

sprintf(strsql,"%s.\n",gettext("Сортируем записи"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

/*Сортировка файла*/

if(data->rk->metka_sort == 0)
 sprintf(strsql,"sort -o %s -t\\| -n -k2,6 %s",imaf1,imaf1);
// sprintf(strsql,"sort -o %s -t\\| +1n +2n +3n +4n +5n %s",imaf1,imaf1);
if(data->rk->metka_sort == 1)
 sprintf(strsql,"sort -o %s -t\\| -n -k1,2 -k5,6 %s",imaf1,imaf1);
// sprintf(strsql,"sort -o %s -t\\| +0n -1 +4n -5 %s",imaf1,imaf1);
system(strsql);

sprintf(strsql,"%s\n",gettext("Печатаем"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if((ff1 = fopen(imaf1,"r")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,NULL);
  data->kon_ras=0;
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }

char imaf[32];
FILE *ff;

sprintf(imaf,"sp%d.lst",getpid());
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Распечатка списка документов"));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
  data->kon_ras=0;
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }
char imaf2[32];
FILE *ff2;
sprintf(imaf2,"spn%d.lst",getpid());
data->rk->imaf.plus(imaf2);
data->rk->naimf.plus(gettext("Распечатка содержимого документов"));
if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,NULL);
  data->kon_ras=0;
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }
char imafnp[56];
sprintf(imafnp,"spnp%d.lst",getpid());
data->rk->imaf.plus(imafnp);
data->rk->naimf.plus(gettext("Распечатка неподтверждённых записей в документах"));
FILE *ffnp;
if((ffnp = fopen(imafnp,"w")) == NULL)
 {
  iceb_er_op_fil(imafnp,"",errno,NULL);
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Распечатка документов"),dn,mn,gn,dk,mk,gk,organ,ff);
iceb_u_zagolov(gettext("Распечатка документов"),dn,mn,gn,dk,mk,gk,organ,ff2);
iceb_u_zagolov(gettext("Распечатка документов"),dn,mn,gn,dk,mk,gk,organ,ffnp);



fprintf(ffnp,"\n%s !\n",gettext("Только не подтвержденные записи в документах"));

if(data->rk->metka_pr == 1)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
 }
if(data->rk->metka_pr == 2)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
 }
if(data->rk->metka_dok == 0)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
 }
if(data->rk->metka_dok == 1)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
 }
if(data->rk->metka_dok == 3)
 {
  fprintf(ff,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
  fprintf(ffnp,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
 }

if(data->rk->kodop.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
 }
if(data->rk->kontr.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
 }

if(data->rk->sklad.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
 }

if(data->rk->grupk.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код группы контрагента"),data->rk->grupk.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код группы контрагента"),data->rk->grupk.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Код группы контрагента"),data->rk->grupk.ravno());
 }


fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("  Дата    |Дата отср.|Ном.док. |Нал. накл.|Дата н.нак|  Сумма  |Сум.кop.|Сумма с НДС|  По кассе |Контр.|  Наименование контрагента    |Код оп.|   Телефон   |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

double itog=0.,itogsk=0.,itognds=0.;
double sp=0.,spbn=0.;
double isumsnds=0.,sumsnds=0.;
double sumndspr=0.;
double bb,bb1;
char str[1024];
char nn[56];
char kor[32];
char kop[32];
char nnn[32];
char naior[512];
char telefon[512];
char kor1[56];
char otsr[24];
int lnds;
int skl;
double suma=0.;
double sumabn=0.;
double sumkor=0.;
double ndssum=0.;
short		shek;
kolstr1=0;
int tipz=0;
class iceb_u_str nomdokp("");
float pnds=0.;
while(fgets(str,sizeof(str),ff1) != NULL)
 {
//  kolstr1++;
//  strzag(LINES-1,0,kolstr2,kolstr1);
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
  pnds=atof(strsql);
  iceb_u_polen(str,data_nal_nak,sizeof(data_nal_nak),11,'|');
  iceb_u_polen(str,&nomdokp,12,'|');
  iceb_u_polen(str,&tipz,13,'|');
    
  memset(naior,'\0',sizeof(naior));
  memset(telefon,'\0',sizeof(telefon));
  memset(kor1,'\0',sizeof(kor1));
  if(kor[0] == '0' && kor[1] == '0' && iceb_u_pole(kor,kor1,2,'-') == 0)
    sprintf(strsql,"select naik from Sklad where kod='%s'",kor1);
  else
    sprintf(strsql,"select naikon,telef from Kontragent where kodkon='%s'",kor);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    if(kor1[0] == '\0')
     {
      fprintf(ff,"%s - %s\n",gettext("Не найден контрагент"),kor);
      sprintf(strsql,"%s %s",gettext("Не найден контрагент"),kor);
      iceb_menu_soob(strsql,data->window);
     }
    else
     {
      fprintf(ff,"%s %s\n",gettext("Не найден код склада"),kor1);
      sprintf(strsql,"%s %s",gettext("Не найден код склада"),kor1);
      iceb_menu_soob(strsql,data->window);
     }
   }
  else
   {
    strncpy(naior,row1[0],sizeof(naior)-1);
    if(kor1[0] == '\0')
      strncpy(telefon,row1[1],sizeof(telefon)-1);
   }
 
  /*Узнаем дату отсрочки платежа*/

  sprintf(strsql,"select sodz from Dokummat2 \
  where god=%d and nomd='%s' and sklad=%d and nomerz=9",
  g,nn,skl);
  memset(otsr,'\0',sizeof(otsr));

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strncpy(otsr,row1[0],sizeof(otsr)-1);   

  /*Узнаем НДС документа*/
  sprintf(strsql,"select sodz from Dokummat2 \
  where god=%d and nomd='%s' and sklad=%d and nomerz=11",
  g,nn,skl);
  lnds=0;
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     lnds=atoi(row1[0]);

  /*Узнаем Был ли распечатан кассовый чек*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=5",
  g,nn,skl);
  shek=0;
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     shek=atoi(row1[0]);

  sumndspr=0.;

  //Читаем введенную сумму НДС для приходного документа
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=6",
  g,nn,skl);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     sumndspr=atoi(row1[0]);

  /*Узнаем сумму записей по накладной*/
  
  fprintf(ff2,"\n%d.%d.%d %s(%s) %s %s\n",d,m,g,nn,nomdokp.ravno(),kor,naior);

  fprintf(ffnp,"\n%d.%d.%d %s(%s) %s %s\n",d,m,g,nn,kor,nomdokp.ravno(),naior);

  rasnak(skl,d,m,g,nn,&suma,&sumabn,&sp,&spbn,vt,&sumkor,lnds,ff2,ffnp,
  &sumsnds,pnds,data->window);

  if(lnds == 0)
   {
    if(sumndspr != 0.)
     ndssum=sumndspr;
    else
     ndssum=(suma+sumkor)*pnds/100.;
    bb=suma+sumkor+sumabn+ndssum;
   }
  else
    bb=suma+sumkor+sumabn;

  bb=iceb_u_okrug(bb,okrg1);
  bb1=suma+sumabn;
  itog+=bb1;
  itognds+=bb;

  if(shek != 0)
   {
    isumsnds+=sumsnds;
   }
  else
   sumsnds=0.;  
  itogsk+=sumkor;

  sprintf(strsql,"%02d.%02d.%d %-*s %-*.*s %-*s %8.2f %8.8g %8.2f %8.2f\n",
  d,m,g,
  iceb_u_kolbait(5,kor),kor,
  iceb_u_kolbait(15,naior),iceb_u_kolbait(15,naior),naior,
  iceb_u_kolbait(9,nn),nn,
  bb1,sumkor,bb,sumsnds);

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  fprintf(ff,"%02d.%02d.%d %10.10s %-*s %-*s %10s %9.2f %8.8g %11.2f %11.2f %-*s %-*.*s \
%-*s %s\n",
  d,m,g,
  otsr,
  iceb_u_kolbait(9,nn),nn,
  iceb_u_kolbait(10,nnn),nnn,
  data_nal_nak,bb1,sumkor,bb,sumsnds,
  iceb_u_kolbait(6,kor),kor,
  iceb_u_kolbait(30,naior),iceb_u_kolbait(30,naior),naior,
  iceb_u_kolbait(7,kop),kop,
  telefon);

  if(tipz == 1)  
   {
    sprintf(strsql,"select vido from Prihod where kod='%s'",kop);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     if(atoi(row1[0]) == 0)    
      if(nomdokp.getdlinna() > 1)
        fprintf(ff,"%10s %10s %-*s\n","","",iceb_u_kolbait(9,nomdokp.ravno()),nomdokp.ravno());
   }

 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s: %10.2f %8.8g %11.2f %11.2f\n",
iceb_u_kolbait(51,gettext("Итого")),gettext("Итого"),itog,itogsk,itognds,isumsnds);

sprintf(strsql,"%*s: %8.2f %8.8g %8.2f %8.2f\n",
iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),itog,itogsk,itognds,isumsnds);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

iceb_podpis(ff,data->window);

fprintf(ff,"\x12");
fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/

fprintf(ff2,"%s:\n\
-----------------------------------------------------------------------------------------------\n",
gettext("Общий итог"));

fprintf(ff2,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("Итого")),gettext("Итого"),itog," ",sp+spbn);

bb=sp+spbn+sp*pnds/100.;
bb=iceb_u_okrug(bb,okrg1);
fprintf(ff2,"%*s %10.2f %10s %10.2f\n",
iceb_u_kolbait(61,gettext("Итого с НДС")),gettext("Итого с НДС"),itognds," ",bb);

iceb_podpis(ff2,data->window);
iceb_podpis(ffnp,data->window);


fclose(ff);
fclose(ff1);
fclose(ff2);
fclose(ffnp);
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

void		rasnak(int skl,short d,short m,short g,char nn[],
double *suma,double *sumabn,double *sp,double *spbn,
short vt,double *sumkor,int lnds,FILE *ff,FILE *ffnp,double *sumsnds,
float pnds,
GtkWidget *wpredok)
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
char		ei[56];
short		vtr;
double		kolih,cena;
int		nk;
short		kgrm;
char		bros[512];
short		mnds; /*1-НДС в том числе*/
char		nomn[56]; /*Номенклатурный номер*/
char		shu[56];
double		sumands=0.;
double		sumandspod=0.;
double		sumandspr=0.; //Суммаc НДС для приходного документа введенная вручную
SQLCURSOR cur;
SQLCURSOR cur1;


/*Узнаем сумму скидки*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
g,nn,skl);

*sumkor=0.;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  *sumkor=atof(row[0]);

//Узнаем сумму НДС
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=6",
g,nn,skl);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  sumandspr=atof(row[0]);


/*Узнаем сумму всех записей*/
*sumsnds=*suma=*sumabn=0.;
sumap=sumapbn=0.;

sprintf(strsql,"select kodm,kolih,cena,voztar,ei,nomkar,mnds \
from Dokummat1 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' \
order by kodm asc",g,m,d,skl,nn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;

fprintf(ff,"-----------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext(" N |       Наименование продукции |Ед. |Количество|  Ц е н а | С у м м а|Количество|Сумма под-|\n"));
fprintf(ff,gettext("   |              (товару)        |изм.|          |          |          |подтвержд.|тверждена |\n"));
fprintf(ff,"-----------------------------------------------------------------------------------------------\n");

fprintf(ffnp,"-----------------------------------------------------------------------------------------------\n");
fprintf(ffnp,gettext(" N |       Наименование продукции |Ед. |Количество|  Ц е н а | С у м м а|Количество|Сумма под-|\n"));
fprintf(ffnp,gettext("   |              (товару)        |изм.|          |          |          |подтвержд.|тверждена |\n"));
fprintf(ffnp,"-----------------------------------------------------------------------------------------------\n");

ks=0;
memset(nomn,'\0',sizeof(nomn));
itog[0]=itog[1]=0.;
while(cur.read_cursor(&row) != 0)
 {
/*   printw("*%d %d %s\n",mk2.g,mk2.skl,mk2.nn);*/
  kolih=atof(row[1]);
  cena=atof(row[2]);
//Для вложенного налога
  if(lnds == 0)
    bb=cena+cena*pnds/100.;
  else
    bb=cena;

  bb=iceb_u_okrug(bb,okrg1);
 
  bb=bb*kolih;

  *sumsnds+=iceb_u_okrug(bb,okrg1);

  cena=iceb_u_okrug(cena,okrcn);
  vtr=atoi(row[3]);
  strncpy(ei,row[4],sizeof(ei)-1);
  nk=atoi(row[5]);
  mnds=atoi(row[6]);  
  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  /*Узнаем код группы*/
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",
  row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("%s %s !\n",gettext("Не найден код материалла"),row[0]);
   }
  strcpy(naim,row1[0]);
  kgrm=atoi(row1[1]);
  
  if(vtr == 1 || kgrm == vt || lnds > 0 || mnds == 1)
    *sumabn=*sumabn+bb;
  else
    *suma=*suma+bb;

  kol=readkolkw(skl,nk,d,m,g,nn,wpredok);  
    
  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  bb1=cena*kol;
  bb1=iceb_u_okrug(bb1,okrg1);

  ks++;
  /*Узнаем счет учета для номенклатурного номера*/
  memset(nomn,'\0',sizeof(nomn));
  if(nk != 0)
   {
    memset(shu,'\0',sizeof(shu));
    sprintf(strsql,"select shetu from Kart where sklad=%d and nomk=%d",
    skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      printf("Не найдена карточка %d на складе %d !\n",nk,skl);
     }
    else
     {
      strncpy(shu,row1[0],sizeof(shu)-1);
     }
    sprintf(nomn,"%d.%s.%s.%d",skl,shu,row[0],nk);
   }

  fprintf(ff,"%3d %-*.*s %-*.*s %10.10g %10.10g %10.2f %10.10g %10.2f %s\n",
  ks,
  iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,
  iceb_u_kolbait(4,ei),iceb_u_kolbait(4,ei),ei,
  kolih,cena,bb,kol,bb1,nomn);

  if(kol == 0.)
   fprintf(ffnp,"%3d %-*.*s %-*.*s %10.10g %10.10g %10.2f %10.10g %10.2f %s\n",
   ks,
  iceb_u_kolbait(30,naim),iceb_u_kolbait(30,naim),naim,
  iceb_u_kolbait(4,ei),iceb_u_kolbait(4,ei),ei,
   kolih,cena,bb,kol,bb1,nomn);

  if(vtr == 1 || kgrm == vt || lnds > 0 || mnds == 1)
    sumapbn+=bb1;
  else
    sumap+=bb1;

 }

/*Читаем услуги*/

sprintf(strsql,"select * from Dokummat3 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",g,m,d,skl,nn);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {

  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=iceb_u_okrug(cena,okrcn);

//Для вложенного налога
  bb=cena+cena*pnds/100.;
  bb=iceb_u_okrug(bb,okrg1);
  bb=bb*kolih;
  *sumsnds+=iceb_u_okrug(bb,okrg1);
  
  if(kolih > 0)
    bb=kolih*cena;
  else
    bb=cena;
  bb=iceb_u_okrug(bb,okrg1);
  *suma=*suma+iceb_u_okrug(bb,okrg1);

  ks++;
  fprintf(ff,"%3d %-*.*s %-*.*s %10.10g %10s",
  ks,
  iceb_u_kolbait(30,row[7]),iceb_u_kolbait(30,row[7]),row[7],
  iceb_u_kolbait(4,row[3]),iceb_u_kolbait(4,row[3]),row[3],
  kolih,iceb_prcn(cena));
  fprintf(ff," %10s\n",iceb_prcn(bb));

 }



itog[0]=*suma+*sumabn;
itog[1]=sumap+sumapbn;

fprintf(ff,"%*s %10.2f %10s %10.2f\n",
iceb_u_kolbait(61,gettext("Итого")),gettext("Итого"),itog[0]," ",itog[1]);

if(*sumkor != 0)
 {
  bb=*sumkor*100./itog[0];
  bb=iceb_u_okrug(bb,0.01);
  if(*sumkor < 0)
      sprintf(bros,"%s %.6g%% :",gettext("Снижка"),bb*(-1));
  if(*sumkor > 0)
      sprintf(bros,"%s %.6g%% :",gettext("Надбавка"),bb);

  fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(61,bros),bros,*sumkor);

  if(*sumkor < 0)
      sprintf(bros,gettext("Итого со снижкой :"));
  if(*sumkor > 0)
      sprintf(bros,gettext("Итого с надбавкой :"));

   fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(61,bros),bros,*sumkor+itog[0]);

 }
sumandspod=sumands=0.;
if(lnds == 0)
 {
  if(sumandspr != 0.)
   sumands=sumandspr;
  else 
   sumands=( *suma + *sumkor)*pnds/100.;
 }

bb=*suma+*sumkor+*sumabn+sumands;
bb=iceb_u_okrug(bb,okrg1);
if(lnds == 0)
  sumandspod=sumap*pnds/100.;

bb1=sumap+sumapbn+sumandspod;
bb1=iceb_u_okrug(bb1,okrg1);
sumands=

fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("НДС")),gettext("НДС"),sumands," ",sumandspod);

fprintf(ff,"%*s %10.2f %10s %10.2f\n",
iceb_u_kolbait(61,gettext("Итого с НДС")),gettext("Итого с НДС"),bb," ",bb1);

*sp=*sp+sumap;
*spbn=*spbn+sumapbn;

}
