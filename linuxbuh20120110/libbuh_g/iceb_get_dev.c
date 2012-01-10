/*$Id: iceb_get_dev.c,v 1.11 2011-02-21 07:36:07 sasa Exp $*/
/*10.12.2010	11.06.2005	Белых А.И.	iceb_get_dev.c
Подпрограмма чтения настроечного файла для определения 
на каком компъютере и на каком порте подключен считыватель
карточек
*/
#include <errno.h>
#include <stdlib.h>
#include "iceb_libbuh.h"
#include        <unistd.h>

class iceb_get_dev_data config_dev;
extern SQL_baza bd;

int iceb_get_dev(const char *imafil)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
int voz=1;
char nastr_dev[1024];
char bros[1024];

config_dev.workid_answer=workid();
if(config_dev.workid_answer == NULL)
 {
  iceb_menu_soob("iceb_get_dev-При чтении настроек считывателей карточек\nworkid вернул NULL !!!",NULL);
  return(1);
 }

printf("iceb_get_dev workid=%s\n",config_dev.workid_answer);

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imafil);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдены настройки %s",imafil);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],nastr_dev,sizeof(nastr_dev),1,'#') != 0)
   continue;
  if(iceb_u_SRAV(config_dev.workid_answer,nastr_dev,1) == 0)
   {
    iceb_u_polen(row_alx[0],nastr_dev,sizeof(nastr_dev),2,'#');

    iceb_u_polen(nastr_dev,bros,sizeof(bros),1,'|');
    config_dev.ima_komp.new_plus(bros);

    iceb_u_polen(nastr_dev,bros,sizeof(bros),2,'|');
    config_dev.name_dev_card.new_plus(bros);   

    iceb_u_polen(nastr_dev,bros,sizeof(bros),3,'|');
    config_dev.metka_screen=atoi(bros);   

    iceb_u_polen(nastr_dev,bros,sizeof(bros),4,'|');
    config_dev.metka_klav=atoi(bros);   

    iceb_u_polen(nastr_dev,bros,sizeof(bros),5,'|');
    config_dev.metka_mpr=atoi(bros);   

    voz=0;
    break;
   }
   
 }

return(voz);
}
