/*$Id: upl_shvod_r.c,v 1.10 2011-02-21 07:35:58 sasa Exp $*/
/*18.11.2009	02.03.2008	Белых А.И.	upl_shvod_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "upl_shvod.h"

class upl_shvod_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class upl_shvod_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  upl_shvod_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_shvod_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_shvod_r_data *data);
gint upl_shvod_r1(class upl_shvod_r_data *data);
void  upl_shvod_r_v_knopka(GtkWidget *widget,class upl_shvod_r_data *data);

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

int upl_shvod_r(class upl_shvod_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class upl_shvod_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка ведомости списания топлива по водителям и счетам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(upl_shvod_r_key_press),&data);

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

repl.plus(gettext("Распечатка ведомости списания топлива по водителям и счетам"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(upl_shvod_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)upl_shvod_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_shvod_r_v_knopka(GtkWidget *widget,class upl_shvod_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   upl_shvod_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_shvod_r_data *data)
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
/*************************************/
/*Шапка                              */
/*************************************/

void    svst(int *kollist,int *kolstrok,FILE *ff)
{
*kollist+=1;
*kolstrok+=4;

fprintf(ff,"%70s%s N%d\n","",gettext("Лист"),*kollist);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Счет |К/в |Фамилия водителя|К.топ.|  Дата    |Ном.док.|Пробег|Фак.з.т.|По норме| Часы |Недорасход|Перерасход|\n"));
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------\n");
//123456 1234 1234567890123456 123456 1234567890 12345678 123456 12345678 1234
}

/************************************/
/*Итоговая строка                   */
/************************************/

void  itogvst(int metkai, //0-Итог по топливу 1-по водителю 2-по счету
char *kodi, //Код итога
double probeg,double toplivo,
double zat_top_nor,
double has,
double ned,
double per,
FILE *ff,
FILE *ff_shet)
{
char  strsql[512];
char  stroka[1024];
SQL_str  row;
SQLCURSOR curr;
if(probeg == 0. && toplivo == 0. && has == 0.)
  return;
  
memset(stroka,'\0',sizeof(stroka));

if(metkai == 0) //По топливу
 sprintf(stroka,"%s %s %s",
 gettext("Итого"),
 gettext("по"),
 kodi);

if(metkai == 1) //По водителю
 {
  sprintf(stroka,"%s %s %s ",
  gettext("Итого"),
  gettext("по"),
  kodi);

  sprintf(strsql,"select naik from Uplouot where kod=%s",kodi);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strcat(stroka,row[0]);
 }

if(metkai == 2) //По счету
 {
  sprintf(stroka,"%s %s %s ",
  gettext("Итого"),
  gettext("по"),
  kodi);

  sprintf(strsql,"select nais from Plansh where ns='%s'",kodi);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strcat(stroka,row[0]);

  fprintf(ff_shet,"%6s %*.*s %6.6g %8.8g %8.8g %6.2f %10.10g %10.10g\n",
  kodi,iceb_u_kolbait(20,row[0]),iceb_u_kolbait(20,row[0]),row[0],probeg,toplivo,zat_top_nor,has,ned,per);

 }
 
fprintf(ff,"%*s:%6.6g %8.8g %8.8g %6.2f %10.10g %10.10g\n",
iceb_u_kolbait(55,strsql),stroka,probeg,toplivo,zat_top_nor,has,ned,per);

}

