/*$Id: iceb_u_poltekdat.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*16.04.2004	16.07.2004	Белых А.И.	iceb_u_poltekdat.c
Получение текущего числа
*/
#include <time.h>

void iceb_u_poltekdat(short *dt,short *mt,short *gt)
{
time_t vrem;
time(&vrem);
struct tm *bf=localtime(&vrem);

*dt=bf->tm_mday;
*mt=bf->tm_mon+1;
*gt=bf->tm_year+1900;

}
