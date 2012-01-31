/*$Id: zvb_ibank2ua_endw.c,v 1.5 2011-01-13 13:49:56 sasa Exp $*/
/*20.11.2009	18.09.2008	Белых А.И.	zvb_ibank2ua_endw.c
Концовка формирования данных для зачисления на карт-счета для системы iBank2 UA
*/
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze


void zvb_ibank2ua_endw(double sumai,const char *vid_nah,FILE *ffr,FILE *ff,
GtkWidget *wpredok)
{
class iceb_fioruk_rk ruk;
class iceb_fioruk_rk glbuh;

iceb_fioruk(1,&ruk,wpredok);
iceb_fioruk(2,&glbuh,wpredok);


fprintf(ffr,"\
-------------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_u_kolbait(61,"Разом"),"Разом",sumai);


fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",ruk.fio.ravno(),glbuh.fio.ravno());
iceb_podpis(ffr,wpredok); 
fclose(ffr);

class iceb_u_str shet_bp("29242000000110");


fprintf(ff,"\
ONFLOW_TYPE=%s\n\
AMOUNT=%.2f\n\
PAYER_BANK_ACCOUNT=%s\n\
COMMISSION_PERCENT=0.00\n",
vid_nah,
sumai,
shet_bp.ravno());

fclose(ff);


}
