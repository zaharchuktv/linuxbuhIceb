/*$Id: rppvow_r.c,v 1.24 2011-02-21 07:35:57 sasa Exp $*/
/*14.03.2010	01.02.2005	Белых А.И.	rppvow_r.c
Расчёт реестра проводок по видам операций в материальном учёте
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "rppvow.h"

class rppvow_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class rppvow_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  rppvow_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rppvow_r_key_press(GtkWidget *widget,GdkEventKey *event,class rppvow_r_data *data);
gint rppvow_r1(class rppvow_r_data *data);
void  rppvow_r_v_knopka(GtkWidget *widget,class rppvow_r_data *data);

void		rasnak(int skl,short d,short m,short g,char nn[],
double *suma,double *sumabn,double *sp,double *spbn,
short vt,double *sumkor,int lnds,FILE *ff,FILE *ffnp,double *sumsnds,
GtkWidget *wpredok);

void  sapitsh(short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int skl1,char *naiskl,FILE *ff);
void            rasoi(iceb_u_spisok *mssi,iceb_u_double *mspsi,short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int skl1,char *naiskl,FILE *ff);

void            rasis(char orgn[],double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,char naiskl[],char naik[],
iceb_u_spisok *mso,
iceb_u_double *mspo,
iceb_u_spisok *mss,
iceb_u_double *msps,
int*,int*,
FILE *ff);

void            rasio(char orgn[],double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,char naiskl[],char naik[],
iceb_u_spisok *mso,
iceb_u_double *mspo,
int*,int*,
FILE *ff);

void            rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,char naiskl[],int*,int*,FILE *ff);

void            gsapr(short dn,short mn,short gn,short dk,short mk,
short gk,int *sl,int *kst,int skl1,char naiskl[],FILE *ff);

void rppvo1(int skl1,char *naiskl,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
FILE *ff);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double	nds1;
extern double   okrg1;
extern double	okrcn;
extern int kol_strok_na_liste;

int rppvow_r(class rppvow_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class rppvow_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать реестр проводок по видам операций"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rppvow_r_key_press),&data);

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

repl.plus(gettext("Распечатать реестр проводок по видам операций"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rppvow_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)rppvow_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rppvow_r_v_knopka(GtkWidget *widget,class rppvow_r_data *data)
{
//printf("rppvow_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rppvow_r_key_press(GtkWidget *widget,GdkEventKey *event,class rppvow_r_data *data)
{
// printf("rppvow_r_key_press\n");
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

gint rppvow_r1(class rppvow_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;



iceb_u_spisok    mso; /*Массив счетов по организации*/
iceb_u_double mspo; /*Массив сумм проводок по организации*/
iceb_u_spisok    mss; /*Массив счетов по складу*/
iceb_u_double msps; /*Массив сумм проводок по складу*/
iceb_u_spisok    mssi; /*Массив счетов по всем складам*/
iceb_u_double mspsi; /*Массив сумм проводок по всем складам по операции*/
iceb_u_spisok    mssio; /*Массив счетов по всем складам и всем операциям*/
iceb_u_double mspsio; /*Массив сумм проводок по всем складам по всем операциям*/
FILE            *ff;
char		bros[512];
char		str[512];
FILE            *ff1;
struct  tm      *bf;
bf=localtime(&vremn);
short           kom;
char		kome[512]; /*Коментарий*/
short           dp,mp,gp;
short           dd,md,gd;
char            orgn[32];
char            imaf[56],imaftmp[56];
char            sh1[32];
char            shett[112];
char            shett1[112];
double          itd; /*Итого по счету*/
int             i;
double          k1;
short           tipzz;
int		tz; /*1-Приход 2-расход*/
short           klr;
short           dn,mn,gn;
short           dk,mk,gk;
double		deb;
long		kolstr,kolstr2;
float           kolstr1;
SQL_str         row,row1;
char		kop[20];
int		skl=0,skl1=0;
char		naik[512];
char		naimpr[512];
char		naiskl[512];
char		nomdok[112],nomdokv[112];
short		tp;
char		kprr[32],kprr1[32];
char		kpos[32];
char		datd[16];
char		nn1[32];
char		kor[32];		
char 		nomnalnak[32];
int           kst=0,sli=0;
char imafitog[56];
FILE *ffitog;


