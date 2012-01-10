/*$Id: upl_shkg_r.c,v 1.9 2011-02-21 07:35:58 sasa Exp $*/
/*18.11.2009	02.03.2008	Белых А.И.	upl_shkg_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "upl_shkg.h"

class upl_shkg_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class upl_shkg_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  upl_shkg_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_shkg_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_shkg_r_data *data);
gint upl_shkg_r1(class upl_shkg_r_data *data);
void  upl_shkg_r_v_knopka(GtkWidget *widget,class upl_shkg_r_data *data);

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

int upl_shkg_r(class upl_shkg_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class upl_shkg_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка ведомости списания топлива по счетам в киллограммах"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(upl_shkg_r_key_press),&data);

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

repl.plus(gettext("Распечатка ведомости списания топлива по счетам в киллограммах"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(upl_shkg_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)upl_shkg_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_shkg_r_v_knopka(GtkWidget *widget,class upl_shkg_r_data *data)
{
if(data->kon_ras == 1) return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   upl_shkg_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_shkg_r_data *data)
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

void    svst_kg(int *kollist,int *kolstrok,FILE *ff)
{
*kollist+=1;
*kolstrok+=5;

fprintf(ff,"%70s%s N%d\n","",gettext("Лист"),*kollist);

fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Счет |Код топлива|Расход топлива|Коэффициент|Расход топлива|Средняя| Сумма |\n\
      |           |   в литрах   | перевода  |в киллограммах| цена  |       |\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
//123456 12345678901 12345678901234 12345678901 12345678901234 1234567 1234567
}

/********************************/
/*Счетчик                       */
/********************************/
void     shetvst_kg(int *kolstrok,int *kollist,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolstrok=0;
  svst_kg(kollist,kolstrok,ff);
 }  

}
/****************************/
/*Выдача итоговой строки*/
/**************************/
void uplvst_kg_it(int metka, //0-итог по счёту 1-общий
double *itog,FILE *ff)
{

char strsql[512];
if(metka == 0)
 sprintf(strsql,"%s",gettext("Итого по счёту"));
if(metka == 1)
 sprintf(strsql,"%s",gettext("Общий итог"));

fprintf(ff,"%*s:%11.11g %11s %14.14g %7s %7.2f\n",
iceb_u_kolbait(21,strsql),strsql,itog[0]," ",itog[1]," ",itog[2]);



}

/******************************************/
/******************************************/

gint upl_shkg_r1(class upl_shkg_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;

printf("%s\n",__FUNCTION__);

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
char    imaf[56];
int	kollist=0;
int	kolstrok=0;

sprintf(strsql,"select datd,nomd,kt,shet,pr,przg,prgr,vrrd,shet,kv from Upldok2 where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' order by shet asc",gn,mn,dn,gk,mk,dk);


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



char kodmt[32];
char shetz[32];

memset(kodmt,'\0',sizeof(kodmt));
memset(shetz,'\0',sizeof(shetz));

class iceb_u_spisok shet_kodt; //Список счёт->код топлива
class iceb_u_double suma_shet_kodt; //Расход топлива в литрах

float kolstr1=0;
int nomer=0;
double suma;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->kod_top.ravno(),row[2],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->kod_vod.ravno(),row[9],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[8],0,0) != 0)
   continue;

  suma=0.;
  sprintf(strsql,"select nst,nstzg,ztvsn,ztvrn from Upldok \
where datd = '%s' and nomd='%s'",row[0],row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    suma+=atof(row[4])*atof(row1[0])/100.;
    suma+=atof(row[5])*atof(row1[1])/100.;
    suma+=atof(row[6])*atof(row1[2])/100.;
    suma+=atof(row[7])*atof(row1[3]);
    suma=iceb_u_okrug(suma,0.001);
   }  
  else
   {
    sprintf(strsql,"%s\n%s %s",gettext("Не нашли шапку документа!"),row[0],row[1]);
    iceb_menu_soob(strsql,data->window);    
    continue;    
   }
  
  sprintf(strsql,"%s|%s",row[3],row[2]);

  if((nomer=shet_kodt.find(strsql)) == -1)
   shet_kodt.plus(strsql);

  suma_shet_kodt.plus(suma,nomer);
 }

