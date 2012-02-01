/*$Id: iceb_file_selection.c,v 1.8 2011-02-21 07:36:06 sasa Exp $*/
/*23.03.2010	15.11.2004	Белых А.И.	iceb_file_selection.c
Просмотр текущего каталога и выбор файлов
*/

#include "iceb_libbuh.h"
class  file_selection_data
 {
  public:
  GtkWidget *filesel;
  iceb_u_str *imaf_v;
  int voz;
  
 };
 
void file_selection_vibor(GtkWidget *widget,class file_selection_data *data);
void file_selection_vihod(GtkWidget *widget,class file_selection_data *data)
 {
  gtk_widget_destroy(data->filesel);  
 }


int  iceb_file_selection(iceb_u_str *imaf_v,GtkWidget *wpredok)
{
class  file_selection_data data;
data.imaf_v=imaf_v;

GtkWidget *vibor_knop;
GtkWidget *vihod_knop;

data.filesel= gtk_file_selection_new(gettext("Поиск файла"));
gtk_window_set_position( GTK_WINDOW(data.filesel),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.filesel),TRUE);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.filesel),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.filesel),TRUE);
 }

vibor_knop=GTK_FILE_SELECTION(data.filesel)->ok_button;
vihod_knop=GTK_FILE_SELECTION(data.filesel)->cancel_button;

gtk_signal_connect(GTK_OBJECT(data.filesel),"destroy",G_CALLBACK(gtk_main_quit),NULL);
gtk_signal_connect(GTK_OBJECT(vibor_knop),"clicked",GTK_SIGNAL_FUNC(file_selection_vibor),&data);
gtk_signal_connect(GTK_OBJECT(vihod_knop),"clicked",GTK_SIGNAL_FUNC(file_selection_vihod),&data);

//gtk_file_selection_set_filename(GTK_FILE_SELECTION(data.filesel),"kkk.txt");
gtk_widget_show(data.filesel);
gtk_main();

if(wpredok != NULL)
 gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(0);
}
/**************************/
/*Чтение выбранного файла*/
/**************************/
void file_selection_vibor(GtkWidget *widget,class file_selection_data *data)
{
//g_print("%s\n",gtk_file_selection_get_filename(GTK_FILE_SELECTION(data->filesel)));

const char *file_v;
file_v=gtk_file_selection_get_filename(GTK_FILE_SELECTION(data->filesel));

data->imaf_v->new_plus(iceb_u_fromutf(file_v));


gtk_widget_destroy(data->filesel);  

}
