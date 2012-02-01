/*$Id: iceb_u_rstime.c,v 1.10 2011-01-13 13:50:05 sasa Exp $*/
/*30.11.2005	16.04.2003	Белых А.И.	iceb_u_rstime.c
Расшифровка времени. Если вернули 0 - время введено правильно
                                  1 - нет
*/
#include	<stdlib.h>
#include	<string.h>
#include	"iceb_util.h"

int    iceb_u_rstime(short *has,short *min,short *sek,const char *vremq)
{
char bros[112];

*has=*min=*sek=0;
if(vremq[0] == '\0')
 return(0);
 
char vrem1[strlen(vremq)+1];
strcpy(vrem1,vremq);

for(int i=0; i < (int)strlen(vrem1); i++)
  if(vrem1[i] == '.' || vrem1[i] == ',')
    vrem1[i]=':';
     
if(iceb_u_polen(vrem1,bros,sizeof(bros),1,':') != 0)
 strncpy(bros,vrem1,sizeof(bros)-1);
*has=atoi(bros);

iceb_u_polen(vrem1,bros,sizeof(bros),2,':');
*min=atoi(bros);

iceb_u_polen(vrem1,bros,sizeof(bros),3,':');
*sek=atoi(bros);

if(*has < 0 || *has > 23)
 return(1);

if(*min < 0 || *min > 60)
 return(1);

if(*sek < 0 || *sek > 60)
 return(1);

return(0);
}
