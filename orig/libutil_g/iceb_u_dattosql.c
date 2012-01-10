/*$Id: iceb_u_dattosql.c,v 1.2 2011-01-13 13:50:05 sasa Exp $*/
/*16.11.2009	13.11.2009	Белых А.И.	iceb_u_dattosql.c
Преобразование даты в формат для sql-запроса
*/
#include "iceb_util.h"

const char *iceb_u_dattosql(const char *data)
{
static char stroka[16];
memset(stroka,'\0',sizeof(stroka));
short d=0,m=0,g=0;
iceb_u_rsdat(&d,&m,&g,data,1);
sprintf(stroka,"%04d-%02d-%02d",g,m,d);

return(stroka);
}
