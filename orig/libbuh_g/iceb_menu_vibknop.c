/*$Id: iceb_menu_vibknop.c,v 1.3 2011-01-13 13:49:59 sasa Exp $*/
/*05.01.2004	05.01.2004	Белых А.И.	iceb_menu_vibknop.c
Выбор кнопки в простом меню выбора
*/
#include "iceb_libbuh.h"

void  iceb_menu_vibknop(GtkWidget *widget,struct vibknop_data *data)
{

int kn=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->knopka=kn;

gtk_widget_destroy(data->window);

}
