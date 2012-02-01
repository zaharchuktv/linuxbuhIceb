/*$Id: iceb_u_sqldata.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*13.11.2009	08.01.2004	Белых А.И.	iceb_u_sqldata.c
Преобразование даты из SQL формата в нормальное в символьном виде
*/
#include "iceb_util.h"

char *iceb_u_sqldata(char *data)
{
static char datap[32];
short d,m,g;

iceb_u_rsdat(&d,&m,&g,data,2);
sprintf(datap,"%02d.%02d.%04d",d,m,g);
return(datap);

}
