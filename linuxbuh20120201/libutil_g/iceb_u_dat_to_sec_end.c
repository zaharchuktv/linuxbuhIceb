/*$Id: iceb_u_dat_to_sec_end.c,v 1.5 2011-01-13 13:50:05 sasa Exp $*/
/*04.04.2005	04.04.2005	Белых А.И.	iceb_u_dat_to_sec_end.c
Преобразование даты в количество секунд
*/
#include <time.h>
#include "iceb_util.h"

time_t iceb_u_dat_to_sec_end(const char *data)
{

short d,m,g;
iceb_u_rsdat(&d,&m,&g,data,0);

struct tm bf;

bf.tm_mday=d;
bf.tm_mon=m-1;
bf.tm_year=(g-1900);
bf.tm_hour=24;

bf.tm_min=0;
bf.tm_sec=0;

time_t vrem=0;

vrem=mktime(&bf);

return(vrem);

}

time_t iceb_u_dat_to_sec_end(short god)
{


struct tm bf;

bf.tm_mday=1;
bf.tm_mon=0;
bf.tm_year=(god-1900);
bf.tm_hour=24;

bf.tm_min=0;
bf.tm_sec=0;

time_t vrem=0;

vrem=mktime(&bf);

return(vrem);

}

time_t iceb_u_dat_to_sec_end(short den,short mes,short god)
{


struct tm bf;

bf.tm_mday=den;
bf.tm_mon=mes-1;
bf.tm_year=(god-1900);
bf.tm_hour=24;

bf.tm_min=0;
bf.tm_sec=0;

time_t vrem=0;

vrem=mktime(&bf);

return(vrem);

}
