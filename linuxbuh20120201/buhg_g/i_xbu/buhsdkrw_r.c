/*$Id: buhsdkrw_r.c,v 1.17 2011-02-21 07:35:51 sasa Exp $*/
/*10.11.2009	01.03.2007	Белых А.И.	buhsdkrw_r.c
Расчёт отчёта по заданному списку дебетовых и кредитовых счетов
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
#include "buhsdkrw.h"

class buhsdkrw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class buhsdkrw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  buhsdkrw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhsdkrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhsdkrw_r_data *data);
gint buhsdkrw_r1(class buhsdkrw_r_data *data);
void  buhsdkrw_r_v_knopka(GtkWidget *widget,class buhsdkrw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

int buhsdkrw_r(class buhsdkrw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class buhsdkrw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт по заданному списку дебетовых и кредитовых счетов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(buhsdkrw_r_key_press),&data);

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

repl.plus(gettext("Расчёт по заданному списку дебетовых и кредитовых счетов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(buhsdkrw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)buhsdkrw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhsdkrw_r_v_knopka(GtkWidget *widget,class buhsdkrw_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   buhsdkrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhsdkrw_r_data *data)
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
/*******************************/
/*Расчёт по заданным настройкам*/
/********************************/

double buhsdkr_r(short dn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_spisok *sp_sheta_deb,
class iceb_u_spisok *sp_sheta_kre,
const char *shet_d,const char *shet_k,const char *shet_k_minus,
int kolstrp,
class SQLCURSOR *curp,
FILE *ff_dv,FILE *ff_kv,
GtkWidget *bar)
{
SQL_str row;
class iceb_u_spisok sheta_deb;
class iceb_u_spisok sheta_kre;

for(int ii=0; ii < sp_sheta_deb->kolih(); ii++)
 {
  if(iceb_u_proverka(shet_d,sp_sheta_deb->ravno(ii),0,0) != 0)
   continue;
  sheta_deb.plus(sp_sheta_deb->ravno(ii));
 } 

for(int ii=0; ii < sp_sheta_kre->kolih(); ii++)
 {
  if(iceb_u_proverka(shet_k,sp_sheta_kre->ravno(ii),0,0) != 0)
   continue;
  sheta_kre.plus(sp_sheta_kre->ravno(ii));
 } 
 
class iceb_u_double sum_pr; //Массив сумм проводок
int kolih_d=sheta_deb.kolih();
int kolih_k=sheta_kre.kolih();

sum_pr.make_class(kolih_d*kolih_k);

int nom_sd;
int nom_sk;
float kolstr1=0.;
double suma=0.;
curp->poz_cursor(0);
while(curp->read_cursor(&row) != 0)
 {
  iceb_pbar(bar,kolstrp,++kolstr1);    
  if((nom_sd=sheta_deb.find(row[0])) < 0)
    continue;
  if((nom_sk=sheta_kre.find(row[1])) < 0)
    continue;
  suma=atof(row[2]);
  
  
  if(iceb_u_proverka(shet_k_minus,row[0],1,1) == 0)
   suma-=atof(row[3]);

  sum_pr.plus(suma,nom_sd*kolih_k+nom_sk);
  
 }
//Распечатываем дебет по вертикали
buh_rhw(0,&sheta_deb,&sheta_kre,&sum_pr,ff_dv); 

//Распечатываем кредит по вертикали
suma=buh_rhw(1,&sheta_deb,&sheta_kre,&sum_pr,ff_kv);

//printw("\n%s:%.2f\n\n",gettext("Итого"),suma);

return(suma);

}

/******************************************/
/******************************************/

gint buhsdkrw_r1(class buhsdkrw_r_data *data)
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

char zag_ras[512];
memset(zag_ras,'\0',sizeof(zag_ras));
iceb_poldan("Заголовок распечатки",zag_ras,"buhsdkr.alx",data->window);

int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

