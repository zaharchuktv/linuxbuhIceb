/*$Id: zvb_hreshatik_endw.c,v 1.4 2011-01-13 13:49:56 sasa Exp $*/
/*20.11.2009	23.05.2007	Белых А.И.	zvb_hreshatik_endw.c
Концовка формирования данных для зачисления на карт-счета для банка Крещатик
*/
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze


void zvb_hreshatik_endw(double sumai,int kolzap,FILE *ffr,FILE *ff,
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



fprintf(ff,"99%010d%012.2f%012.2f\n",kolzap,sumai,sumai);
fclose(ff);


}
