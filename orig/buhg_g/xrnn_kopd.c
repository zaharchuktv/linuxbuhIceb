/*$Id: xrnn_kopd.c,v 1.4 2011-02-21 07:35:59 sasa Exp $*/
/*27.04.2008	27.04.2008	Белых А.И.	xrnn_kopd.c
Просмотр и выбор кодов операций для приходных документов
Если вернули 0-выбрали
             1-нет
*/

#include "buhg_g.h"


int xrnn_kopd(int metka, /*0-прибавлять через запятую 1-заменять на новое значение*/
class iceb_u_str *kodop,GtkWidget *wpredok)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Просмотр списка операций подсистемы \"Материальный учёт\""));//0
punkt_m.plus(gettext("Просмотр списка операций подсистемы \"Учёт услуг\""));//1
punkt_m.plus(gettext("Просмотр списка операций подсистемы \"Учёт основных средств\""));//2
punkt_m.plus(gettext("Просмотр списка видов командировок подистемы \"Учёт командировочных расходов\""));//3


int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,wpredok);

class iceb_u_str kod("");
class iceb_u_str naim("");

switch(nomer)
 {
  case 0:
   if(iceb_l_opmup(1,&kod,&naim,wpredok) == 0)
    {
     if(metka == 0)
      kodop->z_plus(kod.ravno());
     else 
      kodop->new_plus(kod.ravno());
     return(0);
    }
   break;

  case 1:
   if(l_uslopp(1,&kod,&naim,wpredok) == 0)
    {
     if(metka == 0)
      kodop->z_plus(kod.ravno());
     else 
      kodop->new_plus(kod.ravno());
     return(0);
    }
   break;

  case 2:
   if(l_uosopp(1,&kod,&naim,wpredok) == 0)
    {
     if(metka == 0)
      kodop->z_plus(kod.ravno());
     else 
      kodop->new_plus(kod.ravno());
     return(0);
    }
   break;

  case 3:
   if(l_ukrvk(1,&kod,&naim,wpredok) == 0)
    {
     if(metka == 0)
      kodop->z_plus(kod.ravno());
     else 
      kodop->new_plus(kod.ravno());
     return(0);
    }
   break;
 }
return(1);
}
