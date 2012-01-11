/*$Id: iceb_fsystem.c,v 1.6 2011-02-21 07:36:07 sasa Exp $*/
/*09.12.2010	17.03.2009	Белых А.И.	iceb_fsystem.c
Чтение из файла команд и выполнение 
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "iceb_libbuh.h"

extern SQL_baza bd;

void iceb_fsystem(const char *imaf_nast,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_nast);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

int voz=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  iceb_u_ud_simv(row_alx[0],"\n");
  voz=system(row_alx[0]);
 }
}
