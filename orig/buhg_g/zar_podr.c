/*$Id: zar_podr.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*29.01.2007	29.01.2007	Белых А.И.	zar_podr.c
Расчёт свода по подразделениям
*/
#include "buhg_g.h"
#include "zar_podr.h"

int zar_podr_m(int,class zar_podr_rek *rek,GtkWidget *wpredok);
int zar_podr1_r(class zar_podr_rek *rek,GtkWidget *wpredok);
int zar_podr2_r(class zar_podr_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_podr(int metka_r, //0-форма 1 1-форма 2
GtkWidget *wpredok)
{
static class zar_podr_rek data;

if(data.datan.getdlinna() <= 1)
 {
  data.datan.plus(mmm);
  data.datan.plus(".");
  data.datan.plus(ggg);
  
 }
  
data.imaf.free_class();
data.naimf.free_class();

if(zar_podr_m(metka_r,&data,wpredok) != 0)
 return;

if(metka_r == 0)
 if(zar_podr1_r(&data,wpredok) != 0)
   return;
if(metka_r == 1)
 if(zar_podr2_r(&data,wpredok) != 0)
   return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
