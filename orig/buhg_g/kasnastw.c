/* $Id: kasnastw.c,v 1.8 2011-02-21 07:35:52 sasa Exp $ */
/*14.12.2010	220.09.2000	Белых А.И.	kasnastw.c
Чтение настроек для программы учета кассовых ордеров
Если вернули 0-файл прочитан
	     1-нет
*/
#include <stdlib.h>
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"

short		vplsh; /*0-двух порядковый план счетов 1-мнопорядковый*/
short		startgodb; /*Стартовый год главной книги*/
extern SQL_baza bd;

int kasnastw()
{
char            st1[1024];


vplsh=startgodb=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='nastrb.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"nastrb.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  
  iceb_u_polen(row_alx[0],st1,sizeof(st1),1,'|');
  if(iceb_u_SRAV(st1,"Многопорядковый план счетов",0) == 0)
   {
    iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
    if(iceb_u_SRAV(st1,"Вкл",1) == 0)
      vplsh=1;
    continue;
   }
  if(iceb_u_SRAV(st1,"Стартовый год",0) == 0)
   {
    iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
    startgodb=atoi(st1); 
    continue;
   }

 }
return(0);
}
