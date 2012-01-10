/*$Id: upl_plc_r.c,v 1.5 2011-02-21 07:35:58 sasa Exp $*/
/*30.03.2008	30.03.2008	Белых А.И.	upl_plc_r.c
Проверка логической целосности для подсистемы "Учёт путевых листов"
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class upl_plc_r_data
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

  upl_plc_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_plc_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_plc_r_data *data);
gint upl_plc_r1(class upl_plc_r_data *data);
void  upl_plc_r_v_knopka(GtkWidget *widget,class upl_plc_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int      kol_strok_na_liste;

int upl_plc_r(GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class upl_plc_r_data data;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Проверка логической целосности базы данных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(upl_plc_r_key_press),&data);

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

repl.plus(gettext("Проверка логической целосности базы данных"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(upl_plc_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)upl_plc_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_plc_r_v_knopka(GtkWidget *widget,class upl_plc_r_data *data)
{
printf("upl_plc_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   upl_plc_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_plc_r_data *data)
{
 printf("upl_plc_r_key_press\n");
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

gint upl_plc_r1(class upl_plc_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;

FILE *ff;
char imaf[56];

sprintf(imaf,"upl_plc%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"%s Upldok1\n",gettext("Проверяем записи в таблице"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff,"%s\n",gettext("Проверка логической целосности базы данных"));
fprintf(ff,"%s Upldok1\n",gettext("Проверяем записи в таблице"));

sprintf(strsql,"select datd,kp,nomd from Upldok1");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int koloh=0;
float kolstr1=0;
SQL_str row1;
class SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select kp from Upldok where datd='%s' and kp=%s and nomd='%s'",
  row[0],row[1],row[2]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"%s %-3s %s\n",row[0],row[1],row[2]);

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%s %-3s %s\n",row[0],row[1],row[2]);
    koloh++;
    sprintf(strsql,"delete from Upldok1 where datd='%s' and kp=%s and nomd='%s'",
    row[0],row[1],row[2]);
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  
 }

sprintf(strsql,"%s Upldok2\n",gettext("Проверяем записи в таблице"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff,"\n%s Upldok2\n",gettext("Проверяем записи в таблице"));

sprintf(strsql,"select datd,kp,nomd from Upldok2");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

kolstr1=0;

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select kp from Upldok where datd='%s' and kp=%s and nomd='%s'",
  row[0],row[1],row[2]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    sprintf(strsql,"%s %-3s %s\n",row[0],row[1],row[2]);

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    fprintf(ff,"%s %-3s %s\n",row[0],row[1],row[2]);
    koloh++;

    sprintf(strsql,"delete from Upldok2 where datd='%s' and kp=%s and nomd='%s'",
    row[0],row[1],row[2]);
    iceb_sql_zapis(strsql,1,0,data->window);
   }
  
 }

fclose(ff);

if(koloh == 0)
 {
  
  iceb_menu_soob(gettext("Порядок, ошибок не обнаружено!"),data->window);
  unlink(imaf);
 }
else
 {
  sprintf(strsql,"%s: %d",gettext("Количество исправленых ошибок"),koloh);
  repl.new_plus(strsql);
  
  sprintf(strsql,"%s %s",gettext("Распечатку выгружено в файл"),imaf);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,data->window);
  
  class iceb_u_spisok imafil;
  class iceb_u_spisok naimf;
  imafil.plus(imaf);
  naimf.plus(gettext("Протокол ошибок"));
  iceb_rabfil(&imafil,&naimf,"",0,data->window);
 }




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}









