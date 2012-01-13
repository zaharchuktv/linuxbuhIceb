/*$Id: iceb_printw_vr.c,v 1.8 2011-02-21 07:36:07 sasa Exp $*/
/*11.06.2004	21.12.2003	Белых А.И.	iceb_printw_vr.c
Вывод в окно времени расчета
*/
#include <time.h>
#include <math.h>
#include "iceb_libbuh.h"


void iceb_printw_vr(time_t vrem_n, //Время начала расчета
GtkTextBuffer *buffer,GtkWidget *view)
{
char stroka[300];
time_t vrem_k;

time(&vrem_k);

double kii;
double dii=(vrem_k-vrem_n)/60.;

dii=modf(dii,&kii);

sprintf(stroka,"%s %.f%s %.f%s\n",
gettext("Время расчета"),
kii,
gettext("мин."),
dii*60.,
gettext("сек."));

GtkTextIter iter;

gtk_text_buffer_get_iter_at_offset(buffer,&iter,-1);
gtk_text_buffer_insert(buffer,&iter,iceb_u_toutf(stroka),-1);
gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view),&iter,0.0,TRUE,0.,1.);
//gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer),&iter);

iceb_refresh();
}

