/*$Id: iceb_getuslp.c,v 1.1 2011-02-21 07:36:07 sasa Exp $*/
/*16.02.2011	16.02.2011	Белых А.И.	iceb_getuslp.c
Получить условие продажи 
*/
#include "iceb_libbuh.h"

int iceb_getuslp(const char *kodkon,class iceb_u_str *uslprod,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
uslprod->new_plus("");

sprintf(strsql,"select datd,nomd,vidd from Kontragent2 where kodkon='%s' order by datd desc limit 1",kodkon);
//printf("%s-%s\n",__FUNCTION__,strsql);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  sprintf(strsql,"%s:%s:%s",row[2],iceb_u_datzap(row[0]),row[1]);
  uslprod->new_plus(strsql);
  return(0);
 }

return(1);
}

int iceb_getuslp(int un_nom_zap,class iceb_u_str *uslprod,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
uslprod->new_plus("");

sprintf(strsql,"select datd,nomd,vidd from Kontragent2 where nz=%d",un_nom_zap);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  sprintf(strsql,"%s:%s:%s",row[2],iceb_u_datzap(row[0]),row[1]);
  uslprod->new_plus(strsql);
  return(0);
 }

return(1);
}
