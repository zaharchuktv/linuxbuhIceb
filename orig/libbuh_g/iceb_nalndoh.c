/*$Id: iceb_nalndoh.c,v 1.4 2011-01-13 13:49:59 sasa Exp $*/
/*27.08.2004	12.03.2003	Белых А.И.	iceb_nalndoh.c
Чтение настройки
Возвращаем 0-если параметр включен
           1-выключен
*/

#include        "iceb_libbuh.h"


int iceb_nalndoh(GtkWidget *wpredok)
{
char    bros[512];

memset(bros,'\0',sizeof(bros));

if(iceb_poldan("Предприятие является плательщиком налога на доход на общих основаниях",bros,"matnast.alx",wpredok) == 0)
 if(iceb_u_SRAV(bros,"Вкл",1) != 0)
   return(1);  //Не является

return(0);

}
