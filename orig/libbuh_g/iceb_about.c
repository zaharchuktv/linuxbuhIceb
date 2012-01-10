/*$Id: iceb_about.c,v 1.5 2011-02-21 07:36:06 sasa Exp $*/
/*03.01.2011	14.04.2010	Белых А.И.	iceb_about.c
получение информации о системе
*/
#include        "iceb_libbuh.h"

extern char *version;

/*************************/

void iceb_about()
{
printf("%s\n",__FUNCTION__);

GtkWidget *about=gtk_about_dialog_new();
gtk_signal_connect(GTK_OBJECT(about),"response",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
//g_signal_connect_swapped(about,"response",G_CALLBACK(gtk_widget_destroy),about);

gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(about),"iceBw");
gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about),version);
gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about),"http://iceb.com.ua");
gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about),gettext("Бухгалтерский учёт"));
//gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about),"Copyright \302\251 Aleksandr Belykh");
gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about),"Copyright © Aleksandr Belykh");

const gchar *avtor[]={"Aleksandr Belykh","<sasa@ukrpost.ua>","http://www.iceb.vc.ukrtel.net","tel:+380676654526"};
 
gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about),avtor);


gtk_dialog_run(GTK_DIALOG(about));

}
