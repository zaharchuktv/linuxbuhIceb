/*$Id: iceb_00_skl.c,v 1.6 2011-01-13 13:49:58 sasa Exp $*/
/*10.08.2004	29.07.2004	Белых А.И.	iceb_00_skl.c
Получениe кода склада из кода контрагента
Возвращает код склада
*/
#include <stdlib.h>
#include "iceb_libbuh.h"

int iceb_00_skl(const char *kontr)
{

if(kontr[0] == '0' && kontr[1] == '0' && kontr[2] == '-')
 {
  char sklad[32];
  iceb_u_polen(kontr,sklad,sizeof(sklad),2,'-');
  return(atoi(sklad));  
 }

return(0);

}
