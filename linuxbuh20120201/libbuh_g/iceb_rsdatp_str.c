/*$Id: iceb_rsdatp_str.c,v 1.3 2011-01-13 13:49:59 sasa Exp $*/
/*06.06.2004	06.06.2004	Белых А.И.	iceb_rsdatp_str.c
Проверка коректности ввода двух дат
Если вернули 0-даты введены правильно
*/
#include "iceb_libbuh.h"

int iceb_rsdatp_str(iceb_u_str *datan,iceb_u_str *datak,GtkWidget *wpredok)
{
short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,datan->ravno(),&dk,&mk,&gk,datak->ravno(),wpredok) != 0)
 return(1);

if(datak->getdlinna() <= 1)
 {
  char bros[50];
  sprintf(bros,"%d.%d.%d",dk,mk,gk);
  datak->new_plus(bros);
 }

return(0);

}
