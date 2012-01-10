/*$Id: spizw.c,v 1.6 2011-01-13 13:49:53 sasa Exp $*/
/*20.10.2009	17.05.2005	Белых А.И.	spizw.c
Списание изделий в материальном учёте
*/


#include "buhg_g.h"
#include "spizw.h"

int spizw_m(class spizw_rr *rek_ras,GtkWidget*);
int spizw_r(class spizw_rr *datark,GtkWidget *wpredok);
/********
iceb_u_str spizw_rr::datas;
iceb_u_str spizw_rr::sklad;
iceb_u_str spizw_rr::koliz;
iceb_u_str spizw_rr::nomdok;
************/
void spizw(const char *kod_izdel,GtkWidget *wpredok)
{
class spizw_rr data;
data.kod_izdel.new_plus(kod_izdel);

if(spizw_m(&data,wpredok) != 0)
 return;

spizw_r(&data,wpredok);

}
