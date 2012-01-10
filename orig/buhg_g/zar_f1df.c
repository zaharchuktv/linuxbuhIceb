/*$Id: zar_f1df.c,v 1.6 2011-01-13 13:49:55 sasa Exp $*/
/*19.11.2009	20.12.2006	Белых А.И.	zar_f1df.c
Расчёт формы 1ДФ
*/
#include "buhg_g.h"
#include "zar_f1df.h"

int zar_f1df_m(class zar_f1df_rek *rek,GtkWidget *wpredok);
int zar_f1df_r(class zar_f1df_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_f1df(GtkWidget *wpredok)
{


static class zar_f1df_rek data;

  
data.imaf.free_class();
data.naimf.free_class();

if(zar_f1df_m(&data,wpredok) != 0)
 return;

if(zar_f1df_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
