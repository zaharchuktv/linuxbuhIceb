/*$Id: taxi_saldo_r.c,v 1.12 2011-02-21 07:36:21 sasa Exp $*/
/*31.03.2005	31.03.2005	Белых А.И.	taxi_saldo_r.c
Перенос сальдо по клиентам
*/
#include <stdlib.h>
#include <unistd.h>
#include "taxi.h"

class taxi_saldo_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  taxi_saldo_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   taxi_saldo_r_key_press(GtkWidget *widget,GdkEventKey *event,class taxi_saldo_r_data *data);
gint taxi_saldo_r1(class taxi_saldo_r_data *data);
void  taxi_saldo_r_v_knopka(GtkWidget *widget,class taxi_saldo_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;
extern char *imabaz;
extern char *organ;
extern short start_god_taxi;

int taxi_saldo_r(GtkWidget *wpredok)
{
char strsql[500];
sprintf(strsql,"Перенести сальдо с %dг. на %dг. ? Вы уверены ?",start_god_taxi,start_god_taxi+1);
if(iceb_menu_danet(strsql,2,wpredok) == 2)
 return(1);


iceb_u_str repl;
class taxi_saldo_r_data data;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Перенос сальдо"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(taxi_saldo_r_key_press),&data);

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

repl.plus(gettext("Перенос сальдо"));
sprintf(strsql,"%s:%s %s",gettext("База данных"),imabaz,organ);
repl.ps_plus(strsql);
repl.ps_plus(gettext("Стартовый год"));
repl.plus(":");
repl.plus(start_god_taxi);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(taxi_saldo_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)taxi_saldo_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  taxi_saldo_r_v_knopka(GtkWidget *widget,class taxi_saldo_r_data *data)
{
printf("taxi_saldo_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   taxi_saldo_r_key_press(GtkWidget *widget,GdkEventKey *event,class taxi_saldo_r_data *data)
{
 printf("taxi_saldo_r_key_press\n");
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

gint taxi_saldo_r1(class taxi_saldo_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1000];
iceb_u_str repl;
iceb_clock sss(data->window);




SQL_str row;
SQLCURSOR cur;


sprintf(strsql,"select kod,fio from Taxiklient");
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
float kolstr1=0.;
double suma=0.;
time_t vrem;
time(&vrem);

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  suma=taxi_saldo(row[0],start_god_taxi+1);
  sprintf(strsql,"%-6s %-30.30s %8.2f\n",row[0],row[1],suma);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  if(suma == 0.)
    continue;

  sprintf(strsql,"insert into Taxiklsal values(%d,'%s',%.2f,%d,%ld)",
  0,row[0],suma,iceb_getuid(data->window),vrem);
  iceb_sql_zapis(strsql,1,0,data->window);
 }

sprintf(strsql,"delete from Taxiklsal where god=%d",start_god_taxi+1);
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"update Taxiklsal set god=%d where god=0",start_god_taxi+1);
iceb_sql_zapis(strsql,1,0,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;


//iceb_u_str imafn;
//iceb_imafn("taxinast.alx",&imafn);

repl.new_plus(gettext("Теперь нужно установить новый стартовый год в файле настройки"));
repl.ps_plus("taxinast.alx");
iceb_menu_soob(&repl,data->window);
return(FALSE);

}






