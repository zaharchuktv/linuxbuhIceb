/*$Id: iceb_vixod.c,v 1.3 2011-02-21 07:36:08 sasa Exp $*/
/*17.12.2010    17.12.2010      Белых А.И.     iceb_vixod.c
Проверка даты
Возвращием 0 - если дата выходная
	   1 - нет
*/
#include	"iceb_libbuh.h"

extern SQL_baza bd;

int iceb_vixod(short d,short m,short g,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
short           dd,me,gg;
short           voz=1;

sprintf(strsql,"select str from Alx where fil='kalend.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  return(1);
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  iceb_u_rsdat(&dd,&me,&gg,row_alx[0],1);

  if(dd == d && me == m && gg == g)
   {
    voz = 0;
    break;
   }

 }
return(voz);

}
