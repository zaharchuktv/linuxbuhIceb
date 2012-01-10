/*$Id: iceb_u_poltekgod.c,v 1.2 2011-01-13 13:50:05 sasa Exp $*/
/*10.06.2009	10.06.2009	Белых А.И.	iceb_u_poltekgod.c
Получение текущего года
*/
#include <time.h>

int iceb_u_poltekgod()
{
time_t vrem;
time(&vrem);
struct tm *bf=localtime(&vrem);

return(bf->tm_year+1900);

}
