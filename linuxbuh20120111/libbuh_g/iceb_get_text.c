/*$Id: iceb_get_text.c,v 1.9 2011-01-13 13:49:59 sasa Exp $*/
/*16.09.2003	09.09.2003	Белых А.И.	iceb_get_text.c
Получение текста из строки ввода
*/

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "iceb_libbuh.h"


void iceb_get_text(GtkWidget *widget,char *text)
{
//printf("iceb_get_text char\n");
strcpy(text,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
gtk_entry_select_region(GTK_ENTRY(widget),0,strlen(text));
//printf("iceb_get_text-%s\n",text);
}
