/*$Id: l_restdok2_znm.c,v 1.3 2011-01-13 13:50:09 sasa Exp $*/
/*23.03.2006	23.03.2006	Белых А.И.	l_restdok2_znm.c
Запись в файл настроек меню
*/
#include <errno.h>
#include "i_rest.h"
#include "l_restdok2.h"

void l_restdok2_znm(class restdok2_data *data)
{
const char *imaf={".iceb_l_restdok2.dat"};
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

int kol_pix=gtk_paned_get_position(GTK_PANED(data->gor_panel));
fprintf(ff,"Количество пикселов для разделения горизонтальных панелей|%d\n",kol_pix);

kol_pix=gtk_paned_get_position(GTK_PANED(data->gor_panel));
fprintf(ff,"Количество пикселов для разделения вертикальных панелей|%d\n",kol_pix);

fclose(ff);


}
