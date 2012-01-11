/*$Id: iceb_u_vrem_tek.c,v 1.4 2011-01-13 13:50:05 sasa Exp $*/
/*08.01.2010	20.12.2004	Белых А.И.	iceb_u_poltekhas.c
Получение текущено времени час,минуту,секунду в символьном виде
*/
#include <stdio.h>
#include <time.h>

char *iceb_u_vrem_tek()
{
static char vrem_tek[32];

struct tm *bf;
time_t vrem;

time(&vrem);

bf=localtime(&vrem);

sprintf(vrem_tek,"%02d:%02d:%02d",
bf->tm_hour,
bf->tm_min,
bf->tm_sec);

return(vrem_tek);

}
