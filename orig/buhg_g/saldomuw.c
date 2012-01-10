/*$Id: saldomuw.c,v 1.6 2011-03-09 08:21:24 sasa Exp $*/
/*25.02.2009	25.02.2009	Белых А.И.	saldomuw.c
Перенос сальдо для подсистемы "Материальный учёт"
*/
#include <stdlib.h>
#include "buhg_g.h"
#include "saldo.h"

int saldomuw_r(short godn,short godk,GtkWidget *wpredok);


void saldomuw(GtkWidget *wpredok)
{
class saldo_m_rek data;

if(saldo_m(&data,wpredok) != 0)
 return;

if(iceb_pbpds(1,data.datak.ravno_atoi(),wpredok) != 0)
 return;

saldomuw_r(data.datan.ravno_atoi(),data.datak.ravno_atoi(),wpredok);

}
