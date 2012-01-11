/*$Id: iceb_u_datzap.c,v 1.5 2011-01-13 13:50:05 sasa Exp $*/
/*11.11.2008	30.09.2003	Белых А.И.	iceb_u_datzap.c
Получение символьной строки с датой
*/
#include  "iceb_util.h"

const char *iceb_u_datzap(const char *data)
{
short d=0,m=0,g=0;
static char stroka[32];
memset(stroka,'\0',sizeof(stroka));

if(data[0] == '\0')
 return(stroka);

iceb_u_rsdat(&d,&m,&g,data,2);
if(d != 0)
 sprintf(stroka,"%02d.%02d.%04d",d,m,g);
return(stroka);

}
