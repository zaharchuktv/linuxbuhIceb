/*$Id: iceb_invnomer.c,v 1.8 2011-09-05 08:18:29 sasa Exp $*/
/*13.03.2010	10.10.2002	Белых А.И.	iceb_invnomer.c
Получение Свободного инвентарного номера
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <ctype.h>
#include        <nl_types.h>
#include        "iceb_libbuh.h"
#include        <unistd.h>


extern SQL_baza	bd;

int iceb_invnomer(GtkWidget *wpredok)
{
int		i;
char		strsql[512];
char		bros[312];
SQL_str         row,rowtmp;
long		kolstr;

class iceb_clock kk(wpredok);

class iceb_tmptab tabtmp;
const char *imatmptab={"icebinvnomer"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (kod int not null primary key)",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,wpredok) != 0)
 {
  return(0);
 }  



sprintf(strsql,"select innom from Uosin order by innom asc");
SQLCURSOR cur,curtmp;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  sprintf(bros,"invnomer-%s",gettext("Ошибка создания курсора !"));
  iceb_msql_error(&bd,bros,strsql,wpredok);
 }
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"insert into %s values (%d)",imatmptab,atoi(row[0]));

  iceb_sql_zapis(strsql,1,1,wpredok);    
 }

sprintf(strsql,"select innom from Kart where innom != '' and rnd=''");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  sprintf(bros,"invnomer-%s",gettext("Ошибка создания курсора !"));
  iceb_msql_error(&bd,bros,strsql,wpredok);
 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"insert into %s values (%d)",imatmptab,atoi(row[0]));

  iceb_sql_zapis(strsql,1,1,wpredok);    
 }

sprintf(strsql,"select * from %s order by kod asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

i=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  //Регістационные номера мед.препаратов начинаются на символ
  if(atoi(rowtmp[0]) == 0)  /*Значит символ не число*/
   continue;
  i++;
  if(i != atoi(rowtmp[0]))
   {
    return(i);
   }
 }
i++;

return(i);


}