if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d\n",gettext("Период с"),dn,mn,gn,dk,mk,gk);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  
sprintf(strsql,"select * from Prov where val=0 and \
datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and kto='%s' and deb != 0.",
gn,mn,dn,gk,mk,dk,gettext("МУ"));
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

sprintf(imaftmp,"vidop%d.tmp",getpid());
if((ff1 = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


kolstr2=0;
kolstr1=0.;
SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  /*Склад*/
  if(iceb_u_proverka(data->rk->sklad.ravno(),row[7],0,0) != 0)
    continue;
   
  /*Читаем документ*/
  sprintf(strsql,"select * from Dokummat where datd='%s' and \
sklad=%s and nomd='%s'",row[14],row[7],row[6]);
/*  printw("strsql=%s\n",strsql);*/
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    iceb_u_str repl;
    sprintf(strsql,gettext("Не найден документ N %s Склад %s Дата=%s\n"),
    row[6],row[7],row[14]);

    repl.plus(strsql);

    sprintf(strsql,"%s %s %s %s",gettext("Проводка"),
    row[1],row[2],row[3]);

    repl.plus(strsql);
    iceb_menu_soob(&repl,data->window);
    continue;
   }

  char nomnalnak[20];
  memset(nomnalnak,'\0',sizeof(nomnalnak));
  strncpy(nomnalnak,row1[5],sizeof(nomnalnak)-1);    
  
  tp=atoi(row1[0]);
  strncpy(kor,row1[3],sizeof(kor)-1);    
  strncpy(kop,row1[6],sizeof(kop)-1);    
  strncpy(nn1,row1[11],sizeof(nn1)-1);    
  strncpy(datd,row1[1],sizeof(datd)-1);    

  if(data->rk->pr_ras != 0 && data->rk->pr_ras != tp)
   continue;

  /*Организация*/
  if(iceb_u_proverka(data->rk->kontr.ravno(),kor,0,0) != 0)
    continue;
   
  /*Операция*/
  if(iceb_u_proverka(data->rk->kodop.ravno(),kop,0,0) != 0)
    continue;

  /*Счет*/
  if(iceb_u_proverka(data->rk->shet.ravno(),row[2],0,0) != 0)
    if(iceb_u_proverka(data->rk->shet.ravno(),row[3],0,0) != 0)
      continue;

  kolstr2++;
//             1   2  3 4  5     6    7   8  9  10 11
  fprintf(ff1,"%d|%s|%s|%s|%s %s|%s|%s,%s|%s|%s|%s|%s|\n",
  tp,kop,row[7],kor,row[6],nn1,row[1],
  row[2],row[3],datd,row[9],row[13],nomnalnak);

 }

fclose(ff1);

//sprintf(strsql,"sort -o %s -t\\| +0n -1 +1 -2 +2n -3 +3 -4 +4 -5 %s",imaftmp,imaftmp);
sprintf(strsql,"sort -o %s -t\\| -k1,2n -k2,3 -k3,4n -k4,5 -k5 %s",imaftmp,imaftmp);
system(strsql);


sprintf(imafitog,"itog%d.lst",getpid());

if((ffitog = fopen(imafitog,"w")) == NULL)
 {
  iceb_er_op_fil(imafitog,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imafitog);

sprintf(strsql,"%s (%s)",
gettext("Реестр проводок по видам операций"),
gettext("только итоги"));

data->rk->naimf.plus(strsql);


iceb_u_zagolov(gettext("Реестр проводок по видам операций"),dn,mn,gn,dk,mk,gk,organ,ffitog);

if((ff1 = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(imaf,"rp%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Реестр проводок по видам операций"));


if(data->rk->sklad.getdlinna() > 1)
 {
  kst++;
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  fprintf(ffitog,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
 }

if(data->rk->kontr.getdlinna() > 1)
 {
  kst++;
  fprintf(ff,"%s:%s\n",gettext("Контрагенты"),data->rk->kontr.ravno());
  fprintf(ffitog,"%s:%s\n",gettext("Контрагенты"),data->rk->kontr.ravno());
 }
if(data->rk->pr_ras != 0)
 {
  kst++;
  memset(strsql,'\0',sizeof(strsql));
  if(data->rk->pr_ras == 1)
   strcpy(strsql,"+");
  if(data->rk->pr_ras == 2)
   strcpy(strsql,"-");
   
  fprintf(ff,"%s: %s\n",gettext("Вид операции"),strsql);
  fprintf(ffitog,"%s: %s\n",gettext("Вид операции"),strsql);
 }
if(data->rk->kodop.getdlinna() > 1)
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ffitog,"%s: %s\n",gettext("Код операции"),data->rk->kodop.ravno());
 }
if(data->rk->shet.getdlinna() > 1)
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Счет"),data->rk->shet.ravno());
  fprintf(ffitog,"%s: %s\n",gettext("Счет"),data->rk->shet.ravno());
 }

