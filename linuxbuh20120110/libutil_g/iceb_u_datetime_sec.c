/*$Id: iceb_u_datetime_sec.c,v 1.6 2011-01-13 13:50:05 sasa Exp $*/
/*11.11.2008	05.04.2007	Белых А.И.	iceb_u_datetime_sec.c
Перевод DATETIME(гггг-мм-дд чч:мм:сс) -> в количество секунд
Перевод дд.мм.гг чч:мм:сс -> в количество секунд
*/
#include "iceb_util.h"

time_t iceb_u_datetime_sec_make(short d,short m,short g,short has,short min,short sek);

time_t iceb_u_datetime_sec(const char *datetime)
{
if(datetime[0] == '0')
 return(0);
 
char data[11];
char vrem[20];
iceb_u_polen(datetime,data,sizeof(data),1,' ');
iceb_u_polen(datetime,vrem,sizeof(vrem),2,' ');

short d=0,m=0,g=0;
short has=0,min=0,sek=0;

iceb_u_rsdat(&d,&m,&g,data,2);

iceb_u_rstime(&has,&min,&sek,vrem);


return(iceb_u_datetime_sec_make(d,m,g,has,min,sek));
}
/***************************************/
time_t iceb_u_datetime_sec(short d,short m,short g,short has,short min,short sek)
{

return( iceb_u_datetime_sec_make(d,m,g,has,min,sek));

}
/**************************************/

time_t iceb_u_datetime_sec_make(short d,short m,short g,short has,short min,short sek)
{
time_t kol_sek=0;

struct tm bf;
bf.tm_mday=(int)d;
bf.tm_mon=(int)(m-1);
bf.tm_year=(int)(g-1900);
bf.tm_hour=(int)has;

bf.tm_min=(int)min;
bf.tm_sec=sek;


kol_sek=mktime(&bf);


return(kol_sek);
}

