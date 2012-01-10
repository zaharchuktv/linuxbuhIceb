/* $Id: okladw.c,v 1.11 2011-04-14 16:09:36 sasa Exp $ */
/*12.04.2011	01.08.2000	Белых А.И.	okladw.c
Найти в файле оклад на нужный табельный номер
*/
#include <stdlib.h>
#include        <errno.h>
#include <unistd.h>
#include "buhg_g.h"

extern SQL_baza bd;

double okladw(int tabn,
short mr,short gr,
int *has, //0-оклад 1-часовая ставка
GtkWidget *wpredok)
{
char strsql[512];
double		oklad;
char		bros[512];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

*has=0;

sprintf(strsql,"select str from Alx where fil='zardolhn.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  return(1);
 }

oklad=0.;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
//  printw("%s",row_alx[0]);
  iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|'); 
  if(tabn == atoi(bros))
   {
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|'); 
    if(iceb_u_strstrm(bros,"Ч") != 0)
      *has=1; 
    oklad=atof(bros);
    if(iceb_u_strstrm(bros,"R") != 0)
     oklad=okladtrw(mr,gr,oklad,NULL,wpredok);
    break;
   }
 }
return(oklad);
}
