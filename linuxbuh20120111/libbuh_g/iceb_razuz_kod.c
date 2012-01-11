/*$Id: iceb_razuz_kod.c,v 1.10 2011-01-13 13:49:59 sasa Exp $*/
/*10.11.2007	03.09.2004	Белых А.И.	iceb_razuz_kod.c
Получение на заданный код списка входящих деталей
Если вернули 0-нет входящих деталей
*/
#include "iceb_libbuh.h"

extern SQL_baza	bd;

int iceb_razuz_kod(class iceb_razuz_data *data,GtkWidget *wpredok)
{
char strsql[512];
SQL_str  row;
SQLCURSOR cur;

sprintf(strsql,"select kodi from Specif where kodi=%d limit 1",data->kod_izd);
//printf("iceb_razuz_kod=%s\n",strsql);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 return(0);

/*Определяем входящие узлы*/

iceb_razuz(data,wpredok);

/*Делаем расчет входящих деталей*/
int kolndet=iceb_razuz1(data,wpredok);

return(kolndet);
}
