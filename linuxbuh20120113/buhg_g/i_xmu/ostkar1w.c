/*$Id: ostkar1w.c,v 1.1 2011-04-01 08:00:50 sasa Exp $*/
/*30.03.2011	30.03.2011	Белых А.И.	ostkar1w.c
получение реального остатка материала по всем карточкам
*/
#include <stdlib.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
extern SQL_baza bd;

double ostkar1w(int kodmat,int sklad,short dost,short most,short gost,GtkWidget *wpredok)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

if(sklad != 0)
 sprintf(strsql,"select sklad,nomk from Kart where sklad=%d and kodm=%d",sklad,kodmat);
else
 sprintf(strsql,"select sklad,nomk from Kart where kodm=%d",kodmat);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  return(0.);
 }
double ostatok=0.;
class ostatok ost;
while(cur.read_cursor(&row) != 0)
 {
  ostkarw(1,1,gost,dost,most,gost,atoi(row[0]),atoi(row[1]),&ost);
  ostatok+=ost.ostg[3];  
 }
return(ostatok);
 
}

