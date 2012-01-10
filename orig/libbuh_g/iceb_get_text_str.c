/*$Id: iceb_get_text_str.c,v 1.3 2011-01-13 13:49:59 sasa Exp $*/
/*16.09.2003	09.09.2003	Белых А.И.	iceb_get_text_str.c
Получение текста из строки ввода
*/

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "iceb_libbuh.h"


void iceb_get_text_str(GtkWidget *widget,iceb_u_str *text)
{
text->new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
//printf("\niceb_get_text-%s\n",text->ravno());
gtk_entry_select_region(GTK_ENTRY(widget),0,text->getdlinna());

}
