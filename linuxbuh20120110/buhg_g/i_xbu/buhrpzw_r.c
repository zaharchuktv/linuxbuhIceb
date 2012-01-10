/*$Id: buhrpzw_r.c,v 1.12 2011-02-21 07:35:51 sasa Exp $*/
/*10.11.2009	01.03.2007	Белых А.И.	buhrpzw_r.c
Расчёт видов затрат по элементам
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
#include "buhrpzw.h"

class buhrpzw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class buhrpzw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  buhrpzw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhrpzw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhrpzw_r_data *data);
gint buhrpzw_r1(class buhrpzw_r_data *data);
void  buhrpzw_r_v_knopka(GtkWidget *widget,class buhrpzw_r_data *data);

double buhrpz_r(short dn,short mn,short gn,short dk,short mk,short gk,class iceb_u_spisok *sp_sheta_deb,class iceb_u_spisok *sp_sheta_kre,
char *shet_d,char *shet_k,char *shet_k_minus,int kolstrp,class SQLCURSOR *curp,FILE *ff_dv,FILE *ff_kv,GtkWidget *bar);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

int buhrpzw_r(class buhrpzw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class buhrpzw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт видов затрат по элементам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(buhrpzw_r_key_press),&data);

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

repl.plus(gettext("Расчёт видов затрат по элементам"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(buhrpzw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)buhrpzw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhrpzw_r_v_knopka(GtkWidget *widget,class buhrpzw_r_data *data)
{
//printf("buhrpzw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   buhrpzw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhrpzw_r_data *data)
{
// printf("buhrpzw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}

/********************************/
/*подчёркивание*/
/*****************************/
void buhrpz_lin(int kolih_podr,FILE *ff)
{
fprintf(ff,"---------------------");
for(int ii=0; ii <= kolih_podr; ii++)
  fprintf(ff,"-----------");
fprintf(ff,"\n");

}

/******************************************/
/******************************************/

gint buhrpzw_r1(class buhrpzw_r_data *data)
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

sprintf(strsql,"select str from Alx where fil='buhrpz.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"buhrpz.alx");
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



class iceb_u_spisok sp_vid_zat;
class iceb_u_spisok sp_podr;
int metka_vid_zat=0;
char naim_ras[100];
memset(naim_ras,'\0',sizeof(naim_ras));
char naim_it_str[100];
memset(naim_it_str,'\0',sizeof(naim_it_str));

//Читаем настройки
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],strsql,sizeof(strsql),1,'|') != 0)
   continue;

  if(iceb_u_SRAV("Наименование расчёта",strsql,0) == 0)
   {
    iceb_u_polen(row_alx[0],naim_ras,sizeof(naim_ras),2,'|');
    continue;
   }
  if(iceb_u_SRAV("Наименование итоговой строки",strsql,0) == 0)
   {
    iceb_u_polen(row_alx[0],naim_it_str,sizeof(naim_it_str),2,'|');
    continue;
   }
  if(iceb_u_SRAV("Наименование вида затрат",strsql,0) == 0)
   {
    iceb_u_polen(row_alx[0],strsql,sizeof(strsql),2,'|');
    if(strsql[0] == '\0')
     {
      metka_vid_zat=0;
      continue;
     }
    metka_vid_zat=1;

    sp_vid_zat.plus(strsql);
    continue;
   }

  if(iceb_u_SRAV("Наименование элемента",strsql,0) == 0)
   {
    if(metka_vid_zat == 0)
     continue;

    iceb_u_polen(row_alx[0],strsql,sizeof(strsql),2,'|');

    if(strsql[0] == '\0')
     continue;

    if(sp_podr.find(strsql) < 0)
      sp_podr.plus(strsql);
    continue;
   }


 }

if(naim_ras[0] == '\0')
 strncpy(naim_ras,gettext("Расчёт видов затрат по элементам"),sizeof(naim_ras)-1);
if(naim_it_str[0] == '\0')
 strncpy(naim_it_str,gettext("Итого"),sizeof(naim_it_str)-1);

int kol_naim_podr=sp_podr.kolih();

