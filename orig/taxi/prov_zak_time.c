/*$Id: prov_zak_time.c,v 1.2 2011-01-13 13:50:09 sasa Exp $*/
/*15.08.2004	15.08.2004	Белых А.И.	prov_zak_time.c
Проверить сколько осталось до даты
Если вернули 0 меньше указанного времени
             1 больше
*/
#include <time.h>
#include "taxi.h"

int prov_zak_time(char *data,char *vremq,int kolsek)
{
short d,m,g;
short has=0,min=0,sek=0;
time_t tek_vrem;

time(&tek_vrem);

iceb_u_rsdat(&d,&m,&g,data,2);
iceb_u_rstime(&has,&min,&sek,vremq);

//printf("prov_zak_time %d.%d.%d %d:%d:%d\n",d,m,g,has,min,sek);

struct tm bf;

bf.tm_mday=d;
bf.tm_mon=m-1;
bf.tm_year=g-1900;
bf.tm_hour=has;

bf.tm_min=min;
bf.tm_sec=sek;

time_t zak_vrem=0;
  
zak_vrem=mktime(&bf);

if( zak_vrem - tek_vrem < (time_t)kolsek)
 {
//  printf("prov_zak_time %ld - %ld = %ld < %d\n",zak_vrem, tek_vrem,zak_vrem - tek_vrem, kolsek);
  return(0);
 }

//printf("prov_zak_time %ld - %ld = %ld > %d\n",zak_vrem, tek_vrem,zak_vrem - tek_vrem, kolsek);
return(1);
}
