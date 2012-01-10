/* $Id: provpodkow.c,v 1.4 2011-02-21 07:35:56 sasa Exp $ */
/*13.11.2008	29.09.2000	Белых А.И.	provpodkow.c
Проверка подтверждения всех записей в кассовом ордере
*/
#include        "buhg_g.h"

extern SQL_baza bd;

void provpodkow(const char *kassa,const char *nomd,short dd,short md,short gd,short tipz,GtkWidget *wpredok)
{
char		strsql[512];
int		kolstr;

sprintf(strsql,"select datp from Kasord1 where datp='0000-00-00' \
and kassa=%s and nomd='%s' and tp=%d and datd='%d-%d-%d'",
kassa,nomd,tipz,gd,md,dd);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"",strsql,wpredok);
  return;
 }

if(kolstr != 0)
  sprintf(strsql,"update Kasord set podt=0 where kassa=%s and tp=%d \
and nomd='%s' and god=%d",kassa,tipz,nomd,gd);
else 
  sprintf(strsql,"update Kasord set podt=1 where kassa=%s and tp=%d \
and nomd='%s' and god=%d",kassa,tipz,nomd,gd);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  {
   iceb_msql_error(&bd,"",strsql,wpredok);
  }
}
