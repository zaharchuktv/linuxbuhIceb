/*$Id: doocw.c,v 1.5 2011-01-13 13:49:49 sasa Exp $*/
/*11.11.2008	20.10.2004	Белых А.И.	doocw.c
Дооценка документа
*/
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "dooc.h"

iceb_u_str dooc_data::datad;
iceb_u_str dooc_data::sumadooc;
iceb_u_str dooc_data::osnov;
iceb_u_str dooc_data::kodmat;

int   dooc_m(class dooc_data *rek_ras,GtkWidget *wpredok);
void dooc_r(class dooc_data *rek_ras,GtkWidget *wpredok);

class dooc_data data_dooc;

void doocw(short dd,short md,short gd,
int tipz,
const char *nomdok,
int skl,
GtkWidget *wpredok)
{


data_dooc.tipz=tipz;
data_dooc.skl=skl;
data_dooc.nomdok_c.new_plus(nomdok);
data_dooc.dd=dd;
data_dooc.md=md;
data_dooc.gd=gd;


if(dooc_m(&data_dooc,wpredok) != 0)
 return;

dooc_r(&data_dooc,wpredok);

}
