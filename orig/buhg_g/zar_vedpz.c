/*$Id: zar_vedpz.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*08.12.2006	08.12.2006	Белых А.И.	zar_vedpz.c
Распечатка карточки с начислениями и удержаниями за период
*/
#include "buhg_g.h"
#include "zar_vedpz.h"

int zar_vedpz_m(class zar_vedpz_rek *rek,GtkWidget *wpredok);
int zar_vedpz_r(class zar_vedpz_rek *rek,GtkWidget *wpredok);


void zar_vedpz(GtkWidget *wpredok)
{
static class zar_vedpz_rek data;

  
data.imaf.free_class();
data.naimf.free_class();

if(zar_vedpz_m(&data,wpredok) != 0)
 return;

if(zar_vedpz_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
