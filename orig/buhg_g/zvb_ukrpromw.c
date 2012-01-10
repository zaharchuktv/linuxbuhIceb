/*$Id: zvb_ukrpromw.c,v 1.5 2011-01-13 13:49:56 sasa Exp $*/
/*20.10.2006	19.10.2006	Белых А.И.	zvb_ukrpromw.c
Выгрузка данных для зачисления на карт-счета для Укрпромбанка
*/
#include "buhg_g.h"
#include "l_nahud.h"
#include "zvb_ukrpromw.h"

int zvb_ukrpromw_m(class zvb_ukrpromw_rek *rek_poi,GtkWidget *wpredok);
void zvb_ukrpromw_r(short prn,const char *zapros,class zvb_ukrpromw_rek *rek,class l_nahud_rek *poisk,GtkWidget *wpredok);

void	zvb_ukrpromw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
class zvb_ukrpromw_rek data;

if(zvb_ukrpromw_m(&data,wpredok) != 0)
 return;


zvb_ukrpromw_r(prn,zapros,&data,poisk,wpredok);


}

