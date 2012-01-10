/*$Id: matuddokw.c,v 1.5 2011-02-21 07:35:55 sasa Exp $*/
/*06.05.2005	01.05.2005	Белых А.И.	matuddokw.c
Удаление документов за год
*/


#include "buhg_g.h"

int matuddokw_r(short,GtkWidget*);


void matuddokw()
{

iceb_u_str repl;
repl.plus(gettext("\
В н и м а н и е !!!\n\
Режим удаления документов за год.\n\
Удаление документов за прошедшие годы выполняется с целью уменьшения\n\
объёма базы данных."));

repl.ps_plus(gettext("Введите год"));
iceb_u_str god;
god.plus("");
if(iceb_menu_vvod1(&repl,&god,5,NULL) != 0)
 return;

if(iceb_parol(0,NULL) != 0)
 return;



matuddokw_r((short)god.ravno_atoi(),NULL);

}
