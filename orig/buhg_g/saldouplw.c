/*$Id: saldouplw.c,v 1.5 2011-02-21 07:35:57 sasa Exp $*/
/*27.02.2009	27.02.2009	Белых А.И.	saldouplw.c
Перенос сальдо для подсистемы "Материальный учёт"
*/
#include <stdlib.h>
#include "buhg_g.h"
#include "saldo.h"

int saldouplw_r(short godn,short godk,GtkWidget *wpredok);


void saldouplw()
{
class saldo_m_rek data;

if(saldo_m(&data,NULL) != 0)
 return;

saldouplw_r(data.datan.ravno_atoi(),data.datak.ravno_atoi(),NULL);

}
