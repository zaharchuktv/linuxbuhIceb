/*$Id: upl_rpavt_r.c,v 1.8 2011-02-21 07:35:58 sasa Exp $*/
/*18.11.2009	31.03.2008	Белых А.И.	upl_rpavt_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "upl_rpavt.h"

class upl_rpavt_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class upl_rpavt_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  upl_rpavt_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_rpavt_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_rpavt_r_data *data);
gint upl_rpavt_r1(class upl_rpavt_r_data *data);
void  upl_rpavt_r_v_knopka(GtkWidget *widget,class upl_rpavt_r_data *data);

double upl_ptw(short dn,short mn,short gn,short dk,short mk,short gk,char *kod_avt,char *kod_vod,GtkWidget *wpredok);


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

int upl_rpavt_r(class upl_rpavt_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class upl_rpavt_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка реестра путевых листов по автомобилям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(upl_rpavt_r_key_press),&data);

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

repl.plus(gettext("Распечатка реестра путевых листов по автомобилям"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(upl_rpavt_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)upl_rpavt_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_rpavt_r_v_knopka(GtkWidget *widget,class upl_rpavt_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   upl_rpavt_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_rpavt_r_data *data)
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
void upl_rplpv_nad(short dn,short mn,short gn,
short dk,short mk,short gk,
char *kodavt,
char *naimavt,
char *rnavt,
FILE *ff)
{

iceb_u_zagolov(gettext("Реестр путевых листов"),dn,mn,gn,dk,mk,gk,organ,ff);

fprintf(ff,"\n\
%s %s %s %s %s\n",
gettext("Автомобиль"),
kodavt,
naimavt,
gettext("регистрационный номер"),
rnavt);
}
/*******************************/
/*Смотрим былали передача топлива между предыдущей путёвкой и настоящей*/
/****************************************/
double per_top_avt(short dn,short mn,short gn,
short dk,short mk,short gk,
char *kodavt,
double ost2,
FILE *ff_vse,
GtkWidget *wpredok)
{
SQL_str row;
SQLCURSOR cur;
int kolstr;
char strsql[512];
double suma=0.;
short d,m,g;
double isuma=0.;


sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd >= '%d-%d-%d' and \
datd < '%d-%d-%d' and kap=%s and tz=1 and ka <> '%s'",
gn,mn,dn,gk,mk,dk,kodavt,kodavt);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  isuma+=suma;
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff_vse,"%*s %02d.%02d.%04d %118s %8.8g %7.7g\n",
  iceb_u_kolbait(7,row[1]),row[1],d,m,g," ",suma,ost2-isuma);
 }

return(isuma);

}

/*************************/
/*Смотрим от дня последней путёвки в периоде до конца заданного периода - не было ли передач топлива*/
/********************************************/
double upl_rplpa_pt(short dpp,short mpp,short gpp,
short dk,short mk,short gk,
char *kodavt,
double ost2,
FILE *ff_vse,
GtkWidget *wpredok)
{
SQL_str row;
SQLCURSOR cur;
int kolstr;
char strsql[512];
double suma=0.;
short d,m,g;
double isuma=0.;

sprintf(strsql,"select datd,nomd,kolih from Upldok1 where datd >= '%d-%d-%d' \
and datd <= '%d-%d-%d' and kap=%s and tz=1 and ka <> '%s'",
gpp,mpp,dpp,gk,mk,dk,kodavt,kodavt);

//printw("strsql=%s\n",strsql);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  isuma+=suma;
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff_vse,"%*s %02d.%02d.%04d %118s %8.8g %7.7g\n",
  iceb_u_kolbait(7,row[1]),row[1],d,m,g," ",suma,ost2-isuma);
 }

return(isuma);

}

/*******************/
/*Шапка*/
/*****************/
void upl_rpavt_sapi(FILE *ff)
{
fprintf(ff,"\
-----------------------------------------------------\n");
fprintf(ff,gettext("\
Код|Наименование поставщика|Марка топлива|Количество|\n"));
fprintf(ff,"\
-----------------------------------------------------\n");
}

/******************************************/
/******************************************/

gint upl_rpavt_r1(class upl_rpavt_r_data *data)
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




//Определяем список aвтомобилей
/********
sprintf(strsql,"select distinct ka from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' order by kv,ka asc",gn,mn,dn,gk,mk,dk);
**********/
sprintf(strsql,"select distinct ka from Upldok1 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d'",gn,mn,dn,gk,mk,dk);
int kolstr=0;
SQLCURSOR cur1;
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

class iceb_u_spisok avtomobil;

float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kod_avt.ravno(),row[0],0,0) != 0)
   continue;

  avtomobil.plus(row[0]);

 }

char imaf_vse[40];
FILE *ff_vse;

sprintf(imaf_vse,"rplpa%d.lst",getpid());

