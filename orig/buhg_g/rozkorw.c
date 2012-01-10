/*$Id: rozkorw.c,v 1.6 2011-01-13 13:49:52 sasa Exp $*/
/*11.11.2008	20.10.2004	Белых А.И.	rozkorw.c
Оформление возврата
*/
#include "buhg_g.h"
#include "rozkor.h"

iceb_u_str rozkor_data::nomer_ras;
iceb_u_str rozkor_data::nomer_nal_nak;
iceb_u_str rozkor_data::kto_v_ras;
iceb_u_str rozkor_data::prihina;
iceb_u_str rozkor_data::nomer_dogovora;
iceb_u_str rozkor_data::data_dogovora;
iceb_u_str rozkor_data::god_nal_nak;

int   rozkor_m(class rozkor_data *rek_ras,GtkWidget *wpredok);
void rozkor_r(class rozkor_data *rek_ras,GtkWidget *wpredok);

class rozkor_data data_rozkor;

void rozkorw(short dd,short md,short gd,
int tipz,
const char *nomdok,
int skl,
float pnds,
GtkWidget *wpredok)
{


data_rozkor.tipz=tipz;
data_rozkor.skl=skl;
data_rozkor.nomdok_c.new_plus(nomdok);
data_rozkor.dd=dd;
data_rozkor.md=md;
data_rozkor.gd=gd;
data_rozkor.pnds=pnds;

if(rozkor_m(&data_rozkor,wpredok) != 0)
 return;

rozkor_r(&data_rozkor,wpredok);

}
