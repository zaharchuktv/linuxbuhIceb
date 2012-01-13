/*$Id: iceb_pnds.c,v 1.4 2011-02-21 07:36:07 sasa Exp $*/
/*18.10.2010	14.10.2010	Белых А.И.	iceb_pnds.c
Получение действующего процента НДС по дате
*/
#include <stdlib.h>
#include "iceb_libbuh.h"
extern SQL_baza bd;

double iceb_pnds(short d,short m,short g,GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

char strsql[512];

sprintf(strsql,"select pr from Nalog where vn=0 and dndn <= '%04d-%02d-%02d' order by dndn desc limit 1",g,m,d);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"",strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  printf("%s-%s\n",__FUNCTION__,strsql);
  
  iceb_menu_soob(gettext("Не нашли процент НДС!"),wpredok);
  return(0.);
 }
cur.read_cursor(&row);
return(atof(row[0]));
}
/**********************************/
double iceb_pnds(const char *data,GtkWidget *wpredok)
{
short d=0,m=0,g=0;
iceb_u_rsdat(&d,&m,&g,data,0);
return( iceb_pnds(d,m,g,wpredok));

}
/**********************************/
double iceb_pnds(GtkWidget *wpredok)
{
short d=0,m=0,g=0;
iceb_u_poltekdat(&d,&m,&g);
return(iceb_pnds(d,m,g,wpredok));

}
