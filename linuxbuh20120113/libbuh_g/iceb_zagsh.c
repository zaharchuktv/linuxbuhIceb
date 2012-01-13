/*$Id: iceb_zagsh.c,v 1.9 2011-02-21 07:36:08 sasa Exp $*/
/*13.03.2010	19.12.2003	Белых А.И.	iceb_zagsh.c
Заголовок акта сверки по контрагентам
*/
#include <stdio.h>
#include "iceb_libbuh.h"


void iceb_zagsh(const char *shet,FILE *ff,GtkWidget *wpredok)
{
char	strsql[512];
SQL_str row;
SQLCURSOR cur;
class iceb_u_str naim("");

sprintf(strsql,"select nais from Plansh where ns='%s'",shet);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim.new_plus(row[0]);
 
fprintf(ff,"\n%s:%s %s\n",gettext("Счет"),shet,naim.ravno());
fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
fprintf(ff,gettext("\
  Дата    |   Коментарий       |  Дебет   |  Кредит  | |  Дата    |   Коментарий       |  Дебет | Кредит |\n"));

fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");

}
