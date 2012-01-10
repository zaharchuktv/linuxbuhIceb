/*$Id: zarnvpw.c,v 1.1 2011-03-15 09:28:17 sasa Exp $*/
/*12.03.2011	12.03.2011	Белых А.И.	zarnvpw.c
Настройка выполнения проводок
*/
#include "buhg_g.h"

void zarnvpw(GtkWidget *wpredok)
{
int metka_prov=0;
class iceb_u_str stroka("");
if(iceb_poldan("Автоматическое выполнение проводок с использованием таблицы",&stroka,"zarnast.alx",wpredok) == 0)
 if(iceb_u_SRAV(stroka.ravno(),"Вкл",1) == 0)
  metka_prov=1;

if(metka_prov == 1)
 {
  iceb_f_redfil("zarnpr.alx",0,wpredok);
  return;
 }

int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Настройка проводок"));


zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Настройка проводок для начислений"));//0
punkt_m.plus(gettext("Настройка проводок для удержаний"));//1
punkt_m.plus(gettext("Настройка проводок для соц. начислений"));//2

while(nomer >= 0)
 {


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

  switch (nomer)
   {
    case -1:
      return;

    case 0:
      iceb_f_redfil("avtprozar1.alx",0,wpredok);
      break;

    case 1:
      iceb_f_redfil("avtprozar2.alx",0,wpredok);
      break;


    case 2:
      iceb_f_redfil("avtprozar3.alx",0,wpredok);
      break;


   }
   
 }


}
