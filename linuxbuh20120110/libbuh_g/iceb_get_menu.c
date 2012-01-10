/*$Id: iceb_get_menu.c,v 1.6 2011-01-13 13:49:59 sasa Exp $*/
/*09.08.2004	09.08.2004	Белых А.И.	iceb_get_menu.c
Чтение выбранной в меню строки
*/
#include "iceb_libbuh.h"

void    iceb_get_menu(GtkWidget *widget,int *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
*data=enter;
}
