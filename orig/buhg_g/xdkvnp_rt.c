/*$Id: xdkvnp_rt.c,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*23.05.2010	23.05.2010	Белых А.И.	xdkvnp_rt.c
Чтение назначения платежа
*/
#include "buhg_g.h"

void xdkvnp_rt(class iceb_u_str *text,GtkTextBuffer *buffer)
{

GtkTextIter iter_start;
GtkTextIter iter_end;

gtk_text_buffer_get_start_iter (buffer,&iter_start);
gtk_text_buffer_get_end_iter (buffer,&iter_end);

 
//gtk_text_buffer_get_iter_at_offset (data->buffer,&iter_start,0);
//gtk_text_buffer_get_iter_at_offset (data->buffer,&iter_end,-1);
text->new_plus(iceb_u_fromutf(gtk_text_buffer_get_text(buffer,&iter_start,&iter_end,FALSE)));


}