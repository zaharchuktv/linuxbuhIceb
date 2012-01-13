/*$Id: iceb_gdite.c,v 1.13 2011-02-21 07:36:07 sasa Exp $*/
/*12.10.2010	21.08.2006	Белых А.И.	iceb_gdite.c
Окно с текстом Ждите !!!
*/

#include  "iceb_libbuh.h"


extern char *name_system;

void iceb_gdite(class iceb_gdite_data *data,int metka_bar,GtkWidget *wpredok)
{

data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
gtk_window_set_resizable(GTK_WINDOW(data->window),FALSE); /*запрет на изменение размеров окна*/
gtk_window_set_deletable(GTK_WINDOW(data->window),FALSE); /*Выключить кнопку close в рамке окна*/

if(wpredok != NULL)
 {
//  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data->window),GTK_WINDOW(wpredok));

 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data->window),TRUE);
/*Окно поверх всех окон*/
gtk_window_set_keep_above(GTK_WINDOW(data->window),TRUE);

char bros[312];
sprintf(bros,"%s %s",name_system,gettext("Ждите !"));

gtk_window_set_title (GTK_WINDOW (data->window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data->window), 5);


GtkWidget *vbox=gtk_vbox_new(FALSE, 2);
gtk_container_add (GTK_CONTAINER (data->window), vbox);


GtkWidget *label=gtk_label_new(gettext("Ждите !"));
GdkColor color;
gdk_color_parse("red",&color);
gtk_widget_modify_fg(label,GTK_STATE_NORMAL,&color);

gtk_widget_set_size_request(GTK_WIDGET(label),300,-1);
gtk_box_pack_start(GTK_BOX (vbox), label, TRUE, TRUE, 0);


if(metka_bar == 0)
 {
  data->bar=gtk_progress_bar_new();

  gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data->bar),GTK_PROGRESS_CONTINUOUS);

  gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data->bar),GTK_PROGRESS_LEFT_TO_RIGHT);

  gtk_box_pack_start (GTK_BOX (vbox), data->bar, FALSE, FALSE, 2);
 }
//gtk_window_set_default_size (GTK_WINDOW  (data->window),300,-1);

gtk_window_set_position( GTK_WINDOW(data->window),ICEB_POS_CENTER);
gtk_widget_show_all(data->window);

gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

iceb_refresh();

}