skl=0;
memset(kpos,'\0',sizeof(kpos));
memset(sh1,'\0',sizeof(sh1));
memset(nomdok,'\0',sizeof(nomdok));
memset(nomdokv,'\0',sizeof(nomdokv));
memset(kprr1,'\0',sizeof(kprr1));
memset(naik,'\0',sizeof(naik));
memset(naimpr,'\0',sizeof(naimpr));
memset(naiskl,'\0',sizeof(naiskl));

tipzz=kom=0;
kolstr1=0.;
klr=0;
itd=0.;
kome[0]='\0';
short metka_sap=0;

while(fgets(str,sizeof(str),ff1) != NULL)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    

  iceb_u_pole(str,bros,1,'|');
  tz=(int)iceb_u_atof(bros);

  iceb_u_pole(str,kprr,2,'|');

  iceb_u_pole(str,bros,3,'|');
  skl1=(int)iceb_u_atof(bros);

  iceb_u_pole(str,orgn,4,'|');

  iceb_u_pole(str,nomdok,5,'|');

  iceb_u_pole(str,bros,6,'|');
  iceb_u_rsdat(&dp,&mp,&gp,bros,2);

  iceb_u_pole(str,shett,7,'|');

  iceb_u_pole(str,bros,8,'|');
  iceb_u_rsdat(&dd,&md,&gd,bros,2);

  iceb_u_pole(str,bros,9,'|');
  deb=iceb_u_atof(bros);

  iceb_u_pole(str,kome,10,'|');
  iceb_u_polen(str,nomnalnak,sizeof(nomnalnak),11,'|');
   
  if(iceb_u_SRAV(kprr,kprr1,0) != 0 || tipzz != tz)
   {
    if(kprr1[0] != '\0' )
     {
      rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl,naiskl,naik,&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      rasoi(&mssi,&mspsi,dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ff);

      fprintf(ff,"\f");
      kst=0;
     }
    skl=0;
    memset(nomdokv,'\0',sizeof(nomdokv));
    memset(kpos,'\0',sizeof(kpos));

    /*Определяем наименоввание операции*/
    if(tz == 1)
     sprintf(strsql,"select naik from Prihod where kod='%s'",
     kprr);
    if(tz == 2)
     sprintf(strsql,"select naik from Rashod where kod='%s'",
     kprr);
    
    naimpr[0]='\0';
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %s",gettext("Не найден код операции"),kprr);
      iceb_menu_soob(strsql,data->window);
     }
    else
      strncpy(naimpr,row1[0],sizeof(naimpr)-1);
    
    strcpy(kprr1,kprr);
    tipzz=tz;

    sprintf(strsql,"%d/%s %s\n",tz,kprr,naimpr);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    if(tz == 1)
     sprintf(bros,"(%s)",gettext("Приход"));
    if(tz == 2)
     sprintf(bros,"(%s)",gettext("Расход"));

    /*Определяем наименование склада*/
     sprintf(strsql,"select naik from Sklad where kod=%d",
     skl1);
    
    naiskl[0]='\0';
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %d",gettext("Не найден склад"),skl1);
      iceb_menu_soob(strsql,data->window);
     }
    else
      strncpy(naiskl,row1[0],sizeof(naiskl)-1);

    fprintf(ff,"%s\n\
%s %s - \"%s\" %s %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n",
organ,gettext("Реестр проводок по виду операции"),
kprr,naimpr,bros,
gettext("за период с"),
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

    kst+=3;
    klr+=sli;
    sli=0;
    rppvo1(skl1,naiskl,dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
    metka_sap=0;
   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
      rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl,naiskl,naik,&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      fprintf(ff,"\f");
     }
    memset(nomdokv,'\0',sizeof(nomdokv));
    memset(kpos,'\0',sizeof(kpos));
    kom=1;

    /*Определяем наименование склада*/
     sprintf(strsql,"select naik from Sklad where kod=%d",
     skl1);
    
    naiskl[0]='\0';
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %d",gettext("Не найден склад"),skl1);
      iceb_menu_soob(strsql,data->window);
     }
    else
      strncpy(naiskl,row1[0],sizeof(naiskl)-1);

    sprintf(strsql,"%s: %d %s\n",gettext("Склад"),skl1,naiskl);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    /*fprintf(ff,"Склад %d\n",skl1);*/
    skl=skl1;
    if(metka_sap != 0)
     rppvo1(skl1,naiskl,dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
   }

  if(iceb_u_SRAV(kpos,orgn,0) != 0)
   {
    if(kpos[0] != '\0')
     {
      rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,naik,&mso,&mspo,&sli,&kst,ff);
      itd=0.;
     }
    memset(nomdokv,'\0',sizeof(nomdokv));
    kom=1;
    /*Читаем наименование организации*/
    memset(kor,'\0',sizeof(kor));
    if(iceb_u_atof(orgn) == 0. && iceb_u_pole(orgn,bros,2,'-') == 0 && orgn[0] == '0')
     { 
      strcpy(kor,bros);
      sprintf(bros,"select naik from Sklad where kod=%s",kor);
     }
    else
     {
      strcpy(kor,orgn);
      sprintf(bros,"select naikon from Kontragent where kodkon='%s'",kor);
     }

    memset(naik,'\0',sizeof(naik));
    
    if(sql_readkey(&bd,bros,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %s",gettext("Не найден код контрагента"),kor);
      iceb_menu_soob(strsql,data->window);
     }
    else
      strncpy(naik,row1[0],sizeof(naik)-1);

    sprintf(strsql,"%s %s\n",gettext("Контрагент"),orgn);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    strcpy(kpos,orgn);
   }

  if(iceb_u_SRAV(nomdok,nomdokv,0) != 0)
   {
    if(nomdokv[0] != '\0')
     {
      rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,&sli,&kst,ff);
      itd=0.;
     }
    strcpy(nomdokv,nomdok);
   }


  /*Записываем в массив организаций*/
  if((i=mso.find(shett)) < 0)
     mso.plus(shett);
  mspo.plus(deb,i);

   /*Записываем в массив по складу*/
  if((i=mss.find(shett)) < 0)
     mss.plus(shett);
  msps.plus(deb,i);

   /*Записываем в массив по складам */
  if((i=mssi.find(shett)) < 0)
     mssi.plus(shett);
  mspsi.plus(deb,i);
   
   /*Записываем в массив по складам и операциям*/
  if((i=mssio.find(shett)) < 0)
     mssio.plus(shett);
  mspsio.plus(deb,i);
   

  sprintf(strsql,"%02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %-*s %15.2f\n",
  dp,mp,gp,dd,md,gd,
  iceb_u_kolbait(11,nomdok),nomdok,
  iceb_u_kolbait(6,nomnalnak),nomnalnak,
  iceb_u_kolbait(10,shett),shett,
  deb);
  
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  memset(bros,'\0',sizeof(bros));
  metka_sap++;
  
  gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ff);
  if(kom == 1)
     fprintf(ff,"\
%-*s %-*.*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %-*s %20s %s\n",
     iceb_u_kolbait(7,orgn),orgn,
     iceb_u_kolbait(40,naik),iceb_u_kolbait(40,naik),naik,
     dd,md,gd,dp,mp,gp,
     iceb_u_kolbait(11,nomdok),nomdok,
     iceb_u_kolbait(6,nomnalnak),nomnalnak,
     iceb_u_kolbait(10,shett),shett,
     iceb_u_prnbr(deb),kome);

  if(kom == 0)
   fprintf(ff,"\
%48s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %-*s %20s %s\n",
   " ",dd,md,gd,dp,mp,gp,
   iceb_u_kolbait(11,nomdok),nomdok,
   iceb_u_kolbait(6,nomnalnak),nomnalnak,
   iceb_u_kolbait(10,shett),shett,
   iceb_u_prnbr(deb),kome);

  itd+=deb;
  kom=0;

 }

