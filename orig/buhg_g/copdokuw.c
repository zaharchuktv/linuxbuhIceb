/*$Id: copdokuw.c,v 1.5 2011-01-13 13:49:49 sasa Exp $*/
/*17.06.2006	30.09.2004	Белых А.И.	copdokuw.c
Программа копирования записей в учёте услуг
*/
#include "buhg_g.h"
#include "copdokus.h"



int   copdoku_m(class copdokus_data *rek_ras,GtkWidget*);
void copdoku_r(class copdokus_data *rek_ras,GtkWidget *wpredok);

int copdokuw(short dd,short md,short gd,const char *nomdok,int pod,int tipz,GtkWidget *wpredok)
{
static class copdokus_data data;

data.tipz=tipz;
data.pod=pod;
data.nomdok_c.new_plus(nomdok);
data.dd=dd;
data.md=md;
data.gd=gd;

if(copdoku_m(&data,wpredok) != 0)
 return(1);

copdoku_r(&data,wpredok);

return(0);

}
