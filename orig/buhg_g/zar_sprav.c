/*$Id: zar_sprav.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*08.11.2006	08.11.2006	Белых А.И.	zar_sprav.c
Распечатка справод о зарплате
*/
#include "buhg_g.h"
#include "zar_sprav.h"

int zar_sprav_m(class zar_sprav_rek *rek,GtkWidget *wpredok);
int zar_sprav_r(class zar_sprav_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_sprav(int metka,
//Если metkah= 1 - расчет справки о зарплате
//               2 - расчет средней зарплаты для больничного
//               3 - расчет справки для уволенного для соцстраха
//               4 - расчет справок по датам в счет которых были сделаны начисления
//               5 - удалено !!!! расчет отпускных преподпвателя
//               6 - расчет отпускных
GtkWidget *wpredok)
{
static class zar_sprav_rek data;

data.metka_vr=metka;
  
data.imaf.free_class();
data.naimf.free_class();

if(zar_sprav_m(&data,wpredok) != 0)
 return;

if(zar_sprav_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