//Создаём список дебетовых счетов
sprintf(strsql,"select distinct sh from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by sh asc",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok sp_sheta_deb;

while(cur.read_cursor(&row) != 0)
  sp_sheta_deb.plus(row[0]);

//Создаём список кредитовых счетов
sprintf(strsql,"select distinct shk from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by shk asc",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok sp_sheta_kre;

while(cur.read_cursor(&row) != 0)
  sp_sheta_kre.plus(row[0]);








sprintf(strsql,"select sh,shk,deb,kre from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0",gn,mn,dn,gk,mk,dk);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstralx=0;
sprintf(strsql,"select str from Alx where fil='buhsdkr.alx' order by ns asc");
if((kolstralx=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstralx == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"buhsdkr.alx");
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



char imaf_dv[56];
sprintf(imaf_dv,"sdkr%d.lst",getpid());
FILE *ff_dv;

if((ff_dv=fopen(imaf_dv,"w")) == NULL) //Открываем файл с распечаткой
 {
  iceb_er_op_fil(imaf_dv,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(zag_ras,dn,mn,gn,dk,mk,gk,organ,ff_dv);

char imaf_kv[50];
sprintf(imaf_kv,"skdr%d.lst",getpid());
FILE *ff_kv;

if((ff_kv=fopen(imaf_kv,"w")) == NULL) //Открываем файл с распечаткой
 {
  iceb_er_op_fil(imaf_kv,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(zag_ras,dn,mn,gn,dk,mk,gk,organ,ff_kv);

char imaf_it[56];
sprintf(imaf_it,"skdri%d.lst",getpid());
FILE *ff_it;

if((ff_it=fopen(imaf_it,"w")) == NULL) //Открываем файл с распечаткой
 {
  iceb_er_op_fil(imaf_it,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_zagolov(gettext("Свод по вариантам расчёта"),dn,mn,gn,dk,mk,gk,organ,ff_it);

fprintf(ff_it,"\
------------------------------------------------\n");
fprintf(ff_it,"\
 Код |     Наименование             |  Сумма   |\n");
fprintf(ff_it,"\
------------------------------------------------\n");

class iceb_u_str shet_d;
class iceb_u_str shet_k;
char stroka1[1024];
char variant[512];
char naim_r[512];
class iceb_u_str shet_k_minus;

double itogo_var=0.;
double it=0.;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],stroka1,sizeof(stroka1),1,'|') != 0 )
    continue; 
  if(iceb_u_SRAV(stroka1,"Вариант расчёта",0) != 0 )
   continue;
  else
   {

    iceb_u_polen(row_alx[0],variant,sizeof(variant),2,'|');
    if(variant[0] == '\0')
     continue;
    
    
    iceb_printw(iceb_u_toutf(row_alx[0]),data->buffer,data->view);
    
    if(iceb_u_proverka(data->rk->variant_r.ravno(),variant,0,0) != 0)
     continue;

    
    cur_alx.read_cursor(&row_alx);
    iceb_printw(iceb_u_toutf(row_alx[0]),data->buffer,data->view);
    iceb_u_polen(row_alx[0],naim_r,sizeof(naim_r),2,'|');
    
    shet_d.new_plus("");
    shet_k.new_plus("");
    shet_k_minus.new_plus("");
                
    cur_alx.read_cursor(&row_alx);
    iceb_printw(iceb_u_toutf(row_alx[0]),data->buffer,data->view);
    iceb_u_polen(row_alx[0],stroka1,sizeof(stroka1),2,'|');
    iceb_fplus(stroka1,&shet_d,&cur_alx);    

    cur_alx.read_cursor(&row_alx);
    iceb_printw(iceb_u_toutf(row_alx[0]),data->buffer,data->view);
    iceb_u_polen(row_alx[0],stroka1,sizeof(stroka1),2,'|');
    iceb_fplus(stroka1,&shet_k,&cur_alx);
    
    cur_alx.read_cursor(&row_alx);
    
    if(iceb_u_SRAV("Отнять кредит для счетов",row_alx[0],1) == 0)
     {
      iceb_u_polen(row_alx[0],stroka1,sizeof(stroka1),2,'|');
      iceb_fplus(stroka1,&shet_k_minus,&cur_alx);
     }  
    iceb_printw(iceb_u_toutf(row_alx[0]),data->buffer,data->view);

    fprintf(ff_dv,"\n%s\n",naim_r);  
    fprintf(ff_kv,"\n%s\n",naim_r);  
    
    itogo_var=buhsdkr_r(dn,mn,gn,dk,mk,gk,&sp_sheta_deb,&sp_sheta_kre,shet_d.ravno(),shet_k.ravno(),shet_k_minus.ravno(),kolstr,&cur,ff_dv,ff_kv,data->bar);
    sprintf(strsql,"\n%s:%.2f\n\n",gettext("Итого"),itogo_var);

    fprintf(ff_it,"%*s %-*.*s %10.2f\n",
    iceb_u_kolbait(5,variant),variant,
    iceb_u_kolbait(30,naim_r),iceb_u_kolbait(30,naim_r),naim_r,
    itogo_var);
    
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    it+=itogo_var;    
      
    fprintf(ff_dv,"\n\n\n\n");  
    fprintf(ff_kv,"\n\n\n\n");  
     
   }
    
 }

fprintf(ff_it,"\
------------------------------------------------\n");
fprintf(ff_it,"%*s %10.2f\n",iceb_u_kolbait(36,gettext("Итого")),gettext("Итого"),it);

iceb_podpis(ff_dv,data->window);
fclose(ff_dv);

iceb_podpis(ff_kv,data->window);
fclose(ff_kv);




iceb_podpis(ff_it,data->window);
fclose(ff_it);

iceb_ustpeh(imaf_dv,0,data->window);
iceb_ustpeh(imaf_kv,0,data->window);
iceb_ustpeh(imaf_it,0,data->window);

data->rk->imaf.plus(imaf_dv);
data->rk->imaf.plus(imaf_kv);
data->rk->imaf.plus(imaf_it);

data->rk->naimf.plus(gettext("Отчёт с дебетами по вертикали"));
data->rk->naimf.plus(gettext("Отчёт с кредитами по вертикали"));
data->rk->naimf.plus(gettext("Отчёт с итогами по вариантам расчёта"));


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
