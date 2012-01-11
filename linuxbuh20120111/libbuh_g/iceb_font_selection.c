/*$Id: iceb_font_selection.c,v 1.4 2011-02-21 07:36:07 sasa Exp $*/
/*24.05.2010	24.05.2010	Белых А.И.	iceb_font_selection.c
Фыбор фонтов
*/
#include <pwd.h>
#include <errno.h>
#include <unistd.h>
#include "iceb_libbuh.h"


void  iceb_font_selection_out(GtkWidget *widget,GtkWidget *hrift)
{
gtk_widget_destroy(hrift);
}
void  iceb_font_selection_ok(GtkWidget *widget,GtkWidget *hrift)
{

gchar *name_font=gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(hrift));
//printf("Имя шрифта=%s\n",name_font);
//if(gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(hrift),name_font) == TRUE)
// printf("Шрифт найден\n");

struct passwd *rek_login;
rek_login=getpwuid(getuid());

class iceb_u_str imaf_nast;
imaf_nast.plus(rek_login->pw_dir);
imaf_nast.plus(G_DIR_SEPARATOR_S,".gtkrc-2.0");

FILE *ff;

if((ff = fopen(imaf_nast.ravno(),"w")) == NULL)
 {
  iceb_u_str repl;
  char bros[312];
  
  sprintf(bros,"%s:%d !",gettext("Ошибка открытия файла"),errno);
  repl.plus_ps(bros);
  

  repl.plus(strerror(errno));

  iceb_menu_soob(&repl,NULL);
  return;
 }

/*************
fprintf(ff,"\
style \"gtk-default-koi8\"\n\
{\n\
 font_name = \"%s\"\n\
}\n\
class \"*\" style \"gtk-default-koi8\"\n",name_font);
*****************/
fprintf(ff,"\
style \"gtk-default\"\n\
{\n\
 font_name = \"%s\"\n\
}\n\
class \"*\" style \"gtk-default\"\n",name_font);

fclose(ff);

gtk_widget_destroy(hrift);
}

/*******************************/

void iceb_font_selection(GtkWidget *wpredok)
{

GtkWidget *hrift=gtk_font_selection_dialog_new(iceb_u_toutf(gettext("Выбор шрифта")));

gtk_signal_connect(GTK_OBJECT(hrift),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(hrift),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(hrift),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(hrift),TRUE);
 }

//g_return_if_fail(GTK_IS_FONT_SELECTION_DIALOG(hrift));

gtk_signal_connect(GTK_OBJECT (GTK_FONT_SELECTION_DIALOG(hrift)->ok_button), "clicked",\
GTK_SIGNAL_FUNC(iceb_font_selection_ok),hrift);

gtk_signal_connect(GTK_OBJECT (GTK_FONT_SELECTION_DIALOG(hrift)->cancel_button), "clicked",\
GTK_SIGNAL_FUNC(iceb_font_selection_out),hrift);

gtk_widget_show(hrift);
//gtk_widget_show(GTK_FONT_SELECTION_DIALOG(hrift)->apply_button);
gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

gtk_rc_reparse_all (); /*Читаем настройки всех файлов*/

}
