/* $Id: nomkmw.c,v 1.6 2011-02-21 07:35:55 sasa Exp $ */
/*19.07.2005	09.03.2000	Белих А.И.	nomkmw.c
Автоматическое определение кода материалла и услуги
*/
#include <stdlib.h>
#include        "buhg_g.h"

extern SQL_baza	bd;

int		nomkmw(GtkWidget *wpredok)
{
int             i;
char		strsql[512];
SQL_str         row;
long		kolstr;
short		metka;

iceb_clock sss(wpredok);
metka=i=0;


sprintf(strsql,"select kodm from Material order by kodm asc");
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s\n",row[0]);
  i++;
naz:;
  if(i != atoi(row[0]))
   {
//    printw("i=%d row[0]=%s\n",i,row[0]);
    sprintf(strsql,"select kodus from Uslugi where kodus=%d",i);
    if(sql_readkey(&bd,strsql) <= 0)
     {
      return(i);
     }
    i++;
    goto naz;
   }
 }

//printw("\ni=%d\n",i);
//OSTANOV();

sprintf(strsql,"select kodus from Uslugi order by kodus asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(++i);
 }

while(cur.read_cursor(&row) != 0)
 {
  i++;

naz1:;
  if(i != atoi(row[0]))
   {
    sprintf(strsql,"select kodm from Material where kodm=%d",i);
    if(sql_readkey(&bd,strsql) == 0)
     {
      sprintf(strsql,"select kodus from Uslugi where kodus=%d",i);
      if(sql_readkey(&bd,strsql) <= 0)
       {
        return(i);
       }
     }
    i++;
    goto naz1;
   }
 }
/*
printw("*i=%d\n",i);
OSTANOV();
*/
i++;

return(i);

}
