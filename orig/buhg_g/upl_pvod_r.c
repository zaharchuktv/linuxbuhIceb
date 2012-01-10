/*$Id: upl_pvod_r.c,v 1.10 2011-02-21 07:35:58 sasa Exp $*/
/*17.11.2009	30.03.2008	Белых А.И.	upl_pvod_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "upl_pvod.h"

class upl_pvod_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class upl_pvod_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  upl_pvod_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_pvod_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_pvod_r_data *data);
gint upl_pvod_r1(class upl_pvod_r_data *data);
void  upl_pvod_r_v_knopka(GtkWidget *widget,class upl_pvod_r_data *data);


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

int upl_pvod_r(class upl_pvod_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class upl_pvod_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка реестра путевых листов по водителям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(upl_pvod_r_key_press),&data);

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

repl.plus(gettext("Распечатка реестра путевых листов по водителям"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(upl_pvod_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)upl_pvod_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_pvod_r_v_knopka(GtkWidget *widget,class upl_pvod_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   upl_pvod_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_pvod_r_data *data)
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

/**************************/
/*Надпись над шапкой*/
/**************************/
void uplrpl_nad(short dn,short mn,short gn,
short dk,short mk,short gk,
char *kodvod,
char *naimvod,
char *kodavt,
char *naimavt,
char *rnavt,
FILE *ff)
{

iceb_u_zagolov(gettext("Реестр путевых листов"),dn,mn,gn,dk,mk,gk,organ,ff);

fprintf(ff,"\n\
%s %s %s\n\
%s %s %s %s %s\n",
gettext("Водитель"),
kodvod,naimvod,
gettext("Автомобиль"),
kodavt,
naimavt,
gettext("регистрационный номер"),
rnavt);
}
/*******************/
/*Шапка*/
/*****************/
void uplrpl_sapi(FILE *ff)
{
fprintf(ff,"\
-----------------------------------------------------\n");
fprintf(ff,gettext("\
Код|Наименование поставщика|Марка топлива|Количество|\n"));
fprintf(ff,"\
-----------------------------------------------------\n");
}

/*******************************/
/*Смотрим былали передача топлива между двумя датами*/
/****************************************/
double per_top(short dn,short mn,short gn,
short dk,short mk,short gk,
char *kodvod,
char *kodavt,
double ost2,
int nom_zap, //0-до последней даты 1-включая последнюю дату
int metka,
FILE *ff,FILE *ff_vse,
GtkWidget *wpredok)
{
SQL_str row;
SQLCURSOR cur;
int kolstr;
char strsql[512];
double suma=0.;

//Смотрим передачи топлива другим водителям в тойже дате на тотже автомобиль

sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd = '%d-%d-%d' and \
kap=%s and kodsp=%s and tz=1 and nz = %d and ka=%s",
gn,mn,dn,kodavt,kodvod,nom_zap+1,kodavt);

//printw("strsql=%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }
short d,m,g;
double isuma=0.;

while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  suma=iceb_u_okrug(suma,0.001);

  isuma+=suma;

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff,"%*s %02d.%02d.%04d %73s %8.8g %7.7g\n",iceb_u_kolbait(7,row[1]),row[1],d,m,g," ",suma,ost2-isuma);
  fprintf(ff_vse,"%*s %02d.%02d.%04d %119s %8.8g %7.7g\n",iceb_u_kolbait(7,row[1]),row[1],d,m,g," ",suma,ost2-isuma);
 }

//Смотрим передачи топлива другим автомобилям в тойже дате 
sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd = '%d-%d-%d' and \
kap=%s and kodsp = %s and tz=1 and ka <> %s",
gn,mn,dn,kodavt,kodvod,kodavt);

//printw("strsql=%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  suma=iceb_u_okrug(suma,0.001);
  isuma+=suma;
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff,"%*s %02d.%02d.%04d %73s %8.8g %7.7g\n",iceb_u_kolbait(7,row[1]),row[1],d,m,g," ",suma,ost2-isuma);
  fprintf(ff_vse,"%*s %02d.%02d.%04d %119s %8.8g %7.7g\n",iceb_u_kolbait(7,row[1]),row[1],d,m,g," ",suma,ost2-isuma);
 }



