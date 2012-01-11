/*$Id: iceb_printw.c,v 1.9 2011-01-13 13:49:59 sasa Exp $*/
/*11.06.2004	19.12.2003	Белых А.И.	iceb_printw.c
Вывод текста в приготовленное окно 
*/
#include "iceb_libbuh.h"

void iceb_printw(const char *stroka,GtkTextBuffer *buffer,GtkWidget *view)
{

GtkTextIter iter;

gtk_text_buffer_get_iter_at_offset(buffer,&iter,-1);
gtk_text_buffer_insert(buffer,&iter,stroka,-1);
gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view),&iter,0.0,TRUE,0.,1.);
//gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer),&iter);
iceb_refresh();
 
}
