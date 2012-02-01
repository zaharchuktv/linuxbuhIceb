/*$Id: buhgspw_r.c,v 1.16 2011-02-21 07:35:51 sasa Exp $*/
/*09.11.2009	01.03.2007	Белых А.И.	buhgspw_r.c
Расчёт распределения административных затрат на доходы
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

#include "buhgspw.h"

class buhgspw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class buhgspw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  buhgspw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhgspw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhgspw_r_data *data);
gint buhgspw_r1(class buhgspw_r_data *data);
void  buhgspw_r_v_knopka(GtkWidget *widget,class buhgspw_r_data *data);

void rpsrsw(const char *shet,
short dn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_spisok *sp_kontr,
class iceb_u_spisok *sp_grup,
class iceb_u_double *deb_saldon,
class iceb_u_double *kre_saldon,
class iceb_u_double *deb_oborot,
class iceb_u_double *kre_oborot,
class iceb_u_double *deb_saldok,
class iceb_u_double *kre_saldok,
GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int kol_strok_na_liste;

int buhgspw_r(class buhgspw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class buhgspw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт по спискам групп контрагентов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(buhgspw_r_key_press),&data);

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

repl.plus(gettext("Расчёт по спискам групп контрагентов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(buhgspw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)buhgspw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhgspw_r_v_knopka(GtkWidget *widget,class buhgspw_r_data *data)
{
//printf("buhgspw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   buhgspw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhgspw_r_data *data)
{
 printf("buhgspw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}

/***************************/
/*счётчик строк*/
/*********************/

void buhgsp_ss(short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,int *kolstrok_ras,FILE *ff)
{

*kolstrok_ras+=1;
if(*kolstrok_ras <= kol_strok_na_liste)
 return;

*kolstrok_ras=0;

fprintf(ff,"\f");
sapgorsw(dn,mn,gn,dk,mk,gk,kollist,kolstrok_ras,ff);
*kolstrok_ras+=1;

}


/******************************************/
/******************************************/

gint buhgspw_r1(class buhgspw_r_data *data)
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

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='buhgsp.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"buhgsp.alx");
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

class iceb_u_spisok naim_sp;
class iceb_u_spisok kod_sp;
class iceb_u_spisok kod_gr;

char bros[1024];

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') != 0)
   continue;  
  if(iceb_u_SRAV(bros,"Наименование списка групп",0) == 0)
   {

    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    if(bros[0] == '\0')
     continue;

    naim_sp.plus(bros);

    cur_alx.read_cursor(&row_alx);
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    kod_sp.plus(bros);

    cur_alx.read_cursor(&row_alx);
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    iceb_fplus(0,bros,&kod_gr,&cur_alx);
       
   }
 }

int kol_spiskov=naim_sp.kolih();

