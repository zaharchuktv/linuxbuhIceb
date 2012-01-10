	/* $Id: prbper1w.c,v 1.3 2011-01-13 13:49:52 sasa Exp $ */
/*05.12.2001    27.12.1996	Белых А.И.	prbpern1w.c
Ищем все удержания в счет расчетного месяца включая и расчетный
месяц
*/
#include <stdlib.h>
#include        "buhg_g.h"
extern SQL_baza bd;

double prbper1w(short mz,short gz,short dk,short mk,short gk,
int tabb,GtkWidget *wpredok)
{
double          sum=0.;
long		kolstr;
SQL_str         row;
char		strsql[200];

sprintf(strsql,"select suma from Zarp where godn=%d and mesn=%d and \
tabn=%d and prn='2' and datz <= '%d-%d-%d' and suma <> 0. \
order by tabn,godn,mesn asc",gz,mz,tabb,gk,mk,dk);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"prbper1w",strsql,wpredok);
  return(0.);
 }
while(cur.read_cursor(&row) != 0)
  sum+=atof(row[0]);

if(sum != 0.)
 sum=iceb_u_okrug(sum,0.01);

return(sum);

}
