/*$Id: iceb_alxout.c,v 1.4 2011-02-21 07:36:06 sasa Exp $*/
/*07.09.2010	07.09.2010	Белых А.И.	iceb_alxout.c
Выгрузка настроек в файл
*/
#include <errno.h>
#include "iceb_libbuh.h"
extern SQL_baza bd;
int iceb_alxout(const char *imaf,GtkWidget *wpredok)
{
FILE *ff;
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"",strsql,wpredok);
  return(1);
 }  

if(kolstr == 0)
 {
//  sprintf(strsql,"%s-%s\n%s",__FUNCTION__,gettext("Не найдено ни одной записи !"),imaf);
//  iceb_t_soob(strsql);
  return(1);
 }

if((ff=fopen(imaf,"w")) == NULL)
  {
   iceb_er_op_fil(imaf,"",errno,wpredok);
   return(1);
  }
while(cur.read_cursor(&row) != 0)
 {
  fprintf(ff,"%s",row[0]);
 }

fclose(ff);

return(0);
}