class iceb_u_spisok sp_deb[sp_vid_zat.kolih()][sp_podr.kolih()];
class iceb_u_spisok sp_kre[sp_vid_zat.kolih()][kol_naim_podr];
int nomer_pod=0;
int nomer_vid_zat=0;
cur_alx.poz_cursor(0);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],strsql,sizeof(strsql),1,'|') != 0)
   continue;

  if(iceb_u_SRAV("Наименование вида затрат",strsql,0) == 0)
   {
    iceb_u_polen(row_alx[0],strsql,sizeof(strsql),2,'|');
    if(strsql[0] == '\0')
     {
      metka_vid_zat=0;
      continue;
     }
    nomer_vid_zat=sp_vid_zat.find(strsql);
    metka_vid_zat=1;
    continue;
   }


  if(iceb_u_SRAV("Наименование элемента",strsql,0) == 0)
   {
    if(metka_vid_zat == 0)
     continue;

    iceb_u_polen(row_alx[0],strsql,sizeof(strsql),2,'|');

    if(strsql[0] == '\0')
     continue;

    nomer_pod=sp_podr.find(strsql);

    cur_alx.read_cursor(&row_alx);
    iceb_u_polen(row_alx[0],strsql,sizeof(strsql),2,'|');
    sp_deb[nomer_vid_zat][nomer_pod].plus(strsql);

    cur_alx.read_cursor(&row_alx);
    iceb_u_polen(row_alx[0],strsql,sizeof(strsql),2,'|');
    sp_kre[nomer_vid_zat][nomer_pod].plus(strsql);

    continue;
   }


 }

int kolih_vid_zat=sp_vid_zat.kolih();
int kolih_podr=sp_podr.kolih();
if(kolih_vid_zat == 0)
 {
  sprintf(strsql,"%s %d !",gettext("Количество видов затрат"),kolih_vid_zat);
  iceb_menu_soob(strsql,data->window);
  
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolih_podr == 0)
 {
  sprintf(strsql,"%s %d !",gettext("Количество подразделений"),kolih_podr);
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select sh,shk,deb from Prov where datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by sh asc",gn,mn,dn,gk,mk,dk);

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

double suma=0.;
double mas_sum[kolih_vid_zat][kolih_podr];

memset(mas_sum,'\0',sizeof(mas_sum));
float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);    
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  for(int vz=0; vz < kolih_vid_zat; vz++)
   for(int pd=0; pd < kolih_podr; pd++)
    {
     if(sp_deb[vz][pd].kolih() == 0)
      continue;

     if(iceb_u_proverka(sp_deb[vz][pd].ravno(0),row[0],1,0) != 0)
      continue;     

     if(sp_kre[vz][pd].kolih() == 0)
      continue;

     if(iceb_u_proverka(sp_kre[vz][pd].ravno(0),row[1],1,0) != 0)
      continue;     

     suma=atof(row[2]);
     mas_sum[vz][pd]+=suma;

    }

 }

char imaf[56];

sprintf(imaf,"brpz%d.lst",getpid());
FILE *ff;

if((ff=fopen(imaf,"w")) == NULL) 
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(naim_ras,dn,mn,gn,dk,mk,gk,organ,ff);

//линия
buhrpz_lin(kolih_podr,ff);
fprintf(ff,"%-20.20s|","");
for(int pd=0; pd < kolih_podr; pd++)
 fprintf(ff,"%-*.*s|",iceb_u_kolbait(10,sp_podr.ravno(pd)),iceb_u_kolbait(10,sp_podr.ravno(pd)),sp_podr.ravno(pd));
fprintf(ff,"%-*.*s|\n",iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

buhrpz_lin(kolih_podr,ff);


double itogo_gor=0.;
for(int vz=0; vz < kolih_vid_zat; vz++)
 {
  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,sp_vid_zat.ravno(vz)),iceb_u_kolbait(20,sp_vid_zat.ravno(vz)),sp_vid_zat.ravno(vz));
  itogo_gor=0.;
  for(int pd=0; pd < kolih_podr; pd++)
   {
    fprintf(ff,"%10.2f|",mas_sum[vz][pd]);
    itogo_gor+=mas_sum[vz][pd];
   }
  fprintf(ff,"%10.2f|\n",itogo_gor);
 }

buhrpz_lin(kolih_podr,ff);
//печатаем итоговую стороку
fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,naim_it_str),iceb_u_kolbait(20,naim_it_str),naim_it_str);
double itogo_kol=0.;
itogo_gor=0.;
for(int pd=0; pd < kolih_podr; pd++)
 {
  itogo_kol=0.;
  for(int vz=0; vz < kolih_vid_zat; vz++)
   itogo_kol+=mas_sum[vz][pd];
  fprintf(ff,"%10.2f|",itogo_kol);
  itogo_gor+=itogo_kol;
 }
fprintf(ff,"%10.2f|\n",itogo_gor);

iceb_podpis(ff,data->window);

fclose(ff);

iceb_ustpeh(imaf,0,data->window);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт видов затрат по элементам"));

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
