/*$Id: iceb_00_kontr.c,v 1.4 2011-01-13 13:49:58 sasa Exp $*/
/*08.08.2004	08.08.2004	Белых А.И.	iceb_00_kontr.c
Определение правильного кода контрагента
*/
#include <stdlib.h>
#include "iceb_libbuh.h"

void iceb_00_kontr(const char *kontr_old,iceb_u_str *kontr_new)
{

if(kontr_old[0] == '0' && kontr_old[1] == '0' && kontr_old[2] == '-')
 {
  kontr_new->new_plus("00");
  return;
 }

kontr_new->new_plus(kontr_old);


}
