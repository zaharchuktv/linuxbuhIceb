/*$Id: zar_bpom.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*08.12.2006	08.12.2006	Белых А.И.	zar_bpom.c
Распечатка списка работников имеющих благотворительную помощь
*/
#include "buhg_g.h"
#include "zar_bpom.h"

int zar_bpom_m(class zar_bpom_rek *rek,GtkWidget *wpredok);
int zar_bpom_r(class zar_bpom_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_bpom()
{
static class zar_bpom_rek data;

  
data.imaf.free_class();
data.naimf.free_class();

if(zar_bpom_m(&data,NULL) != 0)
 return;

if(zar_bpom_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
