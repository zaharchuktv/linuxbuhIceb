/*$Id: iceb_u_fio.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*28.07.2008	28.07.2008	Белых А.И.	iceb_u_fio.c
Получение фамилии с инициалами
*/
#include "iceb_util.h"

void iceb_u_fio(const char *fio,class iceb_u_str *fio_in)
{


if(iceb_u_polen(fio,fio_in,1,' ') == 0)
 {
  char imq[56];
  char otch[56];
  memset(imq,'\0',sizeof(imq));
  memset(otch,'\0',sizeof(otch));
    
  iceb_u_polen(fio,imq,sizeof(imq),2,' ');
  iceb_u_polen(fio,otch,sizeof(otch),3,' ');
  fio_in->plus(" ");
  fio_in->plus(imq,1);
  fio_in->plus(".");
  fio_in->plus(otch,1);
  fio_in->plus(".");
 }
else
 {
  fio_in->new_plus(fio);
 }  

}
