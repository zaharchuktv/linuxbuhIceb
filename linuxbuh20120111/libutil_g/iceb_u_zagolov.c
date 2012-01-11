/*$Id: iceb_u_zagolov.c,v 1.11 2011-02-21 07:36:13 sasa Exp $*/
/*11.02.2010	18.06.2001	Белых А.И.	iceb_u_zagolov.c
Заголовок распечаток
*/
#include        <stdio.h>
#include        <time.h>
#include "iceb_util.h"

void	iceb_u_zagolov(const char *naim,const char *datan,
const char *datak,
const char *organ,FILE *ff)
{
short dn,mn,gn;
short dk,mk,gk;
iceb_u_rsdat(&dn,&mn,&gn,datan,1);
iceb_u_rsdat(&dk,&mk,&gk,datak,1);

iceb_u_zagolov(naim,dn,mn,gn,dk,mk,gk,organ,ff);

}
/******************************/
void	iceb_u_zagolov(const char *naim,short dn,short mn,short gn,
short dk,short mk,short gk,const char *organ,FILE *ff)
{
struct tm *bf;
time_t     vrem;
if(ff == NULL)
 return;
time(&vrem);
bf=localtime(&vrem);

fprintf(ff,"%s\n\n%s\n",organ,naim);

if(dn != 0)
  fprintf(ff,"%s %d.%d.%d%s %s %d.%d.%d%s\n",
  gettext("Период с"),
  dn,mn,gn,
  gettext("г."),
  gettext("по"),
  dk,mk,gk,
  gettext("г."));
else
 {
  if(dk != 0)
   fprintf(ff,"%s:%02d.%02d.%d%s\n",gettext("Дата"),
   dk,mk,gk,gettext("г."));
 
  if(dk == 0 && mk != 0)
   fprintf(ff,"%s:%02d.%d%s\n",gettext("Дата"),
   mk,gk,gettext("г."));

  if(dk == 0 && mk == 0 && gk != 0)
   fprintf(ff,"%s:%d%s\n",gettext("Дата"),
   gk,gettext("г."));
 
 }  

fprintf(ff,"\
%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

}
