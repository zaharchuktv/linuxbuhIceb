/*$Id: iceb_getuslp_m.c,v 1.1 2011-02-21 07:36:07 sasa Exp $*/
/*21.02.2011	21.02.2011	Белых А.И.	iceb_getusl_m.c
Получение условия продажи
*/
#include "iceb_libbuh.h"

int iceb_getuslp_m(const char *kodkon,class iceb_u_str *usl_prod,GtkWidget *wpredok)
{
char strsql[512];
int kolzap=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select datd,nomd,vidd from Kontragent2 where kodkon='%s'",kodkon);
if((kolzap=iceb_sql_readkey(strsql,&row,&cur,wpredok)) == 0)
 {
  
  iceb_menu_soob(gettext("Не введён договор для этого контрагента!"),wpredok);
  return(1);
  
 }
if(kolzap == 1)
 {
  sprintf(strsql,"%s:%s:%s",row[2],iceb_u_datzap(row[0]),row[1]);
  usl_prod->new_plus(strsql);
  return(0);
 } 
int un_nom_zap=0;
if((un_nom_zap=iceb_l_kontdog(1,kodkon,wpredok)) == -1)
 return(1);

return(iceb_getuslp(un_nom_zap,usl_prod,wpredok));

}
