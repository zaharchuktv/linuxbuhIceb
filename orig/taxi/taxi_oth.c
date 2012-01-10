/*$Id: taxi_oth.c,v 1.16 2011-02-21 07:36:21 sasa Exp $*/
/*12.03.2005	08.12.2003	Белых А.И.	taxi_oth.c
Отчет по водителям и операторам за любой период времени
*/

#include        <time.h>
#include        <stdlib.h>
#include        <unistd.h>
#include "taxi.h"
#include "taxi_oth.h"

iceb_u_str taxi_oth_data::datn;
iceb_u_str taxi_oth_data::datk;
iceb_u_str taxi_oth_data::vremn;
iceb_u_str taxi_oth_data::vremk;
iceb_u_str taxi_oth_data::kodvod;
iceb_u_str taxi_oth_data::kodop;
iceb_u_str taxi_oth_data::kodzav;
iceb_u_str taxi_oth_data::kodklienta;
int  taxi_oth_v(class taxi_oth_data *data);
int  taxi_oth_r(class taxi_oth_data *data);

//extern SQL_baza	bd;
extern char *name_system;

void taxi_oth()
{
taxi_oth_data data;
if(data.kodklienta.getdlinna() == 0)
 data.clear_zero();
 
if(taxi_oth_v(&data) != 0)
 {
  taxi_oth_r(&data);
//  printf("Запуск работы с распечатками.\n");
  iceb_rabfil(&data.imafil,&data.nameoth,"",0,NULL);
 }
}
