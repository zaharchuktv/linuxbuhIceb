/*$Id: imp_nu.c,v 1.7 2011-02-21 07:35:52 sasa Exp $*/
/*02.09.2007	09.10.2006	Белых А.И.	imp_nu.c
Импорт начислений/удержаний из файла
*/
#include "buhg_g.h"

int imp_nu_r(const char *imaf,short,short,int,int,GtkWidget *wpredok);


void imp_nu(int prn,int kod_nah_ud,short mz,short gz,GtkWidget *wpredok) //1-начисление 2-удержание
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");
class iceb_u_str naim;
if(prn == 1)
 naim.plus(gettext("Импорт начислений"));
if(prn == 2)
 naim.plus(gettext("Импорт удержаний"));
 
if(iceb_menu_import(&imaf,naim.ravno(),"",wpredok) != 0)
 return;

imp_nu_r(imaf.ravno(),mz,gz,prn,kod_nah_ud,wpredok);

}



