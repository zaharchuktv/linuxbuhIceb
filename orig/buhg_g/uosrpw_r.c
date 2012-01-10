/*$Id: uosrpw_r.c,v 1.11 2011-02-21 07:35:58 sasa Exp $*/
/*11.01.2010	01.01.2008	Белых А.И.	uosrpw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "uosrpw.h"
#define         PL      52  /*Количество строк на первом листе*/
#define         VL      55  /*Количество строк на втором листе*/

class uosrpw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class uosrpw_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  uosrpw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosrpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosrpw_r_data *data);
gint uosrpw_r1(class uosrpw_r_data *data);
void  uosrpw_r_v_knopka(GtkWidget *widget,class uosrpw_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int uosrpw_r(class uosrpw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class uosrpw_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Реестр проводок по видам операций"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosrpw_r_key_press),&data);

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

repl.plus(gettext("Реестр проводок по видам операций"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(uosrpw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)uosrpw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosrpw_r_v_knopka(GtkWidget *widget,class uosrpw_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosrpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosrpw_r_data *data)
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

/*******/
/*Шапка*/
/*******/
void            gsapr(short dn,short mn,short gn,short dk,short mk,
short gk,short *sl,short *kst,int skl1,char naiskl[],FILE *ff)
{
*kst=*kst+1;


if((*kst >= VL && *sl > 1) || (*kst >= PL && *sl == 1) ||
(*kst == 0 && *sl == 0))
 {
  if((*kst >= VL && *sl > 1) || (*kst >= PL && *sl == 1))
      fprintf(ff,"\f");
  *sl=*sl+1;
  *kst=1;

  fprintf(ff,"\
%s N%-2d %-30s %s %02d.%02d.%4d%s %s %02d.%02d.%4d%s                               %s N%d\n",
gettext("Подразделение"),
skl1,naiskl,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("Лист"),
*sl);

  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
	        Контрагент                                  |Дата док. |Дата пров.|N документа|  Счета   |       Сумма        |Коментарий\n"));
  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n");
 }

}

/*******************************/
/*Распечатка итога по документа*/
/*******************************/
void            rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,char naiskl[],short *sli,short *kst,FILE *ff)
{
 fprintf(ff,"%*s: %20s\n",iceb_u_kolbait(106,gettext("Итого по документу")),gettext("Итого по документу"),iceb_u_prnbr(itd));
 gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
}

/*******************************/
/*Распечатка итога по организации*/
/*******************************/
void            rasio(char orgn[],double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,
char naikont[], //Наименование контрагента
char naiskl[],  //Наименование подразделения
short *sli,short *kst,
class iceb_u_spisok *mso,
class iceb_u_double *mspo,
FILE *ff)
{
short           i;
//char            shett1[20];
double          bb;

rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,sli,kst,ff);

/*Распечатываем результаты по организации*/
bb=0.;
if(mso->kolih() > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%s \"%s\":\n",gettext("Итого по контрагенту"),naikont);
 }

for(i=0; i < mso->kolih() ;i++)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,mso->ravno(i)),mso->ravno(i),iceb_u_prnbr(mspo->ravno(i)));
  bb+=mspo->ravno(i);
 }
if(mso->kolih() > 0)
 {
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));
  kst++;
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
 }
mso->free_class();
mspo->free_class();
}
/*******************************/
/*Распечатка итога по складу*/
/*******************************/
void rasis(char orgn[],double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,char naikont[],char naiskl[],
short *sli,short *kst,
class iceb_u_spisok *mso,
class iceb_u_double *mspo,
class iceb_u_spisok *mss,
class iceb_u_double *msps,
FILE *ff)
{
short           i;
//char            shett1[20];
double          bb;

rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naikont,naiskl,sli,kst,mso,mspo,ff);

/*Распечатываем результаты по складу*/
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%s \"%s\":\n",gettext("Итого по подразделению"),naiskl);
bb=0.;
for(i=0; i< msps->kolih() ;i++)
 {
  //POLE(mss,shett1,i+1,' ');
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,mss->ravno(i)),mss->ravno(i),iceb_u_prnbr(msps->ravno(i)));
  bb+=msps->ravno(i);
 }
fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));

fprintf(ff,"\
...............................................................................................................................\n");
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
mss->free_class();
msps->free_class();
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
for(i=0; i< mssi->kolih() ;i++)
 {
  //POLE(mssi,shett1,i+1,' ');
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,mssi->ravno(i)),mssi->ravno(i),iceb_u_prnbr(mspsi->ravno(i)));
  k1+=mspsi->ravno(i);
 }
fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));
mssi->free_class();
mspsi->free_class();
}
/****************************/
/*Печать реквизитов поиска*/
/****************************/
void uosrpw_r_prp(class uosrpw_r_data *data,FILE *ff)
{
if(data->rk->podr.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Подразделение"),data->rk->podr.ravno());
if(data->rk->kontr.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Код контрагента"),data->rk->kontr.ravno());

if(data->rk->prih_rash != 0)
 {
  fprintf(ff,"%s:",gettext("Документы"));
  if(data->rk->prih_rash == 1)
   fprintf(ff,"%s\n",gettext("Только приходные документы"));
  if(data->rk->prih_rash == 2)
   fprintf(ff,"%s\n",gettext("Только расходные документы"));
 }

if(data->rk->kod_op.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Код операции"),data->rk->kod_op.ravno());
if(data->rk->shetu.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Счет"),data->rk->shetu.ravno());
}

/******************************************/
/******************************************/

gint uosrpw_r1(class uosrpw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;




class iceb_u_spisok mso;
class iceb_u_double mspo;
class iceb_u_spisok mss;  /*Массив счетов по складу*/
class iceb_u_double msps; /*Массив сумм по складу*/
class iceb_u_spisok mssi; /*Массив счетов по всем складам*/
class iceb_u_double mspsi; /*Массив сумм проводок по всем складам по операции*/
class iceb_u_spisok mssio; /*Массив счетов по всем складам и всем операциям*/
class iceb_u_double mspsio; /*Массив сумм проводок по всем складам по всем операциям*/

short           kst,sli;
FILE            *ff;
FILE            *ffitog;
FILE            *ff1;
char            kop[100];
char		str[300];
char		bros[512];
char		naiskl[40];
short           kom;
char		kome[512]; /*Коментарий*/
long            muf; /*Количество записей в файле*/
short           dp,mp,gp;
short           d,m,g;
short           dd,md,gd;
char            orgn[20];
char            imaf[30],imaftmp[30],imafitog[30];
char            sh1[32];
char            shett[20];
char            shett1[20];
char            shettz[20];
double          itd; /*Итого по счету*/
int             i;
double          k1;
short           tipzz;
int		tz; /*1-Приход 2-расход*/
short           klr;
short           dn,mn,gn;
short           dk,mk,gk;
double		deb;
int		skl=0,skl1=0;
char		nomdok[32];
char		nomdokv[20];
char		kprr[32],kprr1[32];		
char		naimpr[40];
long		kolstr=0;
float kolstr1=0.;
SQL_str         row1;
char		kontr[20];
int		tipz;
char		naikont[40];
SQLCURSOR curr;



tz=data->rk->prih_rash;

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

sprintf(strsql,"select datp,sh,shk,kodkon,nomd,pod,oper,deb,kre,\
datd,komen from Prov where val=0 and datp>='%d-%d-%d' and \
datp <= '%d-%d-%d' and kto='%s' and deb <> 0. order by datp asc",
gn,mn,dn,gk,mk,dk,gettext("УОС"));


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

sprintf(imaftmp,"uosrp%d.tmp",getpid());

if((ff1 = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s: %d.%d.%d%s => %d.%d.%d%s\n",
gettext("Дата"),
dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s\n",gettext("Сортируем записи"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

kolstr1=0.;
muf=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  /*Подразделение*/
  if(iceb_u_proverka(data->rk->podr.ravno(),row[5],0,0) != 0)
    continue;
   
  /*Читаем документ*/
  sprintf(strsql,"select tipz,kodop,kontr,nomdv \
from Uosdok where datd='%s' and nomd='%s'",
  row[9],row[4]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
   {
    sprintf(strsql,"%s N%s %s %s !",gettext("Не найден документ !"),
    row[4],gettext("Дата"),row[9]);
    iceb_menu_soob(strsql,data->window);
    continue;
   }  
  tipz=atoi(row1[0]);
  strncpy(kprr,row1[1],sizeof(kprr)-1);
  strncpy(kontr,row1[2],sizeof(kontr)-1);
  strncpy(nomdokv,row1[3],sizeof(nomdokv)-1);
  if(tz != 0 && tz != tipz)
   continue;

  /*Организация*/
  if(iceb_u_proverka(data->rk->kontr.ravno(),kontr,0,0) != 0)
    continue;

  /*Операция*/
  if(iceb_u_proverka(data->rk->kod_op.ravno(),kprr,0,0) != 0)
    continue;


  /*Счет*/
  if(iceb_u_proverka(data->rk->shetu.ravno(),row[1],0,0) != 0)
    continue;
      
  muf++;  
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  iceb_u_rsdat(&dd,&md,&gd,row[9],2);
  
  fprintf(ff1,"%d|%s|%s|%s|%s %s|%d|%d|%d|%s,%s|%d|%d|%d|%.2f|%s|\n",
  tipz,kprr,row[5],kontr,row[4],nomdokv,g,m,d,
  row[1],row[2],dd,md,gd,atof(row[7]),row[10]);

 }

fclose(ff1);

if(muf == 0)
 {
  unlink(imaftmp);
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

//sprintf(strsql,"sort -o %s -t\\| +0n +1 +2n +3n +4 %s",imaftmp,imaftmp);
sprintf(strsql,"sort -o %s -t\\| -k1,2n -k2,3 -k3,4n -k4,5n -k5,6 %s",imaftmp,imaftmp);
system(strsql);


if((ff1 = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(imaf,"rp%d%d.lst",mn,mk);

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


uosrpw_r_prp(data,ff); /*Печать реквизитов поиска*/


skl=0;
memset(kontr,'\0',sizeof(kontr));
memset(sh1,'\0',sizeof(sh1));
memset(nomdok,'\0',sizeof(nomdok));
memset(nomdokv,'\0',sizeof(nomdokv));
memset(kprr1,'\0',sizeof(kprr1));

tipzz=kom=0;
klr=sli=0;
kst=-1;
itd=0.;
kome[0]='\0';

kolstr1=0;
while(fgets(str,sizeof(str),ff1) != NULL)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_polen(str,&tz,1,'|');

  iceb_u_polen(str,kprr,sizeof(kprr),2,'|');

  iceb_u_polen(str,&skl1,3,'|');

  iceb_u_polen(str,orgn,sizeof(orgn),4,'|');

  iceb_u_polen(str,nomdok,sizeof(nomdok),5,'|');

  iceb_u_polen(str,&gp,6,'|');

  iceb_u_polen(str,&mp,7,'|');

  iceb_u_polen(str,&dp,8,'|');

  iceb_u_polen(str,shett,sizeof(shett),9,'|');

  iceb_u_polen(str,&dd,10,'|');

  iceb_u_polen(str,&md,11,'|');

  iceb_u_polen(str,&gd,12,'|');

  iceb_u_polen(str,&deb,13,'|');

  iceb_u_polen(str,kome,sizeof(kome),14,'|');
   
  if(iceb_u_SRAV(kprr,kprr1,0) != 0 || tipzz != tz)
   {
    if(kprr1[0] != '\0' )
     {
      rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naikont,naiskl,&sli,&kst,&mso,&mspo,&mss,&msps,ff);
      itd=0.;
      rasoi(&mssi,&mspsi,ff);


      fprintf(ff,"\n\n\n\f");
     }
    skl=0;
    memset(nomdokv,'\0',sizeof(nomdokv));
    memset(kontr,'\0',sizeof(kontr));

    /*Читаем наименование операции*/
    memset(naimpr,'\0',sizeof(naimpr));
    if(tz == 1)
     sprintf(strsql,"select naik from Uospri where kod='%s'",kprr);
    if(tz == 2)
     sprintf(strsql,"select naik from Uosras where kod='%s'",kprr);

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %s!\n",gettext("Не нашли код операции"),kprr);
      iceb_menu_soob(strsql,data->window);

      fprintf(ff,"%s %s!\n",gettext("Не нашли код операции"),kprr);
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

    /*Читаем наименование подразделения*/
    memset(naiskl,'\0',sizeof(naiskl));
    sprintf(strsql,"select naik from Uospod where kod=%d",skl1);

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %d !\n",gettext("Нет найден код подразделения"),skl1);
      iceb_menu_soob(strsql,data->window);
      fprintf(ff,"%s %d !\n",gettext("Нет найден код подразделения"),skl1);
     }
    else
     strncpy(naiskl,row1[0],sizeof(naiskl)-1);

    iceb_u_zagolov(gettext("Реестр проводок по видах операций"),dn,mn,gn,dk,mk,gk,organ,ff);

    kst=-1;
    klr+=sli;
    sli=0;
    gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ff);

   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
      rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naikont,naiskl,&sli,&kst,&mso,&mspo,&mss,&msps,ff);
      itd=0.;
      kst=-1;
    klr+=sli;
      sli=0;
      fprintf(ff,"\f");
     }
    memset(shettz,'\0',sizeof(shettz));
    memset(nomdokv,'\0',sizeof(nomdokv));
    memset(kontr,'\0',sizeof(kontr));
    kom=1;


    /*Читаем наименование подразделения*/
    memset(naiskl,'\0',sizeof(naiskl));
    sprintf(strsql,"select naik from Uospod where kod=%d",skl1);

    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %d !\n",gettext("Нет найден код подразделения"),skl1);
      iceb_menu_soob(strsql,data->window);
      fprintf(ff,"%s %d !\n",gettext("Нет найден код подразделения"),skl1);
     }
    else
     strncpy(naiskl,row1[0],sizeof(naiskl)-1);

    sprintf(strsql,"%s %d %s\n",gettext("Подразделение"),skl1,naiskl);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    skl=skl1;
    gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ff);
   }

  if(iceb_u_SRAV(kontr,orgn,0) != 0)
   {
    if(kontr[0] != '\0')
     {
      rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naikont,naiskl,&sli,&kst,&mso,&mspo,ff);
      itd=0.;
     }
    memset(nomdokv,'\0',sizeof(nomdokv));
    memset(shettz,'\0',sizeof(shettz));
    kom=1;

    memset(bros,'\0',sizeof(bros));
    memset(kop,'\0',sizeof(kop));
    if(iceb_u_atof(orgn) == 0. && iceb_u_pole(orgn,bros,1,'-') == 0 && orgn[0] == '0')
     strcpy(kop,bros);
    else
     strcpy(kop,orgn);

    memset(naikont,'\0',sizeof(naikont)-1);

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kop);

    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %s !\n",gettext("Не нашли код контрагента"),kop);
      iceb_menu_soob(strsql,data->window);
     }
    else
     strncpy(naikont,row1[0],sizeof(naikont)-1);

    sprintf(strsql,"%s:%s/%s\n",gettext("Контрагент"),orgn,naikont);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    strcpy(kontr,orgn);
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

   gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ff);

   sprintf(strsql,"%02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %15.2f\n",
   dp,mp,gp,dd,md,gd,
   iceb_u_kolbait(11,nomdok),nomdok,
   iceb_u_kolbait(11,shett),shett,
   deb);

   iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

   if(kom == 1)
     fprintf(ff,"\
%-*s %-*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s\n",
     iceb_u_kolbait(7,orgn),orgn,
     iceb_u_kolbait(52,naikont),naikont,
     dd,md,gd,dp,mp,gp,
     iceb_u_kolbait(11,nomdok),nomdok,
     iceb_u_kolbait(11,shett),shett,
     iceb_u_prnbr(deb));

   if(kom == 0)
    fprintf(ff,"\
%60s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s %s\n",
    " ",dd,md,gd,dp,mp,gp,
    iceb_u_kolbait(11,nomdok),nomdok,
    iceb_u_kolbait(11,shett),shett,
    iceb_u_prnbr(deb),kome);

   itd+=deb;
   kom=0;

 }