if(iceb_u_sravmydat(dn,mn,gn,dk,mk,gk) == 0)
 return(isuma); 
//Смотрим передачи топлива в других датах
if(metka == 0)
 sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd > '%d-%d-%d' and \
datd < '%d-%d-%d' and kap=%s and kodsp=%s and tz=1 order by datd,nomd asc",
gn,mn,dn,gk,mk,dk,kodavt,kodvod);

if(metka == 1)
 sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd > '%d-%d-%d' and \
datd <= '%d-%d-%d' and kap=%s and kodsp=%s and tz=1 order by datd,nomd asc",
gn,mn,dn,gk,mk,dk,kodavt,kodvod);

//printw("strsql=%s\n",strsql);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  suma=iceb_u_okrug(suma,0.001);
  isuma+=suma;
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff,"%*s %02d.%02d.%04d %73s %8.8g %7.7g\n",iceb_u_kolbait(7,row[1]),row[1],d,m,g," ",suma,ost2-isuma);
  fprintf(ff_vse,"%*s %02d.%02d.%04d %119s %8.8g %7.7g\n",iceb_u_kolbait(7,row[1]),row[1],d,m,g," ",suma,ost2-isuma);
 }

return(isuma);

}


/******************************************/
/******************************************/

gint upl_pvod_r1(class upl_pvod_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;


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

//Определяем список водителей 

sprintf(strsql,"select distinct kv,ka from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' order by kv,ka asc",gn,mn,dn,gk,mk,dk);

SQLCURSOR cur1;
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
  iceb_menu_soob(gettext("Не нашли ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  
class iceb_u_spisok KVKA;

float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kod_vod.ravno(),row[0],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->kod_avt.ravno(),row[1],0,0) != 0)
   continue;
  sprintf(strsql,"%s|%s",row[0],row[1]);
  KVKA.plus(strsql);

 }
