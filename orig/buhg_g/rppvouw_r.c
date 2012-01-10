/*$Id: rppvouw_r.c,v 1.18 2011-02-21 07:35:57 sasa Exp $*/
/*14.03.2010	01.02.2005	Белых А.И.	rppvouw_r.c
Расчёт 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "rppvouw.h"

class rppvouw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class rppvouw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  rppvouw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rppvouw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rppvouw_r_data *data);
gint rppvouw_r1(class rppvouw_r_data *data);
void  rppvouw_r_v_knopka(GtkWidget *widget,class rppvouw_r_data *data);

void            gsapr(short,short,short,short,short,short,int*,int*,int,char[],FILE*);
void            rasid(double,short,short,short,short,short,short,int,char[],int*,int*,FILE*);
void            rasio(char[],double,short,short,short,short,short,short,int,char[],char[],class iceb_u_spisok *mso,
class iceb_u_double *mspo,int*,int*,FILE*);
void            rasis(char[],double,short,short,short,short,short,short,int,char[],char[],class iceb_u_spisok *mso,
class iceb_u_double *mspo,class iceb_u_spisok *mss,class iceb_u_double *msps,int*,int*,FILE*);
void            rasoi(class iceb_u_spisok *mssi,class iceb_u_double *mspsi,FILE*);
void rppvou1_sap(short dn,short mn,short gn,short dk,short mk,
short gk,int *sl,int *kst,int skl1,char naiskl[],FILE *ff);
void rppvouw_r_rp(const char *podr,const char *kontr,int prihr,const char *kodop,const char *shet,int *kst,FILE *ff);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double	nds1;
extern double   okrg1;
extern double	okrcn;
extern int kol_strok_na_liste;

int rppvouw_r(class rppvouw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class rppvouw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать реестр проводок по видам операций"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rppvouw_r_key_press),&data);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(rppvouw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)rppvouw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rppvouw_r_v_knopka(GtkWidget *widget,class rppvouw_r_data *data)
{
//printf("rppvouw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rppvouw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rppvouw_r_data *data)
{
// printf("rppvouw_r_key_press\n");
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

gint rppvouw_r1(class rppvouw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

struct tm *bf;
bf=localtime(&vremn);
short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"select datp,sh,shk,nomd,pod,deb,datd,komen from Prov \
where val=0 and \
datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and kto='%s' and deb != 0.",
gn,mn,dn,gk,mk,dk,gettext("УСЛ"));
SQLCURSOR cur;
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
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    
char imaftmp[30];

sprintf(imaftmp,"uslrp%d.tmp",getpid());
FILE *ff1;
if((ff1 = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d\n",gettext("Период с"),dn,mn,gn,gettext("по"),
dk,mk,gk);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int kolstr2=0;
float kolstr1=0.;
SQL_str row;
SQL_str row1;
SQLCURSOR cur1;
short tp=0;
char datd[16];
char kontr[32];
char kodop[32];
char nn1[32];

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  /*Подразделение*/
  if(iceb_u_proverka(data->rk->podr.ravno(),row[4],0,0) != 0)
    continue;

  /*Счет*/
  if(iceb_u_proverka(data->rk->shet.ravno(),row[1],0,0) != 0)
    continue;
   
  /*Читаем документ*/
  sprintf(strsql,"select tp,datd,kontr,kodop,nomnn from Usldokum where datd='%s' and \
podr=%s and nomd='%s'",row[6],row[4],row[3]);
/*  printw("strsql=%s\n",strsql);*/
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"Не найден документ N %s Подразделение %s Дата=%s\n",
    row[3],row[4],row[6]);

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    sprintf(strsql,"%s %s %s %s\n","Проводка",row[1],row[2],row[3]);

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    continue;
   }
  tp=atoi(row1[0]);
  if(data->rk->pr_ras != 0 && data->rk->pr_ras != tp)
   continue;

  /*Организация*/
  if(iceb_u_proverka(data->rk->kontr.ravno(),row1[2],0,0) != 0)
    continue;
   
  /*Операция*/
  if(iceb_u_proverka(data->rk->kodop.ravno(),row1[3],0,0) != 0)
    continue;

  strncpy(datd,row1[1],sizeof(datd)-1);    
  strncpy(kontr,row1[2],sizeof(kontr)-1);    
  strncpy(kodop,row1[3],sizeof(kodop)-1);    
  strncpy(nn1,row1[4],sizeof(nn1)-1);    

  kolstr2++;
  fprintf(ff1,"%d|%s|%s|%s|%s %s|%s|%s,%s|%s|%s|%s|\n",
  tp,kodop,row[4],kontr,row[3],nn1,row[0],
  row[1],row[2],datd,row[5],row[7]);

 }

