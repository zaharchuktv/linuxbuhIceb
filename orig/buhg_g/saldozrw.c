/*$Id: saldozrw.c,v 1.6 2011-03-09 08:21:24 sasa Exp $*/
/*21.02.2011	26.02.2009	Белых А.И.	saldozrw.c
Перенос сальдо для подсистемы "Заработная плата"
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "saldo.h"

int saldozrw_r(short godn,short godk,GtkWidget *wpredok);

void saldozrw()
{
class saldo_m_rek data;

if(saldo_m(&data,NULL) != 0)
 return;

if(iceb_pbpds(1,data.datak.ravno_atoi(),NULL) != 0)
 return;

saldozrw_r(data.datan.ravno_atoi(),data.datak.ravno_atoi(),NULL);
 
}
