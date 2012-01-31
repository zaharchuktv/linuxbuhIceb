/*$Id: oplmuw.c,v 1.3 2011-01-13 13:49:51 sasa Exp $*/
/*16.12.2009	16.12.2009	Белых А.И.	oplmuw.c
проверка кода операции и снятие метки неоплаты для внутренних документов
*/
#include <stdlib.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern SQL_baza bd;

void oplmuw(short d,short m,short g,int tipz,const char *nn,int skll,const char *kodop,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

if(iceb_u_SRAV("000",nn,0) != 0)
 {
  if(tipz == 1)
    sprintf(strsql,"select vido from Prihod where kod='%s'",kodop);
  if(tipz == 2)
    sprintf(strsql,"select vido from Rashod where kod='%s'",kodop);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   return;

  if(atoi(row[0]) == 0) 
   return;
 }
  
sprintf(strsql,"update Dokummat \
set \
mo=%d \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and tip=%d",
1,g,m,d,skll,nn,tipz);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  iceb_msql_error(&bd,"uplmu-Ошибка корректировки записи !",strsql,wpredok);


}

