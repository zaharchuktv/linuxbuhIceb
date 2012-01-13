/*$Id: iceb_runsystem.c,v 1.8 2011-02-21 07:36:07 sasa Exp $*/
/*29.08.2006 	15.04.2004	Белых А.И.	iceb_runsystem.c
Запуск вспомогательных программ
*/
#include <stdlib.h>
#include <unistd.h>
#include "iceb_libbuh.h"

class runsystem_data
 {
  public:
  
  GtkWidget *window;
  GtkWidget *label;
  
  iceb_u_str komanda;
  int voz;  
  

 };
gint runsystem1(class runsystem_data *data);

extern char *name_system;

int iceb_runsystem(char *komanda,char *koment,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str soob;

runsystem_data data;

data.komanda.plus(komanda);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_default_size (GTK_WINDOW  (data.window),300,-1);


sprintf(strsql,"%s %s",name_system,iceb_u_toutf(koment));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

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

soob.new_plus(gettext("Ждите !!!"));
GtkWidget *label=gtk_label_new(soob.ravno_toutf());
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

soob.new_plus(koment);
soob.ps_plus(komanda);

label=gtk_label_new(soob.ravno_toutf());
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

gtk_widget_show_all(vbox);


gtk_idle_add((GtkFunction)runsystem1,&data);

gtk_widget_show_all(data.window);
//gtk_widget_realize(data.window);
//gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR));
gdk_window_set_cursor(data.window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);

}

gint runsystem1(class runsystem_data *data)
{
g_usleep(100000);
iceb_refresh();
data->voz=system(data->komanda.ravno());

gtk_widget_destroy(data->window);

return(FALSE);

}