fclose(ff1);

//sprintf(strsql,"sort -o %s -t\\| +0n -1 +1 -2 +2n -3 +3 -4 +4 -5 %s",imaftmp,imaftmp);
sprintf(strsql,"sort -o %s -t\\| -k1,2n -k2,3 -k3,4n -k4,5 -k5 %s",imaftmp,imaftmp);
system(strsql);


class iceb_u_spisok mssi;
class iceb_u_spisok mssio;
class iceb_u_double mspsi;
class iceb_u_double mspsio;

class iceb_u_spisok mso; //Массив счетов по организации
class iceb_u_double mspo;
class iceb_u_spisok mss; //Массив счетов по организации
class iceb_u_double msps;

if((ff1 = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
unlink(imaftmp);
char imaf[30];

sprintf(imaf,"uslrp%d.lst",getpid());

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kst=0;
int sli=0;

rppvouw_r_rp(data->rk->podr.ravno(),data->rk->kontr.ravno(),data->rk->pr_ras,data->rk->kodop.ravno(),data->rk->shet.ravno(),&kst,ff);



int skl=0;
char kpos[32];
char sh1[32];
char nomdok[32];
char nomdokv[32];
char kprr1[32];
char naik[512];
char naimpr[512];
char naiskl[512];

memset(kpos,'\0',sizeof(kpos));
memset(sh1,'\0',sizeof(sh1));
memset(nomdok,'\0',sizeof(nomdok));
memset(nomdokv,'\0',sizeof(nomdokv));
memset(kprr1,'\0',sizeof(kprr1));
memset(naik,'\0',sizeof(naik));
memset(naimpr,'\0',sizeof(naimpr));
memset(naiskl,'\0',sizeof(naiskl));

short tipzz=0,kom=0;
kolstr1=0.;
int klr=0;
double itd=0.;
char kome[512];
kome[0]='\0';
int nomer=0;
short tz=0;
char str[1024];
char orgn[32];
int skl1=0;
char prih_ras[56];
char kprr[32];
short dp,mp,gp;
short dd,md,gd;
double deb=0.;
char shett[32];
char shettz[32];
memset(shettz,'\0',sizeof(shettz));
while(fgets(str,sizeof(str),ff1) != NULL)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    
/*  printw("kprr-%s kprr1-%s tipzz=%d tz=%d\n",kprr,kprr1,tipzz,tz);*/
  iceb_u_pole(str,strsql,1,'|');
  tz=atoi(strsql);

  iceb_u_pole(str,kprr,2,'|');

  iceb_u_pole(str,strsql,3,'|');
  skl1=atoi(strsql);

  iceb_u_pole(str,orgn,4,'|');

  iceb_u_pole(str,nomdok,5,'|');

  iceb_u_pole(str,strsql,6,'|');
  iceb_u_rsdat(&dp,&mp,&gp,strsql,2);

  iceb_u_pole(str,shett,7,'|');

  iceb_u_pole(str,strsql,8,'|');
  iceb_u_rsdat(&dd,&md,&gd,strsql,2);

  iceb_u_pole(str,strsql,9,'|');
  deb=atof(strsql);

  iceb_u_polen(str,kome,sizeof(kome),10,'|');
   
  if(iceb_u_SRAV(kprr,kprr1,0) != 0 || tipzz != tz)
   {
    if(kprr1[0] != '\0' )
     {
      rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,naik,&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      rasoi(&mssi,&mspsi,ff);

      fprintf(ff,"\f");
      kst=0;
     }
    skl=0;
    memset(nomdokv,'\0',sizeof(nomdokv));
    memset(kpos,'\0',sizeof(kpos));

    /*Определяем наименоввание операции*/
    if(tz == 1)
     sprintf(strsql,"select naik from Usloper1 where kod='%s'",
     kprr);
    if(tz == 2)
     sprintf(strsql,"select naik from Usloper2 where kod='%s'",
     kprr);
    
    naimpr[0]='\0';
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       sprintf(strsql,"Не нашли операцию %s",kprr);
      iceb_menu_soob(strsql,data->window);
     }
    else
      strncpy(naimpr,row1[0],sizeof(naimpr)-1);
    
    strcpy(kprr1,kprr);
    tipzz=tz;

    sprintf(strsql,"%d/%s %s\n",tz,kprr,naimpr);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    memset(prih_ras,'\0',sizeof(prih_ras));
    if(tz == 1)
     sprintf(prih_ras,"(%s)",gettext("Приход"));
    if(tz == 2)
     sprintf(prih_ras,"(%s)",gettext("Расход"));

    /*Определяем наименование подразделения*/
     sprintf(strsql,"select naik from Uslpodr where kod=%d",
     skl1);
    
    naiskl[0]='\0';
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),skl1);
       iceb_menu_soob(strsql,data->window);
     }
    else
      strncpy(naiskl,row1[0],sizeof(naiskl)-1);

    fprintf(ff,"%s\n\
%s %s - \"%s\" %s %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n",
organ,gettext("Реестр проводок по виду операции"),
kprr,naimpr,prih_ras,
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
    rppvou1_sap(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ff);

   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
      rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,naik,&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      klr+=sli;
      sli=0;
      kst=0;
      fprintf(ff,"\f");
     }
    memset(shettz,'\0',sizeof(shettz));
    memset(nomdokv,'\0',sizeof(nomdokv));
    memset(kpos,'\0',sizeof(kpos));
    kom=1;

    /*Определяем наименование подразделения*/
     sprintf(strsql,"select naik from Uslpodr where kod=%d",
     skl1);
    
    naiskl[0]='\0';
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),skl1);
       iceb_menu_soob(strsql,data->window);
     }
    else
      strncpy(naiskl,row1[0],sizeof(naiskl)-1);

    sprintf(strsql,"%s: %d %s\n",gettext("Подразделение"),skl1,naiskl);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    skl=skl1;
   }

  if(iceb_u_SRAV(kpos,orgn,0) != 0)
   {
    if(kpos[0] != '\0')
     {
      rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,naik,&mso,&mspo,&sli,&kst,ff);
      itd=0.;
     }
    memset(nomdokv,'\0',sizeof(nomdokv));
    memset(shettz,'\0',sizeof(shettz));
    kom=1;

    /*Читаем наименование организации*/
    memset(kontr,'\0',sizeof(kontr));
    if(atof(orgn) == 0. && iceb_u_pole(orgn,strsql,1,'-') == 0 && orgn[0] == '0')
      strcpy(kontr,strsql);
    else
      strcpy(kontr,orgn);

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr);
    naik[0]='\0';
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr);
       iceb_menu_soob(strsql,data->window);
     }
    else
      strncpy(naik,row1[0],sizeof(naik)-1);

    sprintf(strsql,gettext("Контрагент %s\n"),orgn);
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
  if((nomer=mso.find(shett)) < 0)
      mso.plus(shett);
  mspo.plus(deb,nomer);
   
   /*Записываем в массив по подразделению*/
  if((nomer=mss.find(shett)) < 0)
      mss.plus(shett);
  msps.plus(deb,nomer);

   /*Записываем в массив по подразделениям */
  if((nomer=mssi.find(shett)) < 0)
      mssi.plus(shett);
  mspsi.plus(deb,nomer);

   /*Записываем в массив по подразделениям и операциям*/
  if((nomer=mssio.find(shett)) < 0)
      mssio.plus(shett);
  mspsio.plus(deb,nomer);

   gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ff);

   sprintf(strsql,"%02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %15.2f\n",
   dp,mp,gp,dd,md,gd,
   iceb_u_kolbait(11,nomdok),nomdok,
   iceb_u_kolbait(11,shett),shett,
   deb);
   iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

   if(kom == 1)
     fprintf(ff,"\
%-*s %-*.*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s %.13s\n",
     iceb_u_kolbait(7,orgn),orgn,
     iceb_u_kolbait(40,naik),iceb_u_kolbait(40,naik),naik,
     dd,md,gd,dp,mp,gp,
     iceb_u_kolbait(11,nomdok),nomdok,
     iceb_u_kolbait(11,shett),shett,
     iceb_u_prnbr(deb),kome);

   if(kom == 0)
     fprintf(ff,"\
%48s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s %.13s\n",
     " ",dd,md,gd,dp,mp,gp,
     iceb_u_kolbait(11,nomdok),nomdok,
     iceb_u_kolbait(11,shett),shett,
     iceb_u_prnbr(deb),kome);

   itd+=deb;
   kom=0;

 }