rasis(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naikont,naiskl,&sli,&kst,&mso,&mspo,&mss,&msps,ff);
rasoi(&mssi,&mspsi,ff);

if(data->rk->kod_op.getdlinna() <= 1 || ( data->rk->kod_op.getdlinna() > 1 && iceb_u_pole(data->rk->kod_op.ravno(),shett1,2,',') == 0))
 {
  k1=0.;
  fprintf(ff,"\n==================================================\n\
%s:\n",
  gettext("Общий итог по всем операциям"));
  for(i=0; i< mssio.kolih() ;i++)
   {
    //POLE(mssio,shett1,i+1,' ');
    fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,mssio.ravno(i)),mssio.ravno(i),iceb_u_prnbr(mspsio.ravno(i)));
    k1+=mspsio.ravno(i);
   }
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));
 }

unlink(imaftmp);



iceb_podpis(ff,data->window);
fclose(ff);

sprintf(imafitog,"rpit%d.lst",getpid());

if((ffitog = fopen(imafitog,"w")) == NULL)
 {
  iceb_er_op_fil(imafitog,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Реестр проводок по видах операций"),dn,mn,gn,dk,mk,gk,organ,ffitog);

uosrpw_r_prp(data,ffitog); /*Печать реквизитов поиска*/



k1=0.;
fprintf(ffitog,"\n==================================================\n\
%s:\n",
gettext("Общий итог по всем операциям"));
for(i=0; i< mssio.kolih() ;i++)
 {
  fprintf(ffitog,"%-*s - %20s\n",iceb_u_kolbait(10,mssio.ravno(i)),mssio.ravno(i),iceb_u_prnbr(mspsio.ravno(i)));
  k1+=mspsio.ravno(i);
 }
fprintf(ffitog,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));

iceb_podpis(ffitog,data->window);
fclose(ffitog);


klr+=sli;

sprintf(strsql,"%s: %d\n",gettext("Количество листов"),klr);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);



data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Реестр проводок по видам операций"));
data->rk->imaf.plus(imafitog);
data->rk->naim.plus(gettext("Общий итог"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),1,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
