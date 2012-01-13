/*$Id: iceb_optimbazw_r.c,v 1.4 2011-02-21 07:36:07 sasa Exp $*/
/*16.04.2010	09.05.2005	Белых А.И. 	iceb_optimbazw_r.c
Оптимизазия базы данных
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "iceb_libbuh.h"

class iceb_optimbazw_r_data
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
  class iceb_u_str imabaz;
  iceb_optimbazw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   iceb_optimbazw_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_optimbazw_r_data *data);
gint iceb_optimbazw_r1(class iceb_optimbazw_r_data *data);
void  iceb_optimbazw_r_v_knopka(GtkWidget *widget,class iceb_optimbazw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern iceb_u_str shrift_ravnohir;
extern double	nds1;
extern int kol_strok_na_liste;

int iceb_optimbazw_r(const char *imabaz,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class iceb_optimbazw_r_data data;
data.imabaz.new_plus(imabaz); 

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Оптимизация базы данных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_optimbazw_r_key_press),&data);

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

gtk_box_pack_start(GTK_BOX(vbox),data.label,TRUE,TRUE,0);

repl.plus(gettext("Оптимизация базы данных"));
sprintf(strsql,"%s:%s\n",gettext("База данных"),data.imabaz.ravno());
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(iceb_optimbazw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)iceb_optimbazw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_optimbazw_r_v_knopka(GtkWidget *widget,class iceb_optimbazw_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_optimbazw_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_optimbazw_r_data *data)
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

/******************************************/
/******************************************/

gint iceb_optimbazw_r1(class iceb_optimbazw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

SQL_str row;
SQLCURSOR cur;

strcpy(strsql,"select VERSION()");
sql_readkey(&bd,strsql,&row,&cur);

int metkazap=0;
if(iceb_u_SRAV(row[0],"3.22",1) == 0)
  metkazap=1;  

sprintf(strsql,"VERSION:%s\n",row[0]);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


sprintf(strsql,"SHOW TABLES FROM %s",data->imabaz.ravno());
float kolstr1=0;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  goto vper;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  goto vper;
 }

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);
//  strzag(LINES-1,0,kolstr,++kolstr1);
  sprintf(strsql,"%s %s !\n",gettext("Оптимизация таблицы"),row[0]);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"optimize table %s",row[0]);
  if(metkazap == 1)
   {
    if(sql_zap(&bd,strsql) < 0)
      iceb_msql_error(&bd,gettext("Ошибка оптимизации таблицы !"),strsql,data->window);
   }
  else
   {
    iceb_sql_readkey(strsql,data->window);
   }

  sprintf(strsql,"%s.\n",gettext("Оптимизацию таблицы закончено"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

 }

sprintf(strsql,"\n%s:%d\n",gettext("Количество таблиц"),kolstr);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"\n%s.\n",gettext("Оптимизация базы закончена"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


vper:;
memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,gettext("Расчет закончен"));
//gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));
gtk_label_set_text(GTK_LABEL(data->label),strsql);

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);


data->voz=0;

return(FALSE);

}
