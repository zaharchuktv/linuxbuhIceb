/*$Id: copdokw.c,v 1.8 2011-01-13 13:49:49 sasa Exp $*/
/*11.11.2008	30.09.2004	Белых А.И.	copdokw.c
Программа копирования записей 
*/
#include "buhg_g.h"
#include "copdok.h"

int   copdok_m(class copdok_data *rek_ras,GtkWidget*);
void copdok_r(class copdok_data *rek_ras,GtkWidget *wpredok);

int copdokw(short dd,short md,short gd,const char *nomdok,int skl,int tipz,GtkWidget *wpredok)
{
static class copdok_data data;

data.tipz=tipz;
data.skl=skl;
data.nomdok_c.new_plus(nomdok);
data.dd=dd;
data.md=md;
data.gd=gd;

if(copdok_m(&data,wpredok) != 0)
 return(1);

copdok_r(&data,wpredok);

return(0);

}
