/*$Id: iceb_u_datetime_pr.c,v 1.4 2011-01-13 13:50:05 sasa Exp $*/
/*08.01.2010	15.01.2007	Белых А.И.	iceb_u_datetime_pr.c
Преобразование 	DATETIME в нормальное представление в символьном виде
*/
#include "iceb_util.h"

const char *iceb_u_datetime_pr(char *datatime)
{
static char datatime_pr[32];
char date[56];
char vrem[56];
memset(date,'\0',sizeof(date));
memset(vrem,'\0',sizeof(vrem));

iceb_u_polen(datatime,date,sizeof(date),1,' ');
iceb_u_polen(datatime,vrem,sizeof(vrem),2,' ');
short d=0,m=0,g=0;
iceb_u_rsdat(&d,&m,&g,date,2);
sprintf(datatime_pr,"%02d.%02d.%d %s",d,m,g,vrem);
//printf("iceb_u_datetime_pr=%s\n",datatime_pr);
return(datatime_pr);

}
