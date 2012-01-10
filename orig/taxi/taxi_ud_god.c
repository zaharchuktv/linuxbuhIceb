/*$Id: taxi_ud_god.c,v 1.3 2011-01-13 13:50:09 sasa Exp $*/
/*05.04.2005	04.04.2005	Белых А.И.	taxi_ud_god.c
Удаление данных за год
*/
#include "taxi.h"

void taxi_ud_god()
{

iceb_u_str repl;
iceb_u_str god;

repl.plus("Перед удалением данных убедитесь в том, что перенесено сальдо по клиентам !!!");
repl.ps_plus("Введите год за который нужно удалить данные.");

if(iceb_menu_vvod1(&repl,&god,5,NULL) != 0)
 return;

char strsql[500];
sprintf(strsql,"Удалить данные за %s год ? Вы уверены ?",god.ravno());

if(iceb_menu_danet(strsql,2,NULL) == 2)
 return;

//printf("****%s\n",god.ravno());

short godu=god.ravno_atoi();

sprintf(strsql,"delete from Taxizak where datvz >= '%04d-01-01' and datvz <= '%04d-12-31'",
godu,godu);
printf("%s\n",strsql);

iceb_sql_zapis(strsql,0,0,NULL);


}
