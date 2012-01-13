/*$Id: iceb_u_vrem.c,v 1.4 2011-01-13 13:50:05 sasa Exp $*/
/*08.01.2010	06.07.2004	Белых А.И.	iceb_u_vrem.c
Получение даты и времени в секундах
*/
#include "iceb_util.h"

time_t iceb_u_vrem(iceb_u_str *data,iceb_u_str *vrem,int metka) //0-время начала 1-время конца
{

short dn,mn,gn;
char datat[56];
memset(datat,'\0',sizeof(datat));
strncpy(datat,data->ravno(),sizeof(datat)-1);

iceb_u_rsdat(&dn,&mn,&gn,datat,1);

short hh,mm,ss;
if(metka == 1 && vrem->getdlinna() <= 1)
 {
  hh=24;
  mm=ss=0;
 }
else
 iceb_u_rstime(&hh,&mm,&ss,vrem->ravno());

struct tm bf;
memset(&bf,'\0',sizeof(bf));

bf.tm_mday=(int)dn;
bf.tm_mon=(int)(mn-1);
bf.tm_year=(int)(gn-1900);
bf.tm_hour=(int)hh;

bf.tm_min=(int)mm;
bf.tm_sec=ss;


return(mktime(&bf));


}
