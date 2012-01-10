/*$Id: clearzarw_r.c,v 1.6 2011-02-21 07:35:51 sasa Exp $*/
/*20.07.2009	06.11.2008	Белых А.И.	clearzarw_r.c
Удаление записей
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class clearzarw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  short mu;
  short gu;
  int kom;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  clearzarw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   clearzarw_r_key_press(GtkWidget *widget,GdkEventKey *event,class clearzarw_r_data *data);
gint clearzarw_r1(class clearzarw_r_data *data);
void  clearzarw_r_v_knopka(GtkWidget *widget,class clearzarw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

int clearzarw_r(int kom,short mu,short gu,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class clearzarw_r_data data;

data.mu=mu;
data.gu=gu;
data.kom=kom;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Удаление записей"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(clearzarw_r_key_press),&data);

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

repl.plus(gettext("Удаление записей"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(clearzarw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)clearzarw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  clearzarw_r_v_knopka(GtkWidget *widget,class clearzarw_r_data *data)
{
printf("clearzarw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   clearzarw_r_key_press(GtkWidget *widget,GdkEventKey *event,class clearzarw_r_data *data)
{
 printf("clearzarw_r_key_press\n");
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

gint clearzarw_r1(class clearzarw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;



 
strcpy(strsql,"select VERSION()");
iceb_sql_readkey(strsql,&row,&cur,NULL);
int metkazap=0;
if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  
printf("VERSION:%s\n",row[0]);

if(data->kom == 0)
  sprintf(strsql,"%s %d%s\n",
  gettext("Удаление записей за"),
  data->gu,gettext("г."));
if(data->kom == 1)
  sprintf(strsql,"%s %d.%d%s\n",
  gettext("Удаление записей за"),
  data->mu,data->gu,gettext("г."));
if(data->kom == 2)
  sprintf(strsql,"%s.\n",
  gettext("Удаление записей с нулевым значением"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s\n",gettext("Удаляем записи начислений и удержаний"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
if(data->kom == 0)
 sprintf(strsql,"delete from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31'",data->gu,data->mu,data->gu,data->mu);
if(data->kom == 1)
 sprintf(strsql,"delete from Zarp where datz >= '%04d-01-01' and \
datz <= '%04d-12-31'",data->gu,data->gu);

if(data->kom == 2)
 sprintf(strsql,"delete from Zarp where datz < '%04d-%d-01' and suma=0.",data->gu,data->mu);

iceb_sql_zapis(strsql,1,0,data->window);

int kolud=sql_rows(&bd);
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Zarp");
  if(metkazap == 1)
   {
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  else
    iceb_sql_readkey(strsql,data->window);
 }

if(data->kom == 2)
 {
  sprintf(strsql,"%s: %d",gettext("Количество удаленных записей"),kolud);
  iceb_menu_soob(strsql,data->window);
  goto kon;
 } 

sprintf(strsql,"%s\n",gettext("Удаляем записи в вспомогательной таблице"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(data->kom == 0)
 sprintf(strsql,"delete from Zarn where god = %d and mes=%d",
 data->gu,data->mu);
if(data->kom == 1)
 sprintf(strsql,"delete from Zarn where god=%d",
 data->gu);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);

sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Zarn");
  if(metkazap == 1)
   {
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  else
    iceb_sql_readkey(strsql,data->window);
 }

sprintf(strsql,"%s\n",gettext("Удаляем записи начислений на фонд оплаты труда"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(data->kom == 0)
 sprintf(strsql,"delete from Zarsocz where datz >= '%d-%d-01' and datz <= '%d-%d-31'",
 data->gu,data->mu,data->gu,data->mu);

if(data->kom == 1)
 sprintf(strsql,"delete from Zarsocz where datz >= '%d-01-01' and datz <= '%d-12-31'",
 data->gu,data->gu);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Zarsocz");
  if(metkazap == 1)
   {
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  else
    iceb_sql_readkey(strsql,data->window);
 }

sprintf(strsql,"%s\n",gettext("Удаляем записи отработанного времени"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(data->kom == 0)
 sprintf(strsql,"delete from Ztab where god=%d and mes=%d",
 data->gu,data->mu);
if(data->kom == 1)
 sprintf(strsql,"delete from Ztab where god=%d",
 data->gu);

iceb_sql_zapis(strsql,1,0,data->window);

kolud=sql_rows(&bd);
sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if(kolud > 0)
 { 
  sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table Ztab");
  if(metkazap == 1)
   {
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  else
    iceb_sql_readkey(strsql,data->window);
 }
if(data->kom == 1)
 {
  sprintf(strsql,"%s\n",gettext("Удаляем записи сальдо"));

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  sprintf(strsql,"delete from Zsal where god=%d",data->gu);

  iceb_sql_zapis(strsql,1,0,data->window);

  kolud=sql_rows(&bd);
  sprintf(strsql,"%s: %d\n",gettext("Количество удаленных записей"),kolud);

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  if(kolud > 0)
   { 
    sprintf(strsql,"%s\n",gettext("Оптимизируем таблицу"));
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    memset(strsql,'\0',sizeof(strsql));
    sprintf(strsql,"optimize table Zsal");
    if(metkazap == 1)
     {
      iceb_sql_zapis(strsql,1,0,data->window);
     }
    else
      iceb_sql_readkey(strsql,data->window);
   }
 }




kon:;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}