fclose(ff1);
unlink(imaftmp);

rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl,naiskl,naik,&mso,&mspo,&mss,&msps,&sli,&kst,ff);
rasoi(&mssi,&mspsi,dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ff);



char shdeb[32];
char shkre[32];

if(data->rk->kodop.getdlinna() <= 0 || 
(data->rk->kodop.getdlinna() > 1 && iceb_u_pole(data->rk->kodop.ravno(),shett1,2,',') == 0))
 {
  k1=0.;
  fprintf(ff,"\n==================================================\n\
%s\n",gettext("Общий итог по всем операциям:"));
  sapitsh(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ff);
  for(i=0; i< mssio.kolih() ;i++)
   {
    strncpy(shett1,mssio.ravno(i),sizeof(shett1)-1);
    iceb_u_pole(shett1,shdeb,1,',');
    iceb_u_pole(shett1,shkre,2,',');
    fprintf(ff,"%-*s %-*s %15s\n",
    iceb_u_kolbait(6,shdeb),shdeb,
    iceb_u_kolbait(6,shkre),shkre,
    iceb_u_prnbr(mspsio.ravno(i)));
    k1+=mspsio.ravno(i);
   }
  fprintf(ff,"%*s:%15s\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));
 }


sprintf(strsql,"\n%s\n",gettext("Общий итог по всем операциям:"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ffitog,"\n%s\n",gettext("Общий итог по всем операциям:"));
k1=0.;

fprintf(ffitog,"\
------------------------------\n");
fprintf(ffitog,gettext("\
Дебет |Кредит|    Сумма      |\n"));
fprintf(ffitog,"\
------------------------------\n");

for(i=0; i< mssio.kolih() ;i++)
 {
  strncpy(shett1,mssio.ravno(i),sizeof(shett1)-1);
  sprintf(strsql,"%-*s - %15s\n",iceb_u_kolbait(15,shett1),shett1,iceb_u_prnbr(mspsio.ravno(i)));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  iceb_u_pole(shett1,shdeb,1,',');
  iceb_u_pole(shett1,shkre,2,',');
  fprintf(ffitog,"%-*s %-*s %15s\n",
  iceb_u_kolbait(6,shdeb),shdeb,
  iceb_u_kolbait(6,shkre),shkre,
  iceb_u_prnbr(mspsio.ravno(i)));
  k1+=mspsio.ravno(i);
 }
fprintf(ffitog,"%*s:%15s\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));

iceb_podpis(ffitog,data->window);
iceb_podpis(ff,data->window);

fclose(ffitog);
fclose(ff);


klr+=sli;
sprintf(strsql,"%s: %d\n",gettext("Количество листов"),klr);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


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
/******************************/
/* Шапка итога по счетам      */
 /******************************/

void  sapitsh(short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int skl1,char *naiskl,FILE *ff)
{
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
------------------------------\n");

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,gettext("\
Дебет |Кредит|    Сумма      |\n"));

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
------------------------------\n");

}
/*******************************/
/*Распечатка итога по контрагенту*/
/*******************************/
void            rasio(char orgn[],double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,char naiskl[],char naik[],
iceb_u_spisok *mso,
iceb_u_double *mspo,
int *sli,
int *kst,
FILE *ff)
{
short           i;
char            shett1[112];
double          bb;
char		shdeb[56];
char		shkre[56];


rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,sli,kst,ff);

/*Распечатываем результаты по организации*/
bb=0.;
if(mso->kolih() > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%s \"%s\":\n",gettext("Итого по контрагенту"),naik);
 }
/*printw("kso- %d\n",kso);*/

sapitsh(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
//sapitsh(ff);

for(i=0; i< mso->kolih() ;i++)
 {
  strncpy(shett1,mso->ravno(i),sizeof(shett1)-1);
  iceb_u_pole(shett1,shdeb,1,',');
  iceb_u_pole(shett1,shkre,2,',');

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s %-*s %15s\n",
  iceb_u_kolbait(6,shdeb),shdeb,
  iceb_u_kolbait(6,shkre),shkre,
  iceb_u_prnbr(mspo->ravno(i)));

  bb+=mspo->ravno(i);
 }
if(mso->kolih() > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%*s:%15s\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
 }

mso->free_class();
mspo->free_class();
}