fclose(ff1);
rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,naik,&mso,&mspo,&mss,&msps,&sli,&kst,ff);
rasoi(&mssi,&mspsi,ff);
int kssio=mssio.kolih();
char shett1[20];
double k1=0.;
if(data->rk->kodop.ravno()[0] == '\0' || ( data->rk->kodop.ravno()[0] != '\0' && iceb_u_pole(data->rk->kodop.ravno(),shett1,2,',') == 0))
 {
  k1=0.;
  fprintf(ff,"\n==================================================\n\
%s\n",gettext("Общий итог по всем операциям:"));
  for(int i=0; i< kssio ;i++)
   {
//    iceb_u_pole(mssio,shett1,i+1,' ');
    fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,mssio.ravno(i)),mssio.ravno(i),iceb_u_prnbr(mspsio.ravno(i)));
    k1+=mspsio.ravno(i);
   }
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));
 }

sprintf(strsql,"\n%s\n",gettext("Общий итог по всем операциям:"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

char imaf_oi[50];
sprintf(imaf_oi,"rpoi%d.lst",getpid());

if((ff1 = fopen(imaf_oi,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_oi,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(gettext("Реестр проводок по видам операций"),dn,mn,gn,dk,mk,gk,organ,ff1);

rppvouw_r_rp(data->rk->podr.ravno(),data->rk->kontr.ravno(),data->rk->pr_ras,data->rk->kodop.ravno(),data->rk->shet.ravno(),&kst,ff1);

fprintf(ff1,"%s\n",gettext("Общий итог по всем операциям:"));

for(int i=0; i< kssio ;i++)
 {
  sprintf(strsql,"%-*s - %20s\n",iceb_u_kolbait(10,mssio.ravno(i)),mssio.ravno(i),iceb_u_prnbr(mspsio.ravno(i)));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  fprintf(ff1,"%-*s - %20s\n",iceb_u_kolbait(10,mssio.ravno(i)),mssio.ravno(i),iceb_u_prnbr(mspsio.ravno(i)));
 }

iceb_podpis(ff1,data->window);
fclose(ff1);


fclose(ff);


data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Реестр проводок по видам операций"));

data->rk->imaf.plus(imaf_oi);
data->rk->naimf.plus(gettext("Общие итоги"));

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
/*******/
/*Шапка*/
/*******/
void  gsapr(short dn,short mn,short gn,short dk,short mk,
short gk,int *sl,int *kst,int skl1,char naiskl[],FILE *ff)
{
*kst+=1;

if(*kst <=  kol_strok_na_liste)
 return;

fprintf(ff,"\f");
*kst=1;
rppvou1_sap(dn,mn,gn,dk,mk,gk,sl,kst,skl1,naiskl,ff);


}

/*******************************/
/*Распечатка итога по документа*/
/*******************************/
void            rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,char naiskl[],int *sli,int *kst,FILE *ff)
{
 gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
 fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(94,gettext("Итого по документу")),gettext("Итого по документу"),iceb_u_prnbr(itd));
}

/*******************************/
/*Распечатка итога по организации*/
/*******************************/
void rasio(char orgn[],double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,char naiskl[],char naik[],
class iceb_u_spisok *mso,
class iceb_u_double *mspo,
int *sli,
int *kst,
FILE *ff)
{
short           i;
char            shett1[32];
double          bb;

rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,sli,kst,ff);

/*Распечатываем результаты по организации*/
int kso=mso->kolih();
bb=0.;
if(kso > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%s \"%s\":\n",gettext("Итого по контрагенту"),naik);
 }
/*printw("kso- %d\n",kso);*/

for(i=0; i< kso ;i++)
 {
  strncpy(shett1,mso->ravno(i),sizeof(shett1)-1);

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,shett1),shett1,iceb_u_prnbr(mspo->ravno(i)));

  bb+=mspo->ravno(i);
 }
