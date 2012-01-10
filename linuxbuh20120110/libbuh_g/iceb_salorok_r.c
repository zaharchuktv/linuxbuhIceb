/*$Id: iceb_salorok_r.c,v 1.15 2011-09-05 08:18:29 sasa Exp $*/
/*31.08.2011	02.03.2008	Белых А.И.	iceb_salorok_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "iceb_libbuh.h"
#include <unistd.h>
#include "iceb_salorok.h"
//#include "core_config.h"

class iceb_salorok_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class iceb_salorok_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  iceb_salorok_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   iceb_salorok_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_salorok_r_data *data);
gint iceb_salorok_r1(class iceb_salorok_r_data *data);
void  iceb_salorok_r_v_knopka(GtkWidget *widget,class iceb_salorok_r_data *data);

void iceb_spkw(short dk,short mk,short gk,const char *shet,const char *naimsh,const char *kontr,const char *naikontr,double suma,FILE *ff_spr,GtkWidget *wpredok);


extern SQL_baza bd;
extern char *name_system;
extern char *imabaz;
extern char *organ;

extern iceb_u_str shrift_ravnohir;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int iceb_salorok_r(class iceb_salorok_data *datark,GtkWidget *wpredok)
{
char strsql[1024];
iceb_u_str repl;
class iceb_salorok_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

//sprintf(strsql,"%s %s",_config.system_name(),iceb_u_toutf(gettext("Расчёт сальдо по контрагенту")));
sprintf(strsql,"%s %s",name_system,iceb_u_toutf(gettext("Расчёт сальдо по контрагенту")));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_salorok_r_key_press),&data);

if(wpredok != NULL)
 {
#if !defined(WIN32)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
#endif
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(iceb_u_toutf(gettext("Ждите !!!")));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Расчёт сальдо по контрагенту"));
//sprintf(strsql,"%s:%s %s\n",gettext("База данных"),_config.database_name().c_str(),_config.organization().c_str());
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



sprintf(strsql,"F10 %s",iceb_u_toutf(gettext("Выход")));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,iceb_u_toutf(gettext("Завершение работы в этом окне.")),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(iceb_salorok_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)iceb_salorok_r1,&data);

gtk_main();
#if !defined(WIN32)
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
#endif

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_salorok_r_v_knopka(GtkWidget *widget,class iceb_salorok_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_salorok_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_salorok_r_data *data)
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
/*********************/
/*Вывод общего итога в акт сверки*/
/*********************/
void obit_ac(double db,double kr,FILE *ff1)
{
double          brr=0.;

fprintf(ff1,"----------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%10.2f %10.2f\n",iceb_u_kolbait(31,gettext("Оборот общий")),gettext("Оборот общий"),db,kr);
if(db >= kr)
 {
  brr=db-kr;
  fprintf(ff1,"%*s:%10s\n",iceb_u_kolbait(31,gettext("Сальдо общее")),gettext("Сальдо общее"),iceb_u_prnbr(brr));
 }
else
 {
  brr=kr-db;
  fprintf(ff1,"%*s:%10s %10s\n",iceb_u_kolbait(31,gettext("Сальдо общее")),gettext("Сальдо общее")," ",iceb_u_prnbr(brr));
 }

}

/**************/
/*Вывод сальдо*/
/**************/
void salorkslw(double dd, double kk,double ddn,double kkn,
double debm,double krem,
FILE *ff1,FILE *ffact)
{

double brr=dd+ddn;
double krr=kk+kkn;
fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");

if(debm != 0. || krem != 0.)
 {
  fprintf(ff1,"\
%*s:%12s",
  iceb_u_kolbait(37,gettext("Оборот за месяц")),gettext("Оборот за месяц"),iceb_u_prnbr(debm));

  fprintf(ff1," %12s\n",iceb_u_prnbr(krem));
  if(ffact != NULL)
   fprintf(ffact,"%*s %10.2f %10.2f| |\n",
   iceb_u_kolbait(31,gettext("Оборот за месяц")),gettext("Оборот за месяц"),debm,krem);
 }

fprintf(ff1,"%*s:%12s",
iceb_u_kolbait(37,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(dd));

fprintf(ff1," %12s\n",iceb_u_prnbr(kk));

if(ffact != NULL)
 fprintf(ffact,"%*s %10.2f %10.2f| |\n",
 iceb_u_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),dd,kk);

fprintf(ff1,"%*s:%12s",
iceb_u_kolbait(37,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),iceb_u_prnbr(brr));
fprintf(ff1," %12s\n",iceb_u_prnbr(brr));

if(ffact != NULL)
 fprintf(ffact,"%*s %10.2f %10.2f| |\n",
 iceb_u_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),brr,krr);

if(dd+ ddn >= kk+kkn)
 {
  brr=(dd+ddn)-(kk+kkn);
  fprintf(ff1,"%*s:%12s\n",
  iceb_u_kolbait(37,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),iceb_u_prnbr(brr));

  if(ffact != NULL)
   fprintf(ffact,"%*s %10.2f %10s| |\n",
   iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),brr," ");
 }
