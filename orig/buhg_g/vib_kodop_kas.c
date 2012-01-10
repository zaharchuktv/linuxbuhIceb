/*$Id: vib_kodop_kas.c,v 1.5 2011-02-21 07:35:58 sasa Exp $*/
/*25.12.2007	24.01.2006	Белых А.И.	vib_kodop_kas.c
Выбор операции для подсистемы "Учёт кассовых ордеров"
*/
#include "buhg_g.h"


int vib_kodop_kas(iceb_u_str *kodop,GtkWidget *wpredok)
{
short nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр списка операций приходов"));
punkt_m.plus(gettext("Просмотр списка операций расходов"));

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);



iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

switch (nomer)
 {
  case -1:
    return(1);
    
  case 0:
    if(l_kasopp(1,&kod,&naim,wpredok) == 0)
      kodop->z_plus(kod.ravno());
     return(0);
    break;

  case 1:
    if(l_kasopr(1,&kod,&naim,wpredok) == 0)
      kodop->z_plus(kod.ravno());
     return(0);
    break;

 }
return(1);
}


