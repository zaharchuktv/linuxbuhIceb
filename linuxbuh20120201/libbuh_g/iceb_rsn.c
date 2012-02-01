/*$Id: iceb_rsn.c,v 1.9 2011-02-21 07:36:07 sasa Exp $*/
/*10.12.2010	09.04.2004	Белых А.И.	iceb_rsn.c
Чтение системных настроек
*/
#include <errno.h>
#include <unistd.h>
#include "iceb_libbuh.h"

extern SQL_baza bd;

int  iceb_rsn(class SYSTEM_NAST *data,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];


char bros[312];
char nazvanie[512];
//Читаем необходимые настройки
sprintf(strsql,"select str from Alx where fil='nastsys.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдены настройки nastsys.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }



while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],nazvanie,sizeof(nazvanie),1,'|') != 0)
    continue;
    
  if(iceb_u_SRAV(nazvanie,"Системный принтер",0) == 0)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    data->prog_peh_sys.new_plus(bros);
    continue;
   }

  if(iceb_u_SRAV(nazvanie,"Снять с очереди на печать",0) == 0)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    data->clear_peh.new_plus(bros);
    continue;
   }

  if(iceb_u_SRAV(nazvanie,"Редактор для графического интерфейса",0) == 0)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    data->redaktor.new_plus(bros);
    continue;
   }

  if(iceb_u_SRAV(nazvanie,"Просмотрщик файлов для графического интерфейса",0) == 0)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    data->prosmotr.new_plus(bros);
    continue;
   }

  if(iceb_u_SRAV(nazvanie,"Просмотр очереди на печать",0) == 0)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
    data->prosop.new_plus(bros);
    continue;
   }

 }

return(0);

}
