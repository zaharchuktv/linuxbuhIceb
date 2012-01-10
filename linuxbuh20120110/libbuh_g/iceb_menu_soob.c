/*$Id: iceb_menu_soob.c,v 1.15 2011-02-21 07:36:07 sasa Exp $*/
/*11.11.2008	19.08.2003	Белых А.И.	iceb_menu_soob.c
Прогамма вывода сообщения на экран
*/
#include <stdio.h>
//#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "iceb_libbuh.h"

void  iceb_menu_soob1(const char *soob,GtkWidget *wpredok);
void  iceb_menu_soob2(iceb_u_spisok *soob,GtkWidget *wpredok);

extern char *name_system;


void  iceb_menu_soob(iceb_u_spisok *soob,GtkWidget *wpredok)
 {
  iceb_menu_soob2(soob,wpredok);
 }

void  iceb_menu_soob(iceb_u_str *soob,GtkWidget *wpredok)
 {
  iceb_menu_soob1(soob->ravno(),wpredok);
 }

void  iceb_menu_soob(const char *soob,GtkWidget *wpredok)
 {
  iceb_menu_soob1(soob,wpredok);
 }

/***************************************/

void  iceb_menu_soob2(iceb_u_spisok *soob,GtkWidget *wpredok)
{
char bros[512];
GtkWidget *dialog;
GtkWidget *label;
GtkWidget *ok_button;
GtkWidget *vbox;
int       i=0;
int kolstr=soob->kolih();

dialog = gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_resizable(GTK_WINDOW(dialog),FALSE); /*запрет на изменение размеров окна*/
gtk_container_set_border_width(GTK_CONTAINER(dialog),10);

gtk_window_set_position( GTK_WINDOW( dialog ), GTK_WIN_POS_CENTER );                      

sprintf(bros,"%s %s!",name_system,gettext("Внимание"));

gtk_window_set_title (GTK_WINDOW (dialog),bros);

ok_button = gtk_button_new_with_label(iceb_u_toutf(gettext("Выход")));
	
gtk_signal_connect(GTK_OBJECT(dialog),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(dialog),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(dialog),"key_press_event",GTK_SIGNAL_FUNC(iceb_vihod),&i);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(dialog),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog),TRUE);
 }

vbox=gtk_vbox_new(FALSE,0);
gtk_container_add(GTK_CONTAINER(dialog),vbox);
for(int i=0; i < kolstr; i++)
 {
  label = gtk_label_new (soob->ravno_toutf(i));
  gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);
 }

gtk_box_pack_start((GtkBox*)vbox,ok_button,FALSE,FALSE,0);

gtk_widget_grab_focus(ok_button);
gtk_signal_connect_object( GTK_OBJECT( ok_button ), "clicked",GTK_SIGNAL_FUNC( gtk_widget_destroy ),
                            GTK_OBJECT(dialog) );

gtk_window_set_modal( GTK_WINDOW( dialog ) ,TRUE ); 

gtk_widget_show_all( dialog );

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}
/***************************************/

void  iceb_menu_soob1(const char *soob,GtkWidget *wpredok)
{
char bros[512];
GtkWidget *dialog;
GtkWidget *label;
GtkWidget *ok_button;
GtkWidget *vbox;
int       i=0;
class iceb_u_str  utf;

//Для символов в UTF кодировке нужно больше места
utf.plus(iceb_u_toutf(soob));

dialog = gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_resizable(GTK_WINDOW(dialog),FALSE); /*запрет на изменение размеров окна*/
//dialog = gtk_window_new( GTK_WINDOW_POPUP );
// dialog = gtk_dialog_new();
gtk_container_set_border_width(GTK_CONTAINER(dialog),10);

gtk_window_set_position( GTK_WINDOW( dialog ), GTK_WIN_POS_CENTER );                      

sprintf(bros,"%s %s!",name_system,gettext("Внимание"));

gtk_window_set_title (GTK_WINDOW (dialog),bros);

label = gtk_label_new (utf.ravno());
ok_button = gtk_button_new_with_label(iceb_u_toutf(gettext("Выход")));
	
gtk_signal_connect(GTK_OBJECT(dialog),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
//if(level == 0)
  gtk_signal_connect(GTK_OBJECT(dialog),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
//gtk_signal_connect(GTK_OBJECT(dialog),"key_press_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect_after(GTK_OBJECT(dialog),"key_press_event",GTK_SIGNAL_FUNC(iceb_vihod),&i);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(dialog),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog),TRUE);
 }

vbox=gtk_vbox_new(FALSE,0);
gtk_container_add(GTK_CONTAINER(dialog),vbox);
gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);

gtk_box_pack_start((GtkBox*)vbox,ok_button,FALSE,FALSE,0);

gtk_widget_grab_focus(ok_button);
gtk_signal_connect_object( GTK_OBJECT( ok_button ), "clicked",GTK_SIGNAL_FUNC( gtk_widget_destroy ),GTK_OBJECT(dialog));

gtk_window_set_modal( GTK_WINDOW( dialog ) ,TRUE ); 

gtk_widget_show_all( dialog );
gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

}










