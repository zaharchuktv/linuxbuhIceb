/* $Id: redkalw.c,v 1.8 2011-02-21 07:35:57 sasa Exp $ */
/*09.12.2010    06.01.1994      Белых А.И.      redkalw.c
Чтение количества рабочих дней и часов за нужный месяц
*/
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void redkalw(short m,short g, //Дата
float *dnei,float *has,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
char            bros[1024];
char            data[24];
short           dp,gp;

*dnei=0.;
*has=0.;
sprintf(strsql,"select str from Alx where fil='zarkalndr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],data,sizeof(data),1,'|')  == 0)
   {
    iceb_u_rsdat1(&dp,&gp,data);
    if(dp == m && gp == g)
     {
      iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
      *dnei=(float)iceb_u_atof(bros);
      iceb_u_polen(row_alx[0],bros,sizeof(bros),3,'|');
      *has=(float)iceb_u_atof(bros);
      break;
     }
   }
 }
}
