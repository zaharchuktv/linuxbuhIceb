/*$Id: saldogkw.c,v 1.7 2011-03-09 08:21:24 sasa Exp $*/
/*13.10.2010	25.02.2009	Белых А.И.	saldogkw.c
Перенос сальдо для подсистемы "Главная книга"
*/
#include <stdlib.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "saldo.h"

int saldogk_r(short godn,short godk,GtkWidget *wpredok);


void saldogkw(GtkWidget *wpredok)
{
class saldo_m_rek data;

if(saldo_m(&data,wpredok) != 0)
 return;

if(iceb_pbpds(1,data.datak.ravno_atoi(),wpredok) != 0)
 return;

saldogk_r(data.datan.ravno_atoi(),data.datak.ravno_atoi(),wpredok);

}
