/*$Id: zar_srninu.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*29.11.2006	29.11.2006	Белых А.И.	zar_srninu.c
Расчёт списка работников не имеющих начисления/удержания
*/
#include "buhg_g.h"
#include "zar_srninu.h"

int zar_srninu_m(class zar_srninu_rek *rek,GtkWidget *wpredok);
int zar_srninu_r(class zar_srninu_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_srninu(GtkWidget *wpredok)
{
static class zar_srninu_rek data;

if(data.datan.getdlinna() <= 1)
 {
  data.datan.plus(mmm);
  data.datan.plus(".");
  data.datan.plus(ggg);
  
 }
  
data.imaf.free_class();
data.naimf.free_class();

if(zar_srninu_m(&data,wpredok) != 0)
 return;

if(zar_srninu_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
