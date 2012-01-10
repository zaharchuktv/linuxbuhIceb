/*$Id: taxi_saldo.c,v 1.11 2011-02-21 07:36:21 sasa Exp $*/
/*28.05.2009	04.10.2003	Белых А.И.	taxi_saldo.c
Получение сальдо по постоянному клиенту
*/
#include        <time.h>
#include        <stdlib.h>
#include        <unistd.h>
#include "taxi.h"
extern SQL_baza	bd;
extern short start_god_taxi;

double  taxi_saldo(const char *kodkl)
{
char strsql[300];
SQL_str  row;
SQLCURSOR cur;
double suma=0.;
int  kolstr=0;

sprintf(strsql,"select saldo from Taxiklsal where kod='%s' and god=%d",kodkl,start_god_taxi);
if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
 suma=atof(row[0]);

if(start_god_taxi == 0)
  sprintf(strsql,"select suma from Taxizak where kodk='%s' and suma <> 0. and kz <> 0",kodkl);
else
 {

  sprintf(strsql,"select suma from Taxizak where kodk='%s' and suma <> 0. and kz <> 0 \
and datvz >= '%04d-01-01'",kodkl,start_god_taxi);

 }


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(0.);
 }
while(cur.read_cursor(&row) != 0)
 suma+=atof(row[0]);

return(suma);

}

/**************************************/
/**************************************/

double  taxi_saldo(const char *kodkl,short godk)
{
char strsql[300];
SQL_str  row;
SQLCURSOR cur;
double suma=0.;
int  kolstr=0;

sprintf(strsql,"select saldo from Taxiklsal where kod='%s' and god=%d",kodkl,start_god_taxi);
if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
 suma=atof(row[0]);

if(start_god_taxi == 0)
  sprintf(strsql,"select suma from Taxizak where kodk='%s' and suma <> 0. and kz <> 0 \
and datvz <= '%04d-12-31'",kodkl,godk);
else
 {

  sprintf(strsql,"select suma from Taxizak where kodk='%s' and suma <> 0. and kz <> 0 \
and datvz >= '%04d-01-01' and datvz < '%04d-01-01'",kodkl,start_god_taxi,godk);

 }


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(0.);
 }
while(cur.read_cursor(&row) != 0)
 suma+=atof(row[0]);

return(suma);

}
