/*$Id: spdnr_r.c,v 1.5 2011-02-21 07:36:21 sasa Exp $*/
/*25.12.2007	25.12.2007	Белых А.И.	spdnr_r.c
Расчёт формы 1ДФ
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "i_rest.h"


class spdnr_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class spdnr_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  spdnr_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   spdnr_r_key_press(GtkWidget *widget,GdkEventKey *event,class spdnr_r_data *data);
gint spdnr_r1(class spdnr_r_data *data);
void  spdnr_r_v_knopka(GtkWidget *widget,class spdnr_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;


int spdnr_r(GtkWidget *wpredok)
{
char strsql[500];
iceb_u_str repl;
class spdnr_r_data data;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Получение списка рождённых в текущий и следующий день"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(spdnr_r_key_press),&data);

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

repl.plus(gettext("Получение списка рождённых в текущий и следующий день"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(spdnr_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)spdnr_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  spdnr_r_v_knopka(GtkWidget *widget,class spdnr_r_data *data)
{
printf("spdnr_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spdnr_r_key_press(GtkWidget *widget,GdkEventKey *event,class spdnr_r_data *data)
{
 printf("spdnr_r_key_press\n");
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

gint spdnr_r1(class spdnr_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1000];
class iceb_clock sss(data->window);



short dt,mt,gt;
short dt1,mt1,gt1;
iceb_u_poltekdat(&dt,&mt,&gt);
dt1=dt;
mt1=mt;
gt1=gt;
iceb_u_dpm(&dt1,&mt1,&gt1,1);

sprintf(strsql,"%02d.%02d.%d -> %02d.%02d.%d\n",dt,mt,gt,dt1,mt1,gt1);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,iceb_u_toutf("Клиенты:\n"));
iceb_printw(strsql,data->buffer,data->view);
int kolstr=0;

class SQLCURSOR cur;
SQL_str row;
sprintf(strsql,"select kod,fio,denrog from Taxiklient where denrog >= '%04d-%02d-%02d' and denrog <= '%04d-%02d-%02d' \
order by denrog asc",gt,mt,dt,gt1,mt1,dt1);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  data->voz=1;
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  sprintf(strsql,"%s %-10s %s\n",iceb_u_sqldata(row[2]),row[0],iceb_u_toutf(row[1]));
  iceb_printw(strsql,data->buffer,data->view);
 }

sprintf(strsql,iceb_u_toutf("\nСлужащие:\n"));
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"select tabn,denrog from Kartb where denrog >= '%04d-%02d-%02d' and denrog <= '%04d-%02d-%02d' \
order by denrog asc",gt,mt,dt,gt1,mt1,dt1);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  data->voz=1;
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  sprintf(strsql,"%s %-10s %s\n",iceb_u_sqldata(row[2]),row[0],iceb_u_toutf(row[1]));
  iceb_printw(strsql,data->buffer,data->view);
 }
 
data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