if(kso > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
 }
mspo->free_class();
mso->free_class();
}
/*******************************/
/*Распечатка итога по подразделению*/
/*******************************/
void            rasis(char orgn[],double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,char naiskl[],char naik[],
class iceb_u_spisok *mso,
class iceb_u_double *mspo,
class iceb_u_spisok *mss,
class iceb_u_double *msps,
int *sli,
int *kst,
FILE *ff)
{
short           i;
double          bb;

rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,naik,mso,mspo,sli,kst,ff);

/*Распечатываем результаты по подразделению*/
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%s \"%s\":\n",gettext("Итого по подразделению"),naiskl);
bb=0.;
int kss=mss->kolih();
for(i=0; i< kss ;i++)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,mss->ravno(i)),mss->ravno(i),iceb_u_prnbr(msps->ravno(i)));
  bb+=msps->ravno(i);
 }
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
...............................................................................................................................\n");

msps->free_class();
mss->free_class();

}

/*************************/
/*Распечатка общего итога*/
/*************************/
void            rasoi(class iceb_u_spisok *mssi,class iceb_u_double *mspsi,FILE *ff)
{
double          k1;
short           i;

k1=0.;
fprintf(ff,"\n%s:\n",gettext("Общий итог"));
int kssi=mssi->kolih();
for(i=0; i< kssi ;i++)
 {
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,mssi->ravno(i)),mssi->ravno(i),iceb_u_prnbr(mspsi->ravno(i)));
  k1+=mspsi->ravno(i);
 }
fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));
mspsi->free_class();
mssi->free_class();

}
/***********************/
/*Шапка*/
/***************/
void rppvou1_sap(short dn,short mn,short gn,short dk,short mk,
short gk,int *sl,int *kst,int skl1,char naiskl[],FILE *ff)
{

*sl+=1;
*kst+=4;

fprintf(ff,"\
%s N%-2d %-30s %s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %30s%sN%d\n",
gettext("Подразделение"),
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
-------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext(" 	 Контрагент                              |Дата док. |Дата пров.|N документа|  Счета   |       Сумма        |Коментарий\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");

}
/***********************************/
/*Реквизиты поиска*/
/*************************/
void rppvouw_r_rp(const char *podr,const char *kontr,int prihr,const char *kodop,const char *shet,int *kst,FILE *ff)
{
if(podr[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Код подразделения"),podr);
 }
if(kontr[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Контрагент"),kontr);
 }
if(prihr != 0)
 {
  *kst+=1;

  if(prihr == 1)
   fprintf(ff,"%s:+\n",gettext("Вид операции"));
  if(prihr == 2)
   fprintf(ff,"%s:-\n",gettext("Вид операции"));
 }
if(kodop[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Код операции"),kodop);
 }
if(shet[0] != '\0')
 {
  *kst+=1;
  fprintf(ff,"%s: %s\n",gettext("Счет"),shet);
 }
}
