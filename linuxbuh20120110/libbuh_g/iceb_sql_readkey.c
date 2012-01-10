/*$Id: iceb_sql_readkey.c,v 1.5 2011-01-13 13:49:59 sasa Exp $*/
/*12.04.2009	19.03.2004	Белых А.И.	iceb_sql_readkey.c
Чтение нужной записи из базы данных
*/
#include	"iceb_libbuh.h"


extern SQL_baza	bd;


int iceb_sql_readkey(const char *zapros,SQL_str *row,SQLCURSOR *cur,
GtkWidget *wpredok)
{
int voz=0;
if((voz=sql_readkey(&bd,zapros,row,cur)) < 0)
  iceb_msql_error(&bd,"",zapros,wpredok);
return(voz);
}

int iceb_sql_readkey(const char *zapros,GtkWidget *wpredok)
{
int voz=0;
if((voz=sql_readkey(&bd,zapros)) < 0)
  iceb_msql_error(&bd,"",zapros,wpredok);
return(voz);
}

