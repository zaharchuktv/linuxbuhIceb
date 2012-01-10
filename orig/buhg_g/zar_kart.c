/*$Id: zar_kart.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*07.12.2006	07.12.2006	Белых А.И.	zar_kart.c
Распечатка карточки с начислениями и удержаниями за период
*/
#include "buhg_g.h"
#include "zar_kart.h"

int zar_kart_m(class zar_kart_rek *rek,GtkWidget *wpredok);
int zar_kart_r(class zar_kart_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_kart(GtkWidget *wpredok)
{
static class zar_kart_rek data;

  
data.imaf.free_class();
data.naimf.free_class();

if(zar_kart_m(&data,wpredok) != 0)
 return;

if(zar_kart_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
