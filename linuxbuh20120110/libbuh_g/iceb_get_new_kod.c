/*$Id: iceb_get_new_kod.c,v 1.19 2011-09-05 08:18:29 sasa Exp $*/
/*13.03.2010	29.09.2003	Белых А.И.	iceb_get_new_kod.c
Получение кода для новой записи
*/
#include        <stdlib.h>
#include        <nl_types.h>
#include        <errno.h>
#include        <unistd.h>
#include "iceb_libbuh.h"

int   iceb_get_new_kod1(const char *tabl,const char *kol,int metkak,GtkWidget*);


extern SQL_baza         bd;

int   iceb_get_new_kod(const char *tabl,int metkak,GtkWidget *wpredok) //0-цифровая колонка 1-символьная
{
const char *kol={"kod"};

return(iceb_get_new_kod1(tabl,kol,metkak,wpredok));
}

int   iceb_get_new_kod(const char *tabl,const char *kol,int metkak,GtkWidget *wpredok) //0-цифровая колонка 1-символьная
{
return(iceb_get_new_kod1(tabl,kol,metkak,wpredok));
}

/******************************************************/

int   iceb_get_new_kod1(const char *tabl,const char *kol,int metkak,GtkWidget *wpredok) //0-цифровая колонка 1-символьная
{
char strsql[512];
int  kolstr=0;
SQL_str row,rowtmp;
int kod=0;




if(metkak == 0)
 {
  sprintf(strsql,"select %s from %s order by %s asc",kol,tabl,kol);
  SQLCURSOR cur;
//  printf("iceb_get_new_kod strsql=%s\n",strsql);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return(0);
   }
  if(kolstr == 0)
    return(1);


  kod=1;
  while(cur.read_cursor(&row) != 0)
   {
    if(atoi(row[0]) != kod)
      break;  
    kod++;
   }
  return(kod);
 }

sprintf(strsql,"select %s from %s order by %s asc",kol,tabl,kol);
if(iceb_u_SRAV("Kontragent",tabl,0) == 0)
  sprintf(strsql,"select kodkon from %s order by kodkon asc",tabl);

SQLCURSOR cur,curtmp;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
  return(1);

class iceb_tmptab tabtmp;
const char *imatmptab={"icebgetnewkod"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (kod int not null primary key)",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,wpredok) != 0)
 {
  return(0);
 }  


while(cur.read_cursor(&row) != 0)
 {
  if(atoi(row[0]) == 0)
   continue;

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



kod=1;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  if(atoi(rowtmp[0]) != kod)
    break;  
  kod++;
 }


return(kod);

}