if((ff_vse = fopen(imaf_vse,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_vse,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char naimavt[512];
char rnavt[20];
char kodav[30];
int     nomstr=0;
int probeg=0;
double tono_kil;
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
double kolihi=0.;
double kolih=0.;
int nomer1=0;
int kol=avtomobil.kolih();
kolstr1=0;
float kolstr2=0;
double prin=0.;
double poluh=0.;
double iprin=0.;
double ipoluh=0.;
double iperedano=0.;
SQL_str row1;
for(int nomer=0; nomer < kol; nomer++)
 {
  iceb_pbar(data->bar,kol,++kolstr2);    
  iperedano=0.;
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

  iprin=0.;
  ipoluh=0.;

  strncpy(kodav,avtomobil.ravno(nomer),sizeof(kodav)-1);

  class iceb_u_spisok KZMT; //Код заправки|марка топлива
  class iceb_u_double SMT;

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
    fprintf(ff_vse,"\f");

  upl_rplpv_nad(dn,mn,gn,dk,mk,gk,kodav,naimavt,rnavt,ff_vse);

  fprintf(ff_vse,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff_vse,gettext("\
Номер  |  Дата    |Показ.спидом.|Пробег| Груз |Выполнено |Норма |Отраб.|Н.с.  |Пробег|Норма |Пробег|Норма |Остаток|Приня- |Получе-|Затраты|Переда-|Остаток|Затраты топлива (по норме/факт)| Итого  | Разница|\n\
п.листа|          |выезд |возвр.|      |      | т/км     | т/км |м/час |м/час |город |спис. |за гор|спис. |       |  то   |  но   |       |  но   |       |По гор.|За гор.|На груз|М/час  |        |        |\n"));
//1        2         3      4      5      6        7         8     9      10     11    12     13      14     15       16     17      18     19        20     21      22     23      24       25       26                                          
  fprintf(ff_vse,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  sprintf(strsql,"select datd,kp,nomd,psv,psz,ztfa,prob,nst,ztfazg,probzg,nstzg,\
vesg,ztvsn,ztvsf,vrr,ztvrn,ztvrf,tk,kv,nz,otmet from Upldok where \
datd >= '%d-%d-%d' and datd <= '%d-%d-%d' and ka=%s order by datd,nomd asc",
  gn,mn,dn,gk,mk,dk,kodav); 

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
/********
  printw("\n%s=%s\n",
  gettext("Код автомобиля"),
  kodav);
  refresh();    
***************/
  short dd=0,md=0,gd=0;  
  short dp=0,mp=0,gp=0;  
  nomstr=0;  
  int nom_zap=0;
  while(cur.read_cursor(&row) != 0)
   {
    nomstr++;
    if(iceb_u_proverka(data->rk->kod_pod.ravno(),row[1],0,0) != 0)
     continue;
    if(iceb_u_proverka(data->rk->kod_vod.ravno(),row[18],0,0) != 0)
     continue;

    iceb_u_rsdat(&dd,&md,&gd,row[0],2);
    nom_zap=atoi(row[19]);

    sprintf(strsql,"%02d.%02d.%d %2s %s\n",dd,md,gd,row[1],row[2]);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

/************************
   Несколько передач топлива с одной машины на другие в течение одного дня не должно быть
   а если будут то пусть она их все покажет после первого путевого листа в общей сумме
*********************/
    if(dp != 0 && iceb_u_sravmydat(dp,mp,gp,dd,md,gd) != 0)
     {
      //смотрим былали передача топлива между двумя путевыми листами
      iperedano+=per_top_avt(dp,mp,gp,dd,md,gd,kodav,ost2,ff_vse,data->window);
     }
    dp=dd;
    mp=md;
    gp=gd;

    sprintf(strsql,"select kodsp,kodtp,kolih,tz,kap,ka from Upldok1 where datd='%s' and kp=%s \
and nomd='%s'",row[0],row[1],row[2]);

    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
      continue;
     }
    ost1=ostvaw(dd,md,gd,"",kodav,nom_zap,NULL,NULL,data->window);

    kolihi=0.;
    double kolspis=0;
    prin=poluh=0.;    
    while(cur1.read_cursor(&row1) != 0)
     {
      if(row1[3][0] == '1')
       {
        if(atoi(row1[4]) != 0)
         {
          if(iceb_u_SRAV(row1[4],row1[5],0) == 0) //игнорируем передачи топлива между водителями на одной машине
           {
            sprintf(strsql,"Игнорируем передачи топлива между водителями на одной машине.\n");
            iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
            continue;
           }
          fprintf(ff_vse,"%106s %2s %4s %7.7g\n",
          " ",row1[0],row1[1],atof(row1[2]));

         }         
        else        
          fprintf(ff_vse,"%106s %2s %4s %7s %7.7g\n",
          " ",row1[0],row1[1]," ",atof(row1[2]));
        
        kolih=atof(row1[2]);

        if(atoi(row1[4]) != 0)
         prin+=kolih;
        else
         poluh+=kolih;
         
        kolihi+=kolih;
        sprintf(strsql,"%s|%s",row1[0],row1[1]);

        if((nomer1=KZMT.find(strsql)) < 0)
          KZMT.plus(strsql);
        SMT.plus(kolih,nomer1);
      }
     else
      {
       kolspis+=atof(row1[2]);
      }
     }
    iprin+=prin;
    ipoluh+=poluh;
    
    ost2=ost1+kolihi-kolspis;
    ost2=iceb_u_okrug(ost2,0.001);
    
    iprobeg+=atof(row[6]);
    iprobeg_zg+=atof(row[9]);
    ividano+=kolihi;
    irash+=kolspis;



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
%6s %6.10g %6s %6.7g %7.7g %7.7g %7.7g %7.7g %7s %7.7g \
%7.7g %7.7g %7.7g %7.7g %8.8g %s\n",
    iceb_u_kolbait(7,row[2]),row[2],dd,md,gd,row[3],row[4],
    probeg,atof(row[11]),tono_kil,atof(row[12]),atof(row[14]),atof(row[15]),
    row[6],atof(row[7]),row[9],atof(row[10]),
    ost1,prin,poluh,kolspis," ",ost2,
    gor_nor,zagor_nor,gruz_nor,mhas_nor,
    gor_nor+zagor_nor+gruz_nor+mhas_nor,row[20]);

    
    fprintf(ff_vse,"%154s %7.10g %7.10g %7.10g %7.10g %8.10g %8.10g\n",
    " ",gor_fak,zagor_fak,gruz_fak,mhas_fak,
    gor_fak+zagor_fak+gruz_fak+mhas_fak,
    raznica);
    
    if(nomstr < kolstr)
      fprintf(ff_vse,"\
 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
   }
  if(dd != 0)
   iperedano+=upl_rplpa_pt(dd,md,gd,dk,mk,gk,kodav,ost2,ff_vse,data->window);
   
/********************
  if(dp != 0 && sravmydat(dp,mp,gp,dk,mk,gk) != 0)
   {
    printw("Второй вызов.\n");
    iperedano+=per_top_avt(dp,mp,gp,dk,mk,gk,kodav,ost2,1,ff_vse);
   }
**********************/
  fprintf(ff_vse,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff_vse,"%*s \
%6d %6.7g %10.10g %6s %6.7g %6s \
%6.6g %6s %6.6g        %7s %7.7g %7.7g %7.7g %7.7g %7s \
%7.7g %7.7g %7.7g %7.7g %8.8g\n",
  iceb_u_kolbait(32,gettext("Итого")),gettext("Итого"),
  itogo_probeg,itogo_vesgr,itogo_tk," ",itogo_has," ",
  iprobeg," ",iprobeg_zg," ",iprin,ipoluh,irash,iperedano," ",
  itog_nor[0],itog_nor[1],itog_nor[2],itog_nor[3],itog_nor[4]);

  fprintf(ff_vse,"%154s %7.7g %7.7g %7.8g %7.7g %8.8g %8.8g\n",
  " ",itog_fak[0],itog_fak[1],itog_fak[2],itog_fak[3],itog_fak[4],itog_fak[5]);


  int kol1=KZMT.kolih();
  char kodvd[30];
  char naimvod[512];

  upl_rpavt_sapi(ff_vse);

  for(nomer1=0; nomer1 < kol1; nomer1++)
    {
     iceb_u_polen(KZMT.ravno(nomer1),kodvd,sizeof(kodvd),1,'|');
     iceb_u_polen(KZMT.ravno(nomer1),kodav,sizeof(kodav),2,'|');

     //Читаем поставщика
     memset(naimvod,'\0',sizeof(naimvod));
     sprintf(strsql,"select naik from Uplouot where kod=%s",kodvd);
     if(sql_readkey(&bd,strsql,&row,&cur1) == 1)  
       strncpy(naimvod,row[0],sizeof(naimvod)-1);

     fprintf(ff_vse,"%3s %-*.*s %-*.*s %10.10g\n",
     kodvd,
     iceb_u_kolbait(23,naimvod),iceb_u_kolbait(23,naimvod),naimvod,
     iceb_u_kolbait(13,kodav),iceb_u_kolbait(13,kodav),kodav,
     SMT.ravno(nomer1));  
    }

  fprintf(ff_vse,"\
-----------------------------------------------------\n");

  fprintf(ff_vse,"\n%s__________________\n",gettext("Бухгалтер"));


 }

iceb_podpis(ff_vse,data->window);
fclose(ff_vse);



data->rk->imaf.plus(imaf_vse);
data->rk->naim.plus(gettext("Реестр путевых листов"));

iceb_ustpeh(imaf_vse,0,data->window);




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
