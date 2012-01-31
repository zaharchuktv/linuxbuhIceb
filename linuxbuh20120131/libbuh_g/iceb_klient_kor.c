/*$Id: iceb_klient_kor.c,v 1.6 2011-02-21 07:36:07 sasa Exp $*/
/*14.11.2008	01.03.2004	Белых А.И.	iceb_klient_kor.c
Корректировка кода клиента в таблицах
*/
#include "iceb_libbuh.h"

extern SQL_baza	bd;

void iceb_klient_kor(const char *skod,const char *nkod)
{
char strsql[500];

sprintf(strsql,"update Taxizak set kodk='%s' where kodk='%s'",\
nkod,skod);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,NULL);

sprintf(strsql,"update Restdok set kodkl='%s' where kodkl='%s'",\
nkod,skod);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,NULL);

sprintf(strsql,"update Taxiklsal set kod='%s' where kod='%s'",\
nkod,skod);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
    iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,NULL);
}