/*******************************/
/*Распечатка итога по складу*/
/*******************************/
void            rasis(char orgn[],double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,char naiskl[],char naik[],
iceb_u_spisok *mso,
iceb_u_double *mspo,
iceb_u_spisok *mss,
iceb_u_double *msps,
int *sli,
int *kst,
FILE *ff)
{
short           i;
char            shett1[112];
double          bb;
char		shdeb[56];
char		shkre[56];

rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,naik,mso,mspo,sli,kst,ff);

/*Распечатываем результаты по складу*/
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%s \"%s\":\n",gettext("Итого по складу"),naiskl);

sapitsh(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
//sapitsh(ff);

bb=0.;
//for(i=0; i< kss ;i++)
for(i=0; i< mss->kolih() ;i++)
 {
  strncpy(shett1,mss->ravno(i),sizeof(shett1)-1);

  iceb_u_pole(shett1,shdeb,1,',');
  iceb_u_pole(shett1,shkre,2,',');

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s %-*s %15s\n",
  iceb_u_kolbait(6,shdeb),shdeb,
  iceb_u_kolbait(6,shkre),shkre,
  iceb_u_prnbr(msps->ravno(i)));

  bb+=msps->ravno(i);
 }

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%*s:%15s\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
......................................................................................................................................\n");