char imaf[56];
sprintf(imaf,"rpl%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_vse[40];
FILE *ff_vse;

sprintf(imaf_vse,"rplv%d.lst",getpid());

if((ff_vse = fopen(imaf_vse,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_vse,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



char kodvd[20];
char kodav[20];
char naimvod[512];
char naimavt[512];
char rnavt[20];
short dd,md,gd;
int nomer1=0;
double	kolih=0.;
double  kolihi=0.;
int     nomstr=0;
int probeg;
int kol=KVKA.kolih();
float tono_kil;
int itogo_probeg=0;
double itogo_vesgr=0.0;
double itogo_tk=0.0;
double itogo_has=0.0;
double gor_nor,zagor_nor,gruz_nor,mhas_nor;
double gor_fak,zagor_fak,gruz_fak,mhas_fak;
double raznica;
double itog_nor[5];
double itog_fak[6];
memset(&itog_nor,'\0',sizeof(itog_nor));
memset(&itog_fak,'\0',sizeof(itog_fak));
double iperedano=0.;
double prin=0.,poluh=0.;
double iprin=0.,ipoluh=0.;
float kolstr2=0;
SQL_str row1;
for(int nomer=0; nomer < kol; nomer++)
 {
  iceb_pbar(data->bar,kol,++kolstr2);    
  itogo_vesgr=0.0;
  itogo_tk=0.;
  itogo_has=0.;
  itogo_probeg=0;
  double  iprobeg=0.;
  double  iprobeg_zg=0.;
  double ost1=0.,ost2=0.;
  double  ividano=0.;
  double  irash=0.;
  memset(&itog_nor,'\0',sizeof(itog_nor));
  memset(&itog_fak,'\0',sizeof(itog_fak));
  iperedano=0.;
  iprin=ipoluh=0.;  
  class iceb_u_spisok KZMT; //Код заправки|марка топлива
  class iceb_u_double SMT;
  
  iceb_u_polen(KVKA.ravno(nomer),kodvd,sizeof(kodvd),1,'|');
  iceb_u_polen(KVKA.ravno(nomer),kodav,sizeof(kodav),2,'|');

  //Читаем водителя
  memset(naimvod,'\0',sizeof(naimvod));
  sprintf(strsql,"select naik from Uplouot where kod=%s",kodvd);
  if(iceb_sql_readkey(strsql,&row,&cur1,data->window) == 1)  
    strncpy(naimvod,row[0],sizeof(naimvod)-1);

  //Читаем автомобиль
  memset(rnavt,'\0',sizeof(rnavt));
  memset(naimavt,'\0',sizeof(naimavt));
  sprintf(strsql,"select nomz,naik from Uplavt where kod=%s",kodav);
  if(iceb_sql_readkey(strsql,&row,&cur1,data->window) == 1)  
   {
    strncpy(rnavt,row[0],sizeof(rnavt)-1);
    strncpy(naimavt,row[1],sizeof(naimavt)-1);
   }

  if(nomer != 0)
   {
    fprintf(ff,"\f");
    fprintf(ff_vse,"\f");
   }   

   uplrpl_nad(dn,mn,gn,dk,mk,gk,kodvd,naimvod,kodav,naimavt,rnavt,ff);
   uplrpl_nad(dn,mn,gn,dk,mk,gk,kodvd,naimvod,kodav,naimavt,rnavt,ff_vse);

   fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------\n");

   fprintf(ff,gettext("\
Номер  |  Дата    |Показ.спидом.|Пробег|Норма |Пробег|Норма |Остаток|Принято|Получе-|Затраты|Передача|Остаток|\n\
п.листа|          |выезд |возвр.|город |спис. |за гор|спис. |       |       | но    |       | топлива|       |\n"));
                                                                                     
   fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------\n");

   fprintf(ff_vse,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

   fprintf(ff_vse,gettext("\
Номер  |  Дата    |Показ.спидом.|Пробег| Груз |Выполнено |Норма |Отраб.|Н.с.  |Пробег|Норма |Пробег|Норма |Остаток|Принято|Получе-|Затраты|Передача|Остаток|Затраты топлива (по норме/факт)| Итого  | Разница|\n\
п.листа|          |выезд |возвр.|      |      | т/км     | т/км |м/час |м/час |город |спис. |за гор|спис. |       |       |  но   |       | топлива|       |По гор.|За гор.|На груз|М/час  |        |        |\n"));

   fprintf(ff_vse,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  sprintf(strsql,"select datd,kp,nomd,psv,psz,ztfa,prob,nst,ztfazg,probzg,nstzg,\
vesg,ztvsn,ztvsf,vrr,ztvrn,ztvrf,tk,nz,otmet from Upldok where \
datd >= '%d-%d-%d' and datd <= '%d-%d-%d' and kv=%s and ka=%s order by datd asc",
  gn,mn,dn,gk,mk,dk,kodvd,kodav); 

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  nomstr=0;  

  short dp=0,mp=0,gp=0;  
  int nom_zap=0;  
  while(cur.read_cursor(&row) != 0)
   {
    nomstr++;
    if(iceb_u_proverka(data->rk->kod_pod.ravno(),row[1],0,0) != 0)
     continue;
    iceb_u_rsdat(&dd,&md,&gd,row[0],2);

    sprintf(strsql,"%02d.%02d.%d %2s %s\n",dd,md,gd,row[1],row[2]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    if(dp != 0)
     {
      //смотрим былали передача топлива между двумя путевыми листами
      iperedano+=per_top(dp,mp,gp,dd,md,gd,kodvd,kodav,ost2,nom_zap,0,ff,ff_vse,data->window);
     }
    nom_zap=atoi(row[18]);
    dp=dd;
    mp=md;
    gp=gd;

    sprintf(strsql,"select kodsp,kodtp,kolih,tz,kap from Upldok1 where datd='%s' and kp=%s \
and nomd='%s'",row[0],row[1],row[2]);

    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
     { 
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
      continue;
     }
    if(kolstr1 != 0)
     {
      ost1=ostvaw(dd,md,gd,kodvd,kodav,nom_zap,NULL,NULL,data->window);
      
     }
    kolihi=0.;
    double kolspis=0;
    prin=poluh=0.;    
    while(cur1.read_cursor(&row1) != 0)
     {
      if(row1[3][0] == '1')
       {
        if(atoi(row1[4]) != 0)
         {
          fprintf(ff,"%60s %2s %4s %7.7g\n",
          " ",row1[0],row1[1],atof(row1[2]));

          fprintf(ff_vse,"%106s %2s %4s %7.7g\n",
          " ",row1[0],row1[1],atof(row1[2]));
         }
        else        
         {
          fprintf(ff,"%60s %2s %4s %7s %7.7g\n",
          " ",row1[0],row1[1]," ",atof(row1[2]));

          fprintf(ff_vse,"%106s %2s %4s %7s %7.7g\n",
          " ",row1[0],row1[1]," ",atof(row1[2]));
         }

        kolih=atof(row1[2]);
        kolih=iceb_u_okrug(kolih,0.001);

        kolihi+=kolih;
        if(atoi(row1[4]) != 0)
         prin+=kolih;
        else
         poluh+=kolih;
        sprintf(strsql,"%s|%s",row1[0],row1[1]);

        if((nomer1=KZMT.find(strsql)) < 0)
          KZMT.plus(strsql);
        SMT.plus(kolih,nomer1);
      }
     else
      {
       kolspis+=atof(row1[2]);
       kolspis=iceb_u_okrug(kolspis,0.001);
      }
     }

    ost2=ost1+kolihi-kolspis;
    ost2=iceb_u_okrug(ost2,0.001);
    
    iprobeg+=atof(row[6]);
    iprobeg_zg+=atof(row[9]);
    ividano+=kolihi;
    irash+=kolspis;

    

    fprintf(ff,"%*s %02d.%02d.%d %6s %6s %6s %6.10g %6s %6.10g %7.7g %7.7g %7.7g %7.7g %8s %7.7g %s\n",
    iceb_u_kolbait(7,row[2]),row[2],dd,md,gd,row[3],row[4],row[6],atof(row[7]),row[9],atof(row[10]),
    ost1,prin,poluh,kolspis,"",ost2,row[19]);

    iprin+=prin;
    ipoluh+=poluh;    
    probeg=atoi(row[4])-atoi(row[3]);
    itogo_probeg+=probeg;
    
    tono_kil=atof(row[17]);    
    itogo_tk+=tono_kil;

    itogo_vesgr+=atof(row[11]);
    itogo_has+=atof(row[14]);
    
    gor_nor=atof(row[6])*atof(row[7])/100.;
    gor_nor=iceb_u_okrug(gor_nor,0.001);
    
    zagor_nor=atof(row[9])*atof(row[10])/100.;
    zagor_nor=iceb_u_okrug(zagor_nor,0.001);
    
    gruz_nor=tono_kil*atof(row[12])/100.;
    gruz_nor=iceb_u_okrug(gruz_nor,0.001);

    mhas_nor=atof(row[14])*atof(row[15]);
    mhas_nor=iceb_u_okrug(mhas_nor,0.001);

    itog_nor[0]+=gor_nor;
    itog_nor[1]+=zagor_nor;
    itog_nor[2]+=gruz_nor;
    itog_nor[3]+=mhas_nor;
    itog_nor[4]+=gor_nor+zagor_nor+gruz_nor+mhas_nor;
        
    gor_fak=atof(row[5]);
    zagor_fak=atof(row[8]);
    gruz_fak=atof(row[13]);
    mhas_fak=atof(row[16]);

    raznica=(gor_fak+zagor_fak+gruz_fak+mhas_fak)-(gor_nor+zagor_nor+gruz_nor+mhas_nor);
    raznica=iceb_u_okrug(raznica,0.001);
    
    itog_fak[0]+=gor_fak;
    itog_fak[1]+=zagor_fak;
    itog_fak[2]+=gruz_fak;
    itog_fak[3]+=mhas_fak;
    itog_fak[4]+=gor_fak+zagor_fak+gruz_fak+mhas_fak;
    itog_fak[5]+=raznica;

    fprintf(ff_vse,"%*s %02d.%02d.%d %6s %6s \
%6d %6.7g %10.10g %6.7g %6.7g %6.7g \
%6s %6.10g %6s %6.7g %7.7g %7.7g %7.7g %7.7g %8s %7.7g \
%7.7g %7.7g %7.7g %7.7g %8.8g %s\n",
    iceb_u_kolbait(7,row[2]),row[2],dd,md,gd,row[3],row[4],
    probeg,atof(row[11]),tono_kil,atof(row[12]),atof(row[14]),atof(row[15]),
    row[6],atof(row[7]),row[9],atof(row[10]),
    ost1,prin,poluh,kolspis," ",ost2,
    gor_nor,zagor_nor,gruz_nor,mhas_nor,
    gor_nor+zagor_nor+gruz_nor+mhas_nor,row[19]);

    
    fprintf(ff_vse,"%155s %7.10g %7.10g %7.10g %7.10g %8.10g %8.10g\n",
    " ",gor_fak,zagor_fak,gruz_fak,mhas_fak,
    gor_fak+zagor_fak+gruz_fak+mhas_fak,
    raznica);
    
    if(nomstr < kolstr)
     {
      fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
      fprintf(ff_vse,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
     }
    
   }

   iperedano+=per_top(dp,mp,gp,dk,mk,gk,kodvd,kodav,ost2,nom_zap,1,ff,ff_vse,data->window);

  fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff,"%*s %6.10g %6s %6.10g        %7.7s %7.7g %7.7g %7.10g %8.8g\n",
  iceb_u_kolbait(32,gettext("Итого")),gettext("Итого"),iprobeg," ",iprobeg_zg," ",iprin,ipoluh,irash,iperedano);

   fprintf(ff_vse,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff_vse,"%*s \
%6d %6.7g %10.10g %6s %6.7g %6s \
%6.6g %6s %6.6g        %7.10s %7.7g %7.7g %7.7g %8.8g %7s \
%7.7g %7.7g %7.7g %7.7g %8.8g\n",
  iceb_u_kolbait(32,gettext("Итого")),gettext("Итого"),
  itogo_probeg,itogo_vesgr,itogo_tk," ",itogo_has," ",
  iprobeg," ",iprobeg_zg," ",iprin,ipoluh,irash,iperedano," ",
  itog_nor[0],itog_nor[1],itog_nor[2],itog_nor[3],itog_nor[4]);

  fprintf(ff_vse,"%155s %7.7g %7.7g %7.8g %7.7g %8.8g %8.8g\n",
  " ",itog_fak[0],itog_fak[1],itog_fak[2],itog_fak[3],itog_fak[4],itog_fak[5]);

  
  uplrpl_sapi(ff);

  uplrpl_sapi(ff_vse);

  int kol1=KZMT.kolih();

  for(nomer1=0; nomer1 < kol1; nomer1++)
    {
     iceb_u_polen(KZMT.ravno(nomer1),kodvd,sizeof(kodvd),1,'|');
     iceb_u_polen(KZMT.ravno(nomer1),kodav,sizeof(kodav),2,'|');

     //Читаем поставщика
     memset(naimvod,'\0',sizeof(naimvod));
     sprintf(strsql,"select naik from Uplouot where kod=%s",kodvd);
     if(iceb_sql_readkey(strsql,&row,&cur1,data->window) == 1)  
       strncpy(naimvod,row[0],sizeof(naimvod)-1);

     fprintf(ff,"%3s %-*.*s %-*.*s %10.10g\n",
     kodvd,
     iceb_u_kolbait(23,naimvod),iceb_u_kolbait(23,naimvod),naimvod,
     iceb_u_kolbait(13,kodav),iceb_u_kolbait(13,kodav),kodav,
     SMT.ravno(nomer1));  
     
     fprintf(ff_vse,"%3s %-*.*s %-*.*s %10.10g\n",
     kodvd,
     iceb_u_kolbait(23,naimvod),iceb_u_kolbait(23,naimvod),naimvod,
     iceb_u_kolbait(13,kodav),iceb_u_kolbait(13,kodav),kodav,
     SMT.ravno(nomer1));  
    }
  fprintf(ff,"\
-----------------------------------------------------\n");

  fprintf(ff,"\n%s__________________\n",gettext("Бухгалтер"));

  fprintf(ff_vse,"\
-----------------------------------------------------\n");

  fprintf(ff_vse,"\n%s__________________\n",gettext("Бухгалтер"));

 }


iceb_podpis(ff,data->window);
fclose(ff);

iceb_podpis(ff_vse,data->window);
fclose(ff_vse);


data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Реестр путевых листов"));

data->rk->imaf.plus(imaf_vse);

sprintf(strsql,"%s (%s)",gettext("Реестр путевых листов"),
gettext("развёрнутый"));

data->rk->naim.plus(strsql);


for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),0,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
