/* $Id: poduosdw.c,v 1.6 2011-02-21 07:35:55 sasa Exp $ */
/*16.12.2009    16.12.2009      Белых А.И.      poduosdw.c
Проверка подтверждения документа
*/
#include <stdlib.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void poduosdw(short d,short m,short g,const char *ndk,GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row;
char		strsql[512];
int		mp;
short		podt;

sprintf(strsql,"select podt from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s'",g,m,d,ndk);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  return;
 }
mp=0;
while(cur.read_cursor(&row) != 0)
 {
  if(atoi(row[0]) == 1)
   mp++;  
 }


podt=0;
if(kolstr == mp)
 podt=1;

/*
printw("podt=%d\n",podt);
OSTANOV();
*/

sprintf(strsql,"update Uosdok \
set \
podt=%d \
where datd='%04d-%d-%d' and nomd='%s'",podt,g,m,d,ndk);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  {
   char bros[512];
   sprintf(bros,"poduosdw-%s",gettext("Ошибка корректировки записи !"));
   iceb_msql_error(&bd,bros,strsql,wpredok);
  }

}
