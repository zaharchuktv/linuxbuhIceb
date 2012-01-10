/*$Id: iceb_vihod.c,v 1.4 2011-01-13 13:49:59 sasa Exp $*/
/*18.09.2003	18.09.2003	Белых А.И.	iceb_vihod.c
Обработчик клавиатурного ввода для меню
*/
#include "iceb_libbuh.h"

gboolean iceb_vihod(GtkWidget *widget,GdkEventKey *event,int *knopka)
{

//printf("iceb_vihod-Клавиша нажата.\n");

switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    *knopka=-1;
    gtk_widget_destroy(widget);  
    return(FALSE);
 }

return(TRUE);
}