else
 {
  brr=(kk+kkn)-(dd+ddn);
  fprintf(ff1,"%*s:%12s %12s\n",
  iceb_u_kolbait(37,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",iceb_u_prnbr(brr));


  if(ffact != NULL)
   fprintf(ffact,"%*s %10s %10.2f| |\n",
   iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",brr);

 }
}
/*********************************/
/*Концовка акта сверки           */
/*********************************/

void	konact(FILE *ff)
{
fprintf(ff,"\n");
fprintf(ff,gettext("\
После отражения в учете указанных дополнительных проводок сальдо на ________________________\n\
выражается в сумме__________________________________________________________________________\n\
____________________________________________________________________________________________\n\
                  (сумма прописью и цифрами)\n\
____________________________________________________________________________________________\n\
                   (Указать наименование учереждения)\n\n\
    Главный бухгалтер________________________        Главный бухгалтер________________________\n"));
                                     
fprintf(ff,"\n");

SQL_str row;
SQLCURSOR cur;
char telef[100];
memset(telef,'\0',sizeof(telef));
if(iceb_sql_readkey("select telef from Kontragent where kodkon='00'",&row,&cur,NULL) == 1)
 strncpy(telef,row[0],sizeof(telef)-1);

fprintf(ff,gettext("\
Вышлите нам, пожалуйста, один экземпляр акта с Вашими подписью и печатью.\n\
В случае несоответствия сумм, просим выслать расшифровку или провести сверку\n\
по телефону"));

fprintf(ff," %s\n",telef); 

fprintf(ff,gettext("\
Просим в течении 5 дней вернуть подписанный акт.\n\
При невозвращении акта, сумма будет зачтена, как согласованная."));

iceb_podpis(ff,NULL);

}


/*************************************/
/*Получение сальдо по заданному счету*/
/*************************************/
void saloro1(const char sh[],short dn,short mn,short gn,
short dk,short mk,short gk,const char kor[],const char nai[],FILE *ff1,
FILE *ff4,FILE *ffact,
double *db,double *kr,
short mks, //0-все проводки 1-без взаимно кореспондирующих
class iceb_u_spisok *sheta_srs,class iceb_u_double *mdo,
const char *kodgr,
const char *naimgr,
const char *imatmptab,
GtkWidget *view,
GtkTextBuffer *buffer,
FILE *ff_act,
FILE *ff_act_bi,
FILE *ff_spr,
GtkWidget *wpredok)
{
time_t          tmm;
struct  tm      *bf;
double          ddn,kkn;
short           mvs;
short           d,m,g;
char		shh[32];
double		dd,kk;
int		nm;
long		kolstr;
SQL_str         row;
SQLCURSOR       cur;
char		strsql[1024];
char		naish[512];
short		godn;
short		mes=0,god=0;
short		shet=0;
double		deb=0.,kre=0.;
double		debm=0.,krem=0.;
int		val=0;
class iceb_u_str koment;
short	startgodb=0; /*Стартовый год*/


if(iceb_poldan("Стартовый год",strsql,"nastrb.alx",wpredok) == 0)
 startgodb=atoi(strsql);

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

strcpy(shh,sh);

memset(naish,'\0',sizeof(naish));

sprintf(strsql,"select nais,stat from Plansh where ns='%s'",sh);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  strncpy(naish,row[0],sizeof(naish)-1);
  val=atoi(row[1]);
 }
if(val == 1)
 val=-1;
 
ddn=kkn=0.;
dd=kk=0.;
sprintf(strsql,"select deb,kre from Saldo where kkk='1' and gs=%d and \
ns='%s' and kodkon='%s'",godn,shh,kor);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
//if((kolstr=static_cast<int>(cur.make_cursor(&bd,strsql))) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  ddn+=atof(row[0]);
  kkn+=atof(row[1]);
 }
if(ddn > kkn)
 {
  ddn=ddn-kkn; 
  kkn=0.;
 }
else
 {
  kkn=kkn-ddn;
  ddn=0;
 }
time(&tmm);
bf=localtime(&tmm);

fprintf(ff1,"%s\n\
%s: %s %s\n\
%s %s %s %s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
organ,
gettext("Счет"),sh,naish,
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

if(kodgr[0] != '\0')
 fprintf(ff1,"%s:%s %s\n",gettext("Группа"),kodgr,naimgr);
 
fprintf(ff1,"\
--------------------------------------------------------------------------------------------\n");

fprintf(ff1,gettext("\
  Дата    |Сче.к.|Записал|Номер док. |    Дебет   |   Кредит   |    К о м е н т а р и й    |\n"));
/********
                                                                123456789012345678901234567
*********/

fprintf(ff1,"\
--------------------------------------------------------------------------------------------\n");

fprintf(ff4,"%s\n\
%s: %s %s\n\
%s %s %s %s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
organ,
gettext("Счет"),sh,naish,
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
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

fprintf(ff4,"\
--------------------------------------------------------------------------------------------\n");

fprintf(ff4,gettext("\
  Дата    |Сче.к.|Записал|Номер док. |    Дебет   |   Кредит   |    К о м е н т а р и й    |\n"));

fprintf(ff4,"\
--------------------------------------------------------------------------------------------\n");

iceb_zagsh(sh,ffact,wpredok);

class akt_sverki akt_svr;
strncpy(akt_svr.shet,sh,sizeof(akt_svr.shet)-1);

mvs=0;
g=godn;
d=1;
m=1;
int nomer=0;
int kolbsn37=iceb_u_kolbait(37,gettext("Сальдо начальное"));
int kolbsn20=iceb_u_kolbait(20,gettext("Сальдо начальное"));
while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {
  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0 && mvs == 0)
   {
    nm=sheta_srs->find(sh);
    akt_svr.dn=d;
    akt_svr.mn=m;
    akt_svr.gn=g;

    if(ddn > kkn)    
     {
      akt_svr.start_saldo_deb=ddn-kkn;
      ddn=ddn-kkn;
      mdo->plus(ddn,nm*2);
      kkn=0.;

      sprintf(strsql,"%20s\n",iceb_u_prnbr(ddn));
      iceb_printw(iceb_u_toutf(strsql),buffer,view);

      fprintf(ff1,"%*s:%12s\n",kolbsn37,gettext("Сальдо начальное"),iceb_u_prnbr(ddn));
      
      fprintf(ff4,"%*s:%12s\n",kolbsn37,gettext("Сальдо начальное"),iceb_u_prnbr(ddn));

      fprintf(ffact,"%02d.%02d.%4d %-*.*s %10.2f %10s| |\n",
      d,m,g,kolbsn20,kolbsn20,gettext("Сальдо начальное"),ddn," ");
      fprintf(ffact,"\
------------------------------------------------------ ---------------------------------------------------\n");

     }
    else
     {
      akt_svr.start_saldo_kre=kkn-ddn;
      kkn=kkn-ddn;
      mdo->plus(kkn,nm*2+1);
      ddn=0.;

      sprintf(strsql,"%20s %20s\n"," ",iceb_u_prnbr(kkn));
      iceb_printw(iceb_u_toutf(strsql),buffer,view);

      fprintf(ff1,"%*s:%12s %12s\n",kolbsn37,
      gettext("Сальдо начальное")," ",iceb_u_prnbr(kkn));

      fprintf(ff4,"%*s:%12s %12s\n",kolbsn37,
      gettext("Сальдо начальное")," ",iceb_u_prnbr(kkn));

      fprintf(ffact,"%02d.%02d.%4d %-*.*s %10s %10.2f| |\n",
      d,m,g,kolbsn20,kolbsn20,gettext("Сальдо начальное")," ",kkn);
       fprintf(ffact,"\
------------------------------------------------------ ---------------------------------------------------\n");
     }
      mvs=1;
   }
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"select * from Prov \
where val=%d and datp='%04d-%02d-%02d' and sh='%s' and kodkon='%s'",
  val,g,m,d,shh,kor);
//  if((kolstr=static_cast<int>(cur.make_cursor(&bd,strsql))) < 0)
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    iceb_u_dpm(&d,&m,&g,1);
    continue;
   }
  
  if(kolstr == 0)
   {
    iceb_u_dpm(&d,&m,&g,1);
    continue;
   }
  shet=0;
  
  while(cur.read_cursor(&row) != 0)
   {
    if(mks == 1)
     if(iceb_u_SRAV(sh,row[3],1) == 0 || sheta_srs->find(row[3]) == 0)
       continue;
    deb=atof(row[9]);
    kre=atof(row[10]);


    if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0)
    if(iceb_u_sravmydat(dk,mk,gk,d,m,g) >= 0)
     {
      if(shet == 0)
       {
        if(iceb_u_sravmydat(1,m,g,1,mes,god) != 0)
         {
          if(mes != 0)
           {
            salorkslw(dd,kk,ddn,kkn,debm,krem,ff1,ffact);
            debm=krem=0.;
           }
          mes=m;
          god=g;
         }
        shet=1;
       }
      dd+=deb;
      kk+=kre;
      debm+=deb;
      krem+=kre;

      int kbsh=iceb_u_kolbait(6,row[3]);
      int kbkto=iceb_u_kolbait(3,row[5]);
      int kbnomd=iceb_u_kolbait(11,row[6]);
      int kbkom=iceb_u_kolbait(27,row[13]);
      
      fprintf(ff1,"%02d.%02d.%4d %-*s %-*s%4s %-*s %12s",
      d,m,g,kbsh,row[3],kbkto,row[5],row[11],kbnomd,row[6],iceb_u_prnbr(deb));

      fprintf(ff1," %12s %-*.*s\n",iceb_u_prnbr(kre),kbkom,kbkom,row[13]);
      for(int nom=27; nom < iceb_u_strlen(row[13]); nom+=27)
        fprintf(ff1,"%63s %.*s\n","",iceb_u_kolbait(27,iceb_u_adrsimv(nom,row[13])),iceb_u_adrsimv(nom,row[13]));

      fprintf(ff4,"%02d.%02d.%4d %-*s %-*s%4s %-*s %12s",
      d,m,g,kbsh,row[3],kbkto,row[5],row[11],kbnomd,row[6],iceb_u_prnbr(deb));

      fprintf(ff4," %12s %-*.*s\n",iceb_u_prnbr(kre),kbkom,kbkom,row[13]);
      for(int nom=27; nom < iceb_u_strlen(row[13]); nom+=27)
        fprintf(ff4,"%63s %.*s\n","",iceb_u_kolbait(27,iceb_u_adrsimv(nom,row[13])),iceb_u_adrsimv(nom,row[13]));



      koment.new_plus(row[13]);

      sprintf(strsql,"insert into %s values ('%s','%s','%04d-%02d-%02d','%s','%s',%s,%s,'%s')",
      imatmptab,
      row[2],row[6],g,m,d,row[3],row[5],row[9],row[10],koment.ravno_filtr());
             
      iceb_sql_zapis(strsql,1,1,wpredok);    

      koment.new_plus(row[6]);
      koment.plus(" ",row[13]);

      kbkom=iceb_u_kolbait(20,koment.ravno());      
      fprintf(ffact,"%02d.%02d.%4d %-*.*s %10.2f %10.2f| |\n",
      d,m,g,kbkom,kbkom,koment.ravno(),deb,kre);

      //заряжаем массивы
      sprintf(strsql,"%d.%d.%d|%s",d,m,g,row[6]);
      if(row[6][0] == '\0')
         nomer=-1;
      else
         nomer=akt_svr.data_nomd.find(strsql);

      if(nomer < 0)
       {
        akt_svr.data_nomd.plus(strsql);
        if(iceb_u_strstrm(koment.ravno(),"-ПДВ") == 1) //есть образец в строке
         {
          iceb_u_cpbstr(&akt_svr.koment,koment.ravno(),"-ПДВ");
         }
        else
          akt_svr.koment.plus(koment.ravno());
       }
      akt_svr.suma_deb.plus(deb,nomer);
      akt_svr.suma_kre.plus(kre,nomer);

      fprintf(ffact,"\
------------------------------------------------------ ---------------------------------------------------\n");

     }


    if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0)
     {
      ddn+=deb;
      kkn+=kre;
     }

   }
  iceb_u_dpm(&d,&m,&g,1);
 }