/********************************/
/*Счетчик                       */
/********************************/
void     shetvst(int *kolstrok,int *kollist,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolstrok=0;
  svst(kollist,kolstrok,ff);
 }  

}
/*********************************/
/*Реквизиты поиска */
/****************************/
void uplvst_rp(int *kolstrok,const char *toplivo,
const char *voditel,
const char *shet,
const char *kod_avtom,
FILE *ff)
{
if(toplivo[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Код топлива"),toplivo);  
 }

if(voditel[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Код водителя"),voditel);  
 }

if(shet[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Счет"),shet);  
 }
if(kod_avtom[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Код автомобиля"),kod_avtom);  
 }
}


/******************************************/
/******************************************/

gint upl_shvod_r1(class upl_shvod_r_data *data)
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




long	kolstr=0;
SQL_str row1;
FILE    *ff;
char    imaf[30];
int	kollist=0;
int	kolstrok=0;
short   d,m,g;

sprintf(strsql,"select * from Upldok2 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' order by shet,kv,kt,datd asc",gn,mn,dn,gk,mk,dk);

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

sprintf(imaf,"vst%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


kolstrok=5;
iceb_u_zagolov(gettext("Ведомость списания топлива по счетам и водителям"),dn,mn,gn,dk,mk,gk,organ,ff);

uplvst_rp(&kolstrok,data->rk->kod_top.ravno(),data->rk->kod_vod.ravno(),data->rk->shet.ravno(),data->rk->kod_avt.ravno(),ff);

char imaf_shet[32];
FILE *ff_shet;
sprintf(imaf_shet,"vsts%d.lst",getpid());

if((ff_shet = fopen(imaf_shet,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_shet,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff_shet);


iceb_u_zagolov(gettext("Cписание топлива по счетам"),dn,mn,gn,dk,mk,gk,organ,ff_shet);

uplvst_rp(&kolstrok,data->rk->kod_top.ravno(),data->rk->kod_vod.ravno(),data->rk->shet.ravno(),data->rk->kod_avt.ravno(),ff_shet);

fprintf(ff_shet,"\
----------------------------------------------------------------------------------\n");
fprintf(ff_shet,gettext("\
 Счёт |Наименование счёта  |Пробег|Фак.з.т.|По норме| Часы |Недорасход|Перерасход|\n"));
fprintf(ff_shet,"\
----------------------------------------------------------------------------------\n");




svst(&kollist,&kolstrok,ff);

double iprobeg[3];
double itoplivo[3];
double ihas[3];
double izat_top_nor[3];
double  sprobeg=0.;
double stoplivo=0.;
double shas=0.;
char naimvod[512];

memset(&iprobeg,'\0',sizeof(iprobeg));
memset(&itoplivo,'\0',sizeof(itoplivo));
memset(&ihas,'\0',sizeof(ihas));
memset(&izat_top_nor,'\0',sizeof(izat_top_nor));

char kodvod[20];
char kodmt[20];
char shetz[20];
char nomdok[32];

memset(kodvod,'\0',sizeof(kodvod));
memset(kodmt,'\0',sizeof(kodmt));
memset(shetz,'\0',sizeof(shetz));
memset(nomdok,'\0',sizeof(nomdok));

float kolstr1=0;
double nst,nstzg,ztvsn,ztvrn;
double zat_top_nor=0.;
double raznica=0.;
double nedorashod_top=0.,pererashod_top=0.;
double nedorashod_vod=0.,pererashod_vod=0.;
double nedorashod_shet=0.,pererashod_shet=0.;
class SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->kod_top.ravno(),row[3],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->kod_vod.ravno(),row[9],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->kod_avt.ravno(),row[10],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[4],0,0) != 0)
   continue;
     

  if(iceb_u_SRAV(kodmt,row[3],0) != 0)
   {
    if(kodmt[0] != '\0')
     {
      shetvst(&kolstrok,&kollist,ff);
      itogvst(0,kodmt,iprobeg[0],itoplivo[0],izat_top_nor[0],ihas[0],nedorashod_top,pererashod_top,ff,NULL);
     }
    nedorashod_top=pererashod_top=0.;
    iprobeg[0]=0.;
    itoplivo[0]=ihas[0]=0.;
    izat_top_nor[0]=0.;
    strcpy(kodmt,row[3]);
   }

  if(iceb_u_SRAV(kodvod,row[9],0) != 0)
   {
    if(kodvod[0] != '\0')
     {
      shetvst(&kolstrok,&kollist,ff);
      itogvst(0,kodmt,iprobeg[0],itoplivo[0],izat_top_nor[0],ihas[0],nedorashod_top,pererashod_top,ff,NULL);

      shetvst(&kolstrok,&kollist,ff);
      itogvst(1,kodvod,iprobeg[1],itoplivo[1],izat_top_nor[1],ihas[1],nedorashod_vod,pererashod_vod,ff,NULL);
     }
    nedorashod_top=pererashod_top=0.;
    nedorashod_vod=pererashod_vod=0.;

    iprobeg[0]=0.;
    itoplivo[0]=ihas[0]=0.;
    izat_top_nor[0]=0.;

    iprobeg[1]=0.;
    itoplivo[1]=ihas[1]=0.;
    izat_top_nor[1]=0.;

    strcpy(kodvod,row[9]);

    memset(naimvod,'\0',sizeof(naimvod));
    sprintf(strsql,"select naik from Uplouot where kod=%s",kodvod);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     strncpy(naimvod,row1[0],sizeof(naimvod)-1);    
   }

  if(iceb_u_SRAV(shetz,row[4],0) != 0)
   {
    if(shetz[0] != '\0')
     {
      shetvst(&kolstrok,&kollist,ff);
      itogvst(0,kodmt,iprobeg[0],itoplivo[0],izat_top_nor[0],ihas[0],nedorashod_top,pererashod_top,ff,NULL);

      shetvst(&kolstrok,&kollist,ff);
      itogvst(1,kodvod,iprobeg[1],itoplivo[1],izat_top_nor[1],ihas[1],nedorashod_vod,pererashod_vod,ff,NULL);

      shetvst(&kolstrok,&kollist,ff);
      itogvst(2,shetz,iprobeg[2],itoplivo[2],izat_top_nor[2],ihas[2],nedorashod_shet,pererashod_shet,ff,ff_shet);

     }
    nedorashod_top=pererashod_top=0.;
    nedorashod_vod=pererashod_vod=0.;
    nedorashod_shet=pererashod_shet=0.;
    memset(&iprobeg,'\0',sizeof(iprobeg));
    memset(&itoplivo,'\0',sizeof(itoplivo));
    memset(&ihas,'\0',sizeof(ihas));
    memset(&izat_top_nor,'\0',sizeof(izat_top_nor));

    strcpy(shetz,row[4]);
   }
  //читаем нормы расхода топлива
  nst=nstzg=ztvsn=ztvrn=0.;
  sprobeg=0.;
  sprintf(strsql,"select nst,nstzg,ztvsn,ztvrn,psv,psz from Upldok \
where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    nst=atof(row1[0]);
    nstzg=atof(row1[1]);
    ztvsn=atof(row1[2]);
    ztvrn=atof(row1[3]);
   }  
  else
   {
    sprintf(strsql,"%s\n%s %s",gettext("Не нашли шапку документа!"),row[0],row[2]);
    iceb_menu_soob(strsql,data->window);    
    continue;    
   }

  sprobeg=atof(row[6])+atof(row[13]);

  iceb_u_rsdat(&d,&m,&g,row[0],2);

  shetvst(&kolstrok,&kollist,ff);


  stoplivo=atof(row[7])+atof(row[14])+atof(row[15])+atof(row[16]);
  stoplivo=iceb_u_okrug(stoplivo,0.001);  

  //определяем затраты топлива по норме
  zat_top_nor=atof(row[6])*nst/100.+atof(row[13])*nstzg/100.+atof(row[17])*ztvsn/100.+atof(row[18])*ztvrn;
  zat_top_nor=iceb_u_okrug(zat_top_nor,0.001);

  raznica=stoplivo-zat_top_nor;
  raznica=iceb_u_okrug(raznica,0.001);
  
  fprintf(ff,"%-*s %-*s %-*.*s %-*s %02d.%02d.%04d %-*s %6.6g %8.8g %8.8g %6s",
  iceb_u_kolbait(6,row[4]),row[4],
  iceb_u_kolbait(4,row[9]),row[9],
  iceb_u_kolbait(16,naimvod),iceb_u_kolbait(16,naimvod),naimvod,
  iceb_u_kolbait(6,row[3]),row[3],
  d,m,g,
  iceb_u_kolbait(8,row[2]),row[2],
  sprobeg,stoplivo,zat_top_nor,row[8]);  

  if(raznica < 0)
   {
    fprintf(ff," %10.10g\n",raznica);
    nedorashod_top+=raznica;
    nedorashod_vod+=raznica;
    nedorashod_shet+=raznica;
   }
  else
   {
    fprintf(ff," %10s %10.10g\n"," ",raznica);
    pererashod_top+=raznica;    
    pererashod_vod+=raznica;    
    pererashod_shet+=raznica;    
   }
  
  shas=atof(row[8]);

  iprobeg[0]+=sprobeg;
  itoplivo[0]+=stoplivo;
  ihas[0]+=shas;
  izat_top_nor[0]+=zat_top_nor;
  
  iprobeg[1]+=sprobeg;
  itoplivo[1]+=stoplivo;
  ihas[1]+=shas;
  izat_top_nor[1]+=zat_top_nor;

  iprobeg[2]+=sprobeg;
  itoplivo[2]+=stoplivo;
  ihas[2]+=shas;
  izat_top_nor[2]+=zat_top_nor;

 }

itogvst(0,kodmt,iprobeg[0],itoplivo[0],izat_top_nor[0],ihas[0],nedorashod_top,pererashod_top,ff,NULL);
itogvst(1,kodvod,iprobeg[1],itoplivo[1],izat_top_nor[1],ihas[1],nedorashod_vod,pererashod_vod,ff,NULL);
itogvst(2,shetz,iprobeg[2],itoplivo[2],izat_top_nor[2],ihas[2],nedorashod_shet,pererashod_shet,ff,ff_shet);

iceb_podpis(ff,data->window);
iceb_podpis(ff_shet,data->window);
fclose(ff);
fclose(ff_shet);



data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Ведомость списания топлива по счетам и водителям"));
data->rk->imaf.plus(imaf_shet);
data->rk->naim.plus(gettext("Cписание топлива по счетам"));

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
