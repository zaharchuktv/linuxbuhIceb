/*$Id: sosnsumw.c,v 1.5 2011-02-21 07:35:57 sasa Exp $*/
/*29.11.2006	29.11.2006	Белых А.И.	sosnsumw.c
Получение суммы начислений
*/
#include <stdlib.h>
#include "buhg_g.h"

extern SQL_baza bd;

double	sosnsumw(char *tabn,short dn,short mn,short gn,
short dk,short mk,short gk,GtkWidget *wpredok)
{
char	strsql[512];
int	kolstr=0;
SQL_str	row;
double suma=0.;

sprintf(strsql,"select suma from Zarp where tabn=%s \
and datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and prn='1'",
tabn,gn,mn,dn,gk,mk,dk);

class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.); 
 }
while(cur.read_cursor(&row) != 0)
  suma+=atof(row[0]);
return(suma);

}