if(kol_spiskov == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного списка групп контрагентов !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok sp_kontr;
class iceb_u_spisok sp_grup;
class iceb_u_double deb_saldon;
class iceb_u_double kre_saldon;
class iceb_u_double deb_oborot;
class iceb_u_double kre_oborot;
class iceb_u_double deb_saldok;
class iceb_u_double kre_saldok;

//расчёт
rpsrsw(data->rk->shet.ravno(),dn,mn,gn,dk,mk,gk,&sp_kontr,&sp_grup,&deb_saldon,&kre_saldon,&deb_oborot,&kre_oborot,&deb_saldok,&kre_saldok,data->window);


char imaf[56];
sprintf(imaf,"gsp%d.lst",getpid());
FILE *ff;

if((ff=fopen(imaf,"w")) == NULL) 
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Расчет по спискам групп контрагентов"),dn,mn,gn,dk,mk,gk,organ,ff);

int kolstrok_ras=5;
int kollist=0;
int kolgrup=0;
int kol_kontr=sp_kontr.kolih();
char naim_grup[112];
SQL_str row;
class SQLCURSOR cur;
char naim_kontr[112];
double itogo_gr[6];
memset(itogo_gr,'\0',sizeof(itogo_gr));
double itogo_ob[6];
memset(itogo_ob,'\0',sizeof(itogo_ob));
double itogo_sp[6];
memset(itogo_sp,'\0',sizeof(itogo_sp));
short metka_gr=0;
char grupa[56];

for(int ii=0; ii < kol_spiskov; ii++)
 {
  if(iceb_u_proverka(data->rk->kod_spis_kontr.ravno(),kod_sp.ravno(ii),0,0) != 0)
   continue;
  memset(itogo_sp,'\0',sizeof(itogo_sp));
  
  if(ii != 0)
   {
    fprintf(ff,"\f");
    kolstrok_ras=0;
    kollist=0;
   }

  fprintf(ff,"%s\n",naim_sp.ravno(ii));
  kolstrok_ras+=1;

  metka_gr=0;      
  kolgrup=iceb_u_pole2(kod_gr.ravno(ii),',');

  for(int kk=0; kk < kolgrup; kk++)
   {

    iceb_u_polen(kod_gr.ravno(ii),grupa,sizeof(grupa),kk+1,',');
    if(grupa[0] == '\0')
     continue;
    memset(itogo_gr,'\0',sizeof(itogo_gr));

    metka_gr++;
    if(metka_gr > 1)
     {
      fprintf(ff,"\f");
      kolstrok_ras=0;
     }

    memset(naim_grup,'\0',sizeof(naim_grup));
    sprintf(strsql,"select naik from Gkont where kod=%s",grupa);
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     strncpy(naim_grup,row[0],sizeof(naim_grup)-1);
     
    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    fprintf(ff,"%s:%s %s\n",gettext("Группа"),grupa,naim_grup);
    
    sapgorsw(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);

    int kolih_v_gr=0;
    for(int spk=0; spk < kol_kontr; spk++)
     {
      if(iceb_u_SRAV(grupa,sp_grup.ravno(spk),0) != 0)
       continue;

      if(fabs(deb_saldon.ravno(spk)) < 0.009 && \
         fabs(kre_saldon.ravno(spk)) < 0.009 && \
         fabs(deb_oborot.ravno(spk)) < 0.009 && \
         fabs(kre_oborot.ravno(spk)) < 0.009 )
            continue;
      //Узнаём наименование контрагента
      memset(naim_kontr,'\0',sizeof(naim_kontr));
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",sp_kontr.ravno(spk));
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)       
       strncpy(naim_kontr,row[0],sizeof(naim_kontr)-1);
      
      buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
            
      if(deb_saldon.ravno(spk) > kre_saldon.ravno(spk))
        fprintf(ff,"%*s %-*.*s %14.2f %14s %14.2f %14.2f ",
        iceb_u_kolbait(10,sp_kontr.ravno(spk)),sp_kontr.ravno(spk),
        iceb_u_kolbait(20,naim_kontr),iceb_u_kolbait(20,naim_kontr),naim_kontr,
        deb_saldon.ravno(spk),"",deb_oborot.ravno(spk),kre_oborot.ravno(spk));
      else
        fprintf(ff,"%*s %-*.*s %14s %14.2f %14.2f %14.2f ",
        iceb_u_kolbait(10,sp_kontr.ravno(spk)),sp_kontr.ravno(spk),
        iceb_u_kolbait(20,naim_kontr),iceb_u_kolbait(20,naim_kontr),naim_kontr,
        "",kre_saldon.ravno(spk),deb_oborot.ravno(spk),kre_oborot.ravno(spk));

      if(deb_saldok.ravno(spk) > kre_saldok.ravno(spk))
        fprintf(ff,"%14.2f\n",deb_saldok.ravno(spk));
      else
        fprintf(ff,"%14s %14.2f\n","",kre_saldok.ravno(spk));
       
      if(strlen(naim_kontr) > 20)
       {
        buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
        fprintf(ff,"%-10s %s\n"," ",&naim_kontr[20]);
       }

      itogo_gr[0]+=deb_saldon.ravno(spk);
      itogo_gr[1]+=kre_saldon.ravno(spk);

      itogo_gr[2]+=deb_oborot.ravno(spk);
      itogo_gr[3]+=kre_oborot.ravno(spk);

      itogo_gr[4]+=deb_saldok.ravno(spk);
      itogo_gr[5]+=kre_saldok.ravno(spk);
      kolih_v_gr++;      
     }

    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    fprintf(ff,"\
 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");

    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    fprintf(ff,"%*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
    iceb_u_kolbait(31,gettext("Итого по группе")),gettext("Итого по группе"),
    itogo_gr[0],itogo_gr[1],itogo_gr[2],itogo_gr[3],itogo_gr[4],itogo_gr[5]);

    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    
    fprintf(ff,"%*s %d\n",
    iceb_u_kolbait(31,gettext("Количество контрагентов в группе")),gettext("Количество контрагентов в группе"),
    kolih_v_gr); 
    
    itogo_sp[0]+=itogo_gr[0];
    itogo_sp[1]+=itogo_gr[1];
    itogo_sp[2]+=itogo_gr[2];
    itogo_sp[3]+=itogo_gr[3];
    itogo_sp[4]+=itogo_gr[4];
    itogo_sp[5]+=itogo_gr[5];
    
   }

  buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n");

  buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
  fprintf(ff,"%*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
  iceb_u_kolbait(31,gettext("Итого по списку")),gettext("Итого по списку"),
  itogo_sp[0],itogo_sp[1],itogo_sp[2],itogo_sp[3],itogo_sp[4],itogo_sp[5]);

  itogo_ob[0]+=itogo_sp[0];
  itogo_ob[1]+=itogo_sp[1];
  itogo_ob[2]+=itogo_sp[2];
  itogo_ob[3]+=itogo_sp[3];
  itogo_ob[4]+=itogo_sp[4];
  itogo_ob[5]+=itogo_sp[5];

 }

if(kol_spiskov > 1)
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
iceb_u_kolbait(31,gettext(" И т о г о")),iceb_u_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),
itogo_ob[0],itogo_ob[1],itogo_ob[2],itogo_ob[3],itogo_ob[4],itogo_ob[5]);



iceb_podpis(ff,data->window);
fclose(ff);


sprintf(strsql,"%-*s:%10.2f %10.2f\n",
iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
itogo_ob[0],itogo_ob[1]);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%-*s:%10.2f %10.2f\n",
iceb_u_kolbait(20,gettext("Оборот за период")),
gettext("Оборот за период"),
itogo_ob[2],itogo_ob[3]);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%-*s:%10.2f %10.2f\n",
iceb_u_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
itogo_ob[4],itogo_ob[5]);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);



data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт по спискам групп контрагентов"));
iceb_ustpeh(imaf,1,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