sprintf(imaf,"vstkg%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


kolstrok=5;
iceb_u_zagolov(gettext("Ведомость списания топлива по счетам в киллограммах"),dn,mn,gn,dk,mk,gk,organ,ff);

if(data->rk->kod_top.ravno()[0] != '\0')
 {
  kolstrok++;
  fprintf(ff,"%s:%s\n",gettext("Код топлива"),data->rk->kod_top.ravno());  
 }

if(data->rk->kod_vod.ravno()[0] != '\0')
 {
  kolstrok++;
  fprintf(ff,"%s:%s\n",gettext("Код водителя"),data->rk->kod_vod.ravno());  
 }

if(data->rk->shet.ravno()[0] != '\0')
 {
  kolstrok++;
  fprintf(ff,"%s:%s\n",gettext("Счет"),data->rk->shet.ravno());  
 }

svst_kg(&kollist,&kolstrok,ff);
char shet_sp[32];
char kod_top[32];
double koef=0.;
double cn_suma=0.;
char shet_sp_z[32];
double srcn=0.;
double itog_shet[3];

memset(&itog_shet,'\0',sizeof(itog_shet));

double itog[3];
memset(&itog,'\0',sizeof(itog));

memset(shet_sp,'\0',sizeof(shet_sp));
memset(shet_sp_z,'\0',sizeof(shet_sp_z));
for(int i=0; i < shet_kodt.kolih() ; i++)
 {
  iceb_u_polen(shet_kodt.ravno(i),shet_sp,sizeof(shet_sp),1,'|');  
  iceb_u_polen(shet_kodt.ravno(i),kod_top,sizeof(kod_top),2,'|');  
  if(iceb_u_SRAV(shet_sp,shet_sp_z,0) != 0)
   {
    if(shet_sp_z[0] != '\0')
     uplvst_kg_it(0,itog_shet,ff);
    
    memset(&itog_shet,'\0',sizeof(itog_shet));
    strcpy(shet_sp_z,shet_sp);
   }  


  //читаем среднюю цену и коэффициент перевода
  koef=0.;
  srcn=0.;
  sprintf(strsql,"select srcn,kp from Uplmtsc where datsc <= '%04d-%02d-%02d' and kodt='%s' \
order by datsc desc",
  gn,mn,dn,kod_top);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
   {
    srcn=atof(row[0]);
    koef=atof(row[1]);
   }  
  suma=koef*suma_shet_kodt.ravno(i);
  suma=iceb_u_okrug(suma,0.001);
  
  shetvst_kg(&kolstrok,&kollist,ff);
  
  cn_suma=srcn*suma;
  
  itog_shet[0]+=suma_shet_kodt.ravno(i);
  itog_shet[1]+=suma;
  itog_shet[2]+=cn_suma;

  itog[0]+=suma_shet_kodt.ravno(i);
  itog[1]+=suma;
  itog[2]+=cn_suma;
  
  fprintf(ff,"%-*s %-*s %14.14g %11.11g %14.14g %7.2f %7.2f\n",
  iceb_u_kolbait(6,shet_sp),shet_sp,
  iceb_u_kolbait(11,kod_top),kod_top,
  suma_shet_kodt.ravno(i),koef,suma,srcn,cn_suma);
  
 }
uplvst_kg_it(0,itog_shet,ff);
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
uplvst_kg_it(1,itog,ff);

iceb_podpis(ff,data->window);
fclose(ff);


data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Ведомость списания топлива по счетам в киллограммах"));
iceb_ustpeh(imaf,0,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
