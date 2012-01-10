/*$Id: ud_zap_vv.c,v 1.4 2011-02-21 07:36:21 sasa Exp $*/
/*17.08.2006	17.08.2006	Белых А.И.	ud_zap_vv.c
Удаление записей на вход/выход
*/

#include "i_rest.h"

extern SQL_baza bd;

void ud_zap_vv()
{
class iceb_u_str datud;
datud.plus("");

if(iceb_menu_vvod1(gettext("Введите дату до которй удалить все записи"),&datud,11,NULL) != 0)
 return;
if(datud.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата !"),NULL);
  return;
 }
printf("datud=%s\n",datud.ravno());

char strsql[300]; 
sprintf(strsql,"delete from Restvv where dv < '%s 00:00:00'",datud.ravno_sqldata());

printf("%s\n",strsql);

iceb_sql_zapis(strsql,1,0,NULL);
int kolud=sql_rows(&bd);

sprintf(strsql,"optimize table Restvv");
iceb_sql_readkey(strsql,NULL);

sprintf(strsql,"%s:%d",gettext("Количество удалённых записей"),kolud);
iceb_menu_soob(strsql,NULL);

}