mss->free_class();
msps->free_class();
}

/*************************/
/*Распечатка общего итога*/
/*************************/
void            rasoi(iceb_u_spisok *mssi,
iceb_u_double *mspsi,
short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int skl1,char *naiskl,
FILE *ff)
{
double          k1;
short           i;
char            shett1[112];
char		shdeb[56];
char		shkre[56];


k1=0.;
fprintf(ff,"\n%s:\n",gettext("Общий итог"));

sapitsh(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
//sapitsh(ff);
for(i=0; i< mssi->kolih() ;i++)
 {
  strncpy(shett1,mssi->ravno(i),sizeof(shett1)-1);
  iceb_u_pole(shett1,shdeb,1,',');
  iceb_u_pole(shett1,shkre,2,',');
  fprintf(ff,"%-*s %-*s %15s\n",
  iceb_u_kolbait(6,shdeb),shdeb,
  iceb_u_kolbait(6,shkre),shkre,
  iceb_u_prnbr(mspsi->ravno(i)));
  k1+=mspsi->ravno(i);
 }
fprintf(ff,"%*s:%15s\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));


mspsi->free_class();
mssi->free_class();
}
/*******************************/
/*Распечатка итога по документа*/
/*******************************/
void            rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,char naiskl[],int *sli,int *kst,FILE *ff)
{
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);

fprintf(ff,"%*s: %20s\n",
iceb_u_kolbait(99,gettext("Итого по документу")),
gettext("Итого по документу"),
iceb_u_prnbr(itd));

}
/*******/
/*Шапка*/
/*******/
void            gsapr(short dn,short mn,short gn,short dk,short mk,
short gk,int *sl,int *kst,int skl1,char naiskl[],FILE *ff)
{
*kst+=1;

if(*kst <=  kol_strok_na_liste)
 return;

fprintf(ff,"\f");
*kst=1;
rppvo1(skl1,naiskl,dn,mn,gn,dk,mk,gk,sl,kst,ff);
  

}
/************************************/
/*Шапка*/
/************************************/
void rppvo1(int skl1,char *naiskl,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
FILE *ff)
{
*sl+=1;
*kst+=4;

fprintf(ff,"\
%s N%-2d %-30s %s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %30s%s N%d\n",
gettext("Склад"),
skl1,naiskl,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
"",gettext("Лист"),
*sl);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
        Контрагент                              |Дата док. |Дата пров.|N документа|Nнал.н|  Счета   |       Сумма        |Коментарий\n"));
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------\n");

}