sprintf(strsql,"%20s",iceb_u_prnbr(dd));
iceb_printw(iceb_u_toutf(strsql),buffer,view);

sprintf(strsql," %20s\n",iceb_u_prnbr(kk));
iceb_printw(iceb_u_toutf(strsql),buffer,view);

salorkslw(dd,kk,ddn,kkn,debm,krem,ff1,ffact);
salorkslw(dd,kk,ddn,kkn,0.,0.,ff4,NULL);

iceb_spkw(dk,mk,gk,sh,naish,kor,nai,kk+kkn-dd-ddn,ff_spr,wpredok);
fprintf(ff_spr,"\n\n");

*db+=dd+ddn;
*kr+=kk+kkn;

iceb_akt_sv(&akt_svr,ff_act,ff_act_bi,wpredok);

}
/*********************/
/*Выдача общего итога*/
/*********************/
void  obit(double db,double kr,
short mt, //0-пром. итог 1- общий
int metkascr, //0-выводить на экран 1-нет
FILE *ff1,
GtkWidget *view,
GtkTextBuffer *buffer)
{
double          brr;
char strsql[1024];

if(mt == 1 && metkascr == 0)
 {  
  sprintf(strsql,"--------------------------------------------\n");
  iceb_printw(iceb_u_toutf(strsql),buffer,view);
 }

if(mt == 1)
 {
  fprintf(ff1,"\
---------------------------------------------------------------------------------------\n\
%*s:%12s",iceb_u_kolbait(37,gettext("Общий итог")),gettext("Общий итог"),iceb_u_prnbr(db));
  fprintf(ff1," %12s\n",iceb_u_prnbr(kr));
 }

if(db >= kr)
 {
  brr=db-kr;
  if(metkascr == 0)
   {
    sprintf(strsql,"%20s\n",iceb_u_prnbr(brr));
    iceb_printw(iceb_u_toutf(strsql),buffer,view);
   }
  if(mt == 1)
      fprintf(ff1,"%38s%12s\n"," ",iceb_u_prnbr(brr));
 }
else
 {
  brr=kr-db;
  if(metkascr == 0)
   {
    sprintf(strsql,"%20s %20s\n"," ",iceb_u_prnbr(brr));
    iceb_printw(iceb_u_toutf(strsql),buffer,view);
   }
  if(mt == 1)
      fprintf(ff1,"%38s%12s %12s\n"," "," ",iceb_u_prnbr(brr));
 }

}
/*********************/
/*Выдача итога в файл*/
/*********************/
void		vsali(double debs,double kres,short nm,short met,FILE *ff1,class iceb_u_double *mdo)
{
double		brr;

if(met == 1)
 {
  fprintf(ff1,"\n\
---------------------------------------------------------------------------------------\n\
%*s:%12s",iceb_u_kolbait(32,gettext("Общий итог")),gettext("Общий итог"),iceb_u_prnbr(debs));

  fprintf(ff1," %12s\n",iceb_u_prnbr(kres));

  if(debs >= kres)
   {
    brr=debs -kres;
    fprintf(ff1,"%33s%12s\n"," ",iceb_u_prnbr(brr));
   }
  else
   {
    brr=kres-debs;
    fprintf(ff1,"%33s%12s %12s\n"," "," ",iceb_u_prnbr(brr));
   }

  return;
 }

fprintf(ff1,"\
---------------------------------------------------------------------------------------\n\
%*s:%12s",iceb_u_kolbait(32,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(debs));

fprintf(ff1," %12s\n",iceb_u_prnbr(kres));

brr=debs+mdo->ravno(nm*2);
fprintf(ff1,"%*s:%12s",iceb_u_kolbait(32,gettext("Сальдо развернутое")),gettext("Сальдо развернутое"),iceb_u_prnbr(brr));

brr=kres+mdo->ravno(nm*2+1);
fprintf(ff1," %12s\n",iceb_u_prnbr(brr));
 
if(mdo->ravno(nm*2)+ debs >= mdo->ravno(nm*2+1)+kres)
 {
  brr=mdo->ravno(nm*2)+ debs - (mdo->ravno(nm*2+1)+kres);
  fprintf(ff1,"%*s:%12s\n",iceb_u_kolbait(32,gettext("Сальдо конечное")),gettext("Сальдо конечное"),iceb_u_prnbr(brr));
 }
else
 {
  brr=mdo->ravno(nm*2+1)+kres-(mdo->ravno(nm*2)+ debs) ;
  fprintf(ff1,"%*s:%12s %12s\n",iceb_u_kolbait(32,gettext("Сальдо конечное")),gettext("Сальдо конечное")," ",iceb_u_prnbr(brr));
 }

}

/*********************************/
/*Работа с отсортированным файлом*/
/*********************************/
void potf(const char kor[],const char nai[],short dn,short mn,short gn,
short dk,short mk,short gk,const char *imaf1,const char *imaf2,class iceb_u_spisok *sheta_srs,class iceb_u_double *mdo,
const char *imatmptab,
GtkWidget *wpredok)
{
time_t          tmm;
struct  tm      *bf;
FILE		*ff1;
short		d,m,g;
char		sh[32],shz[32];
char		shk[32];
char		nn[32],nnz[32];
double		deb,deb1=0.,debo,debs;
double		kre,kre1=0.,kreo,kres;
char		kom[512],komz[512];
char		kto[32];
short		kop;
int		nm;
char		naish[512];
char		strsql[512];
SQL_str         row;
char		bros[512];

time(&tmm);
bf=localtime(&tmm);


class SQLCURSOR curtmp;
SQL_str rowtmp;

sprintf(strsql,"select * from %s order by sh asc,datp asc,nomd asc,datp asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

fprintf(ff1,"%s\n",gettext("Проводки по номерам документов просуммированы"));

fprintf(ff1,"%s\n\
%s %s %s %s\n\
     %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
organ,
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
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

shz[0]='\0';
memset(nnz,'\0',sizeof(nnz));
kre=deb=debo=kreo=debs=kres=0.;
nm=kop=0;
SQLCURSOR curr;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  strncpy(sh,rowtmp[0],sizeof(sh)-1);
  iceb_u_rsdat(&d,&m,&g,rowtmp[2],2);
  strncpy(kom,rowtmp[7],sizeof(kom)-1);
  strncpy(nn,rowtmp[1],sizeof(nn)-1);
  strncpy(shk,rowtmp[3],sizeof(shk)-1);
  strncpy(kto,rowtmp[4],sizeof(kto)-1);
  deb1=atof(rowtmp[5]);
  kre1=atof(rowtmp[6]);
 
  if(iceb_u_SRAV(nn,nnz,0) != 0)
   {
    sprintf(bros,"%s %s:",gettext("Итог по документу"),nnz);
    fprintf(ff1,"%*s %12s",iceb_u_kolbait(32,bros),bros,iceb_u_prnbr(deb));
    fprintf(ff1," %12s\n\n",iceb_u_prnbr(kre));
    kop=0;
    strcpy(nnz,nn);
    deb=kre=0.;
   }

  if(iceb_u_SRAV(sh,shz,0) != 0)
   {
    if(shz[0] != '\0')
     {
      if(deb != 0. || kre != 0.)
       {
        sprintf(bros,"%s %s:",gettext("Итог по документу"),nnz);
        fprintf(ff1,"%*s %12s",iceb_u_kolbait(32,bros),bros,iceb_u_prnbr(deb));
        fprintf(ff1," %12s\n\n",iceb_u_prnbr(kre));
        deb=kre=0.;
        
       }
      vsali(debs,kres,nm,0,ff1,mdo);
      debs=kres=0.;
     }

    nm=sheta_srs->find(sh);
    debo+=mdo->ravno(nm*2);
    kreo+=mdo->ravno(nm*2+1);

    memset(naish,'\0',sizeof(naish));

    sprintf(strsql,"select nais from Plansh where ns='%s'",sh);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      strncpy(naish,row[0],sizeof(naish)-1);
    
    fprintf(ff1,"\n%s: %s %s\n\
---------------------------------------------------------------------------------------\n",
    gettext("Счет"),sh,naish);
    if(mdo->ravno(nm*2) > mdo->ravno(nm*2+1) )
      {
       fprintf(ff1,"%*s:%12s\n",iceb_u_kolbait(32,gettext("Сальдо начальное")),
       gettext("Сальдо начальное"),iceb_u_prnbr(mdo->ravno(nm*2)));
      }
     else
      {
       fprintf(ff1,"%*s:%12s %12s\n",iceb_u_kolbait(32,gettext("Сальдо начальное")),
       gettext("Сальдо начальное")," ",iceb_u_prnbr(mdo->ravno(nm*2+1)));
      }

    strcpy(shz,sh);
   }

  fprintf(ff1,"%02d.%02d.%4d %-*s %-*s %-*s %12s",
  d,m,g,
  iceb_u_kolbait(8,shk),shk,
  iceb_u_kolbait(7,nn),nn,
  iceb_u_kolbait(4,kto),kto,
  iceb_u_prnbr(deb1));

  fprintf(ff1," %12s %.*s\n",iceb_u_prnbr(kre1),iceb_u_kolbait(27,kom),kom);

  for(int nom=27; nom < iceb_u_strlen(kom); nom+=27)
        fprintf(ff1,"%58s %.*s\n","",iceb_u_kolbait(27,iceb_u_adrsimv(nom,kom)),iceb_u_adrsimv(nom,kom));

  if(nn[0] != '\0' )
   {
    kop++;
   }
  deb+=deb1;
  kre+=kre1;
  debs+=deb1;
  kres+=kre1;
  debo+=deb1;
  kreo+=kre1;
 }
sprintf(bros,"%s %s:",gettext("Итог по документу"),nnz);
fprintf(ff1,"%*s %12s",iceb_u_kolbait(32,bros),bros,iceb_u_prnbr(deb));
fprintf(ff1," %12s\n\n",iceb_u_prnbr(kre));

vsali(debs,kres,nm,0,ff1,mdo);
vsali(debo,kreo,nm,1,ff1,mdo);
iceb_podpis(ff1,wpredok);

fclose(ff1);


sprintf(strsql,"select * from %s order by sh asc,datp asc,koment asc,nomd asc,shk asc",imatmptab);

if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }




if((ff1 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,wpredok);
  return;
 }

fprintf(ff1,"%s\n",gettext("Проводки по комментариям просуммированы"));

fprintf(ff1,"%s\n\
%s %s %s %s\n\
     %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
organ,
gettext("Распечатка проводок"),
gettext("для контрагента"),
kor,nai,
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


shz[0]='\0';
komz[0]='\0';
kre=deb=debo=kreo=debs=kres=0.;
kop=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  strncpy(sh,rowtmp[0],sizeof(sh)-1);
  iceb_u_rsdat(&d,&m,&g,rowtmp[2],2);
  strncpy(kom,rowtmp[7],sizeof(kom)-1);
  strncpy(nn,rowtmp[1],sizeof(nn)-1);
  strncpy(shk,rowtmp[3],sizeof(shk)-1);
  strncpy(kto,rowtmp[4],sizeof(kto)-1);
  deb1=atof(rowtmp[5]);
  kre1=atof(rowtmp[6]);
       
  if(iceb_u_SRAV(kom,komz,0) != 0)
   {
    sprintf(bros,"%s %.10s:",gettext("Итог по комментарию"),komz);
    fprintf(ff1,"%*s %12s",iceb_u_kolbait(32,bros),bros,iceb_u_prnbr(deb));
    fprintf(ff1," %12s\n\n",iceb_u_prnbr(kre));
    kop=0;
    strcpy(komz,kom);
    deb=kre=0.;
   }

  if(iceb_u_SRAV(sh,shz,0) != 0)
   {
    if(shz[0] != '\0')
     {
      vsali(debs,kres,nm,0,ff1,mdo);
      debs=kres=0.;
     }

    nm=sheta_srs->find(sh);
    debo+=mdo->ravno(nm*2);
    kreo+=mdo->ravno(nm*2+1);
    
    fprintf(ff1,"\n%s %s\n\
---------------------------------------------------------------------------------------\n",
    gettext("Счёт"),sh);
    if(mdo->ravno(nm*2) > mdo->ravno(nm*2+1))    
       fprintf(ff1,"%*s:%12s\n",iceb_u_kolbait(32,gettext("Сальдо начальное")),gettext("Сальдо начальное"),iceb_u_prnbr(mdo->ravno(nm*2)));
     else
       fprintf(ff1,"%*s:%12s %12s\n",iceb_u_kolbait(32,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",iceb_u_prnbr(mdo->ravno(nm*2+1)));
    strcpy(shz,sh);
   }

  fprintf(ff1,"%02d.%02d.%4d %-*s %-*s %-*s %12s",
  d,m,g,
  iceb_u_kolbait(8,shk),shk,
  iceb_u_kolbait(7,nn),nn,
  iceb_u_kolbait(4,kto),kto,
  iceb_u_prnbr(deb1));

  fprintf(ff1," %12s %.*s\n",iceb_u_prnbr(kre1),iceb_u_kolbait(27,kom),kom);

  for(int nom=27; nom < iceb_u_strlen(kom); nom+=27)
        fprintf(ff1,"%58s %.*s\n","",iceb_u_kolbait(27,iceb_u_adrsimv(nom,kom)),iceb_u_adrsimv(nom,kom));

  if(kom[0] != '\0' )
    kop++;
  deb+=deb1;
  kre+=kre1;
  debo+=deb1;
  kreo+=kre1;
  debs+=deb1;
  kres+=kre1;
  
 }
sprintf(bros,"%s %.10s:",gettext("Итог по комментарию"),komz);
fprintf(ff1,"%*s %12s",iceb_u_kolbait(32,bros),bros,iceb_u_prnbr(deb));
fprintf(ff1," %12s\n\n",iceb_u_prnbr(kre));
vsali(debs,kres,nm,0,ff1,mdo);
vsali(debo,kreo,nm,1,ff1,mdo);

iceb_podpis(ff1,wpredok);
fclose(ff1);

}



/******************************************/
/******************************************/

gint iceb_salorok_r1(class iceb_salorok_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
class SQLCURSOR cur,cur1;
short		godn;
short	startgodb=0; /*Стартовый год*/




short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(iceb_poldan("Стартовый год",strsql,"nastrb.alx",data->window) == 0)
 startgodb=atoi(strsql);

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


char kodgr[32];
char naimgr[512];
memset(kodgr,'\0',sizeof(kodgr));
memset(naimgr,'\0',sizeof(naimgr));
char nai[512];
memset(nai,'\0',sizeof(nai));
sprintf(strsql,"select naikon,grup from Kontragent where kodkon='%s'",data->rk->kontr.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  strncpy(nai,row[0],sizeof(nai)-1);
  strncpy(kodgr,row[1],sizeof(kodgr)-1);
 }
if(kodgr[0] != '\0')
 {
  sprintf(strsql,"select naik from Gkont where kod=%s",kodgr);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   strncpy(naimgr,row[0],sizeof(naimgr)-1);
 }
 

/*Просматриваем план счетов, находим все счета с развернутым
сальдо и заряжаем в массив*/
time_t tmm;
time(&tmm);
struct  tm      *bf;
bf=localtime(&tmm);
int kolstr=0;


class iceb_u_spisok sheta_srs; /*Список счетов с развёрнутым сальдо*/
/*Смотрим по каким счетам введено сальдо*/
sprintf(strsql,"select ns from Saldo where kkk=1 and gs=%d and kodkon='%s'",
godn,data->rk->kontr.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
 {
  sheta_srs.plus(row[0]);
 }

/*Смотрим по каким счетам были сделаны проводки*/
sprintf(strsql,"select distinct sh from Prov where datp >= '%04d-1-1' and \
datp <= '%04d-%02d-%02d' and kodkon='%s'",godn,gk,mk,dk,data->rk->kontr.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select saldo from Plansh where ns='%s'",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"%s %s",gettext("Не найден счёт в плане счетов!"),row[0]);
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  if(atoi(row1[0]) != 3)
   continue;  
  if(sheta_srs.find(row[0]) < 0)
    sheta_srs.plus(row[0]);
 }



char sh[32];
int ks=sheta_srs.kolih();
if(ks == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

sprintf(strsql,"%s\n",gettext("Расчет сальдо по всем счетам с развернутым сальдо"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


sprintf(strsql,"%s:%d\n",gettext("Количество счетов с развернутым сальдо"),ks);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

for(int ii=0; ii < ks ; ii++)
 {
  sprintf(strsql," %s",sheta_srs.ravno(ii));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
 }
iceb_printw("\n",data->buffer,data->view);


class iceb_u_double mdo;
mdo.make_class(ks*2);

char imaf_spr[56];
sprintf(imaf_spr,"sprk%d.lst",getpid());
FILE *ff_spr;
if((ff_spr = fopen(imaf_spr,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_spr,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf[56];
FILE *ff1;
sprintf(imaf,"sl%d.lst",getpid());
if((ff1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


class iceb_tmptab tabtmp;
const char *imatmptab={"salorok"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(24) not null,\
nomd char(24) not null,\
datp DATE NOT NULL,\
shk char(24) not null,\
kto char(8) not null,\
deb double(14,2) not null,\
kre double(14,2) not null,\
koment char(112) not null)",imatmptab);



if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  



char imaf1[56];
sprintf(imaf1,"sla%d.lst",getpid());

char imaf2[56];

sprintf(imaf2,"slb%d.lst",getpid());

char imaf4[56];
FILE *ff4;

sprintf(imaf4,"slbc%d.lst",getpid());
if((ff4 = fopen(imaf4,"w")) == NULL)
 {
  iceb_er_op_fil(imaf4,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafacts[56];
FILE *ffact;

sprintf(imafacts,"acts%d.lst",getpid());
if((ffact = fopen(imafacts,"w")) == NULL)
 {
  iceb_er_op_fil(imafacts,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_acts[56];

sprintf(imaf_acts,"asv%d.lst",getpid());
FILE *ff_act;
if((ff_act = fopen(imaf_acts,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_acts,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_acts_bi[56];

sprintf(imaf_acts_bi,"asvbi%d.lst",getpid());
FILE *ff_act_bi;
if((ff_act_bi = fopen(imaf_acts_bi,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_acts_bi,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zagacts(dn,mn,gn,dk,mk,gk,data->rk->kontr.ravno(),nai,ffact);
iceb_zagacts(dn,mn,gn,dk,mk,gk,data->rk->kontr.ravno(),nai,ff_act);
iceb_zagacts(dn,mn,gn,dk,mk,gk,data->rk->kontr.ravno(),nai,ff_act_bi);
int kolshet=0;
float kolstr1=0.;
double db=0.,kr=0.;
for(int i=0; i < ks; i++)
 {
  iceb_pbar(data->bar,ks,++kolstr1);    
  strncpy(sh,sheta_srs.ravno(i),sizeof(sh)-1);
  
  if(iceb_u_proverka(data->rk->shet.ravno(),sh,0,0) != 0)
    continue;

  /*Проверяем есть ли код организации в списке данного счета*/

  sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",sh,data->rk->kontr.ravno());
  if(iceb_sql_readkey(strsql,data->window) != 1)
    continue;

  sprintf(strsql,"%s\n",sh);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  kolshet++;  
  double db1=0.,kr1=0.;
  saloro1(sh,dn,mn,gn,dk,mk,gk,data->rk->kontr.ravno(),nai,ff1,ff4,ffact,&db1,&kr1,data->rk->metka_prov,&sheta_srs,&mdo,kodgr,naimgr,imatmptab,data->view,data->buffer,ff_act,ff_act_bi,ff_spr,data->window);
  obit(db1,kr1,0,0,ff1,data->view,data->buffer);
  obit(db1,kr1,0,1,ff4,data->view,data->buffer);
  db+=db1; kr+=kr1;
 }
obit(db,kr,1,0,ff1,data->view,data->buffer);
obit(db,kr,1,1,ff4,data->view,data->buffer);
iceb_podpis(ff1,data->window);
iceb_podpis(ff4,data->window);
fclose(ff1);
fclose(ff4);
//Концовка распечатки
if(kolshet > 1)
 {
  obit_ac(db,kr,ffact);
  obit_ac(db,kr,ff_act);
  obit_ac(db,kr,ff_act_bi);
 }
konact(ffact);
fclose(ffact);
konact(ff_act);
fclose(ff_act);
konact(ff_act_bi);
fclose(ff_act_bi);
fclose(ff_spr);


potf(data->rk->kontr.ravno(),nai,dn,mn,gn,dk,mk,gk,imaf1,imaf2,&sheta_srs,&mdo,imatmptab,data->window);





data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Распечатка проводок з дневными итогами"));

data->rk->imaf.plus(imaf1);
data->rk->naim.plus(gettext("Проводки по контрагенту с итогами по документам"));

data->rk->imaf.plus(imaf2);
data->rk->naim.plus(gettext("Проводки по контрагенту с итогами по комментариям"));

data->rk->imaf.plus(imaf4);
data->rk->naim.plus(gettext("Проводки по контрагенту с конечным сальдо"));

data->rk->imaf.plus(imafacts);
data->rk->naim.plus(gettext("Акт сверки по контрагенту"));

data->rk->imaf.plus(imaf_acts);
data->rk->naim.plus(gettext("Акт сверки с общими суммами по документам"));

data->rk->imaf.plus(imaf_acts_bi);
data->rk->naim.plus(gettext("Акт сверки с общими суммами по документам без месячных итогов"));

data->rk->imaf.plus(imaf_spr);
data->rk->naim.plus(gettext("Справка про сальдо по контаргенту"));

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),1,data->window);



gtk_label_set_text(GTK_LABEL(data->label),iceb_u_toutf(gettext("Расчет закончен.")));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
