/*$Id: clprov_m.c,v 1.9 2011-02-21 07:35:51 sasa Exp $*/
/*01.08.2008	13.04.2004	Белых А.И.	clprov_m.c
Удаление проводок
*/
#include        <stdlib.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

void clprov_r(int metka_r,short mu,short gu);


void clprov_m()
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Удаление проводок"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Удаление проводок за месяц"));//0
punkt_m.plus(gettext("Удаление проводок за год"));//1


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

iceb_u_str repl;
iceb_u_str data;
short m,g;

switch(nomer)
 {
  case -1:
   return;


  case 0:
    repl.new_plus(gettext("Удаление проводок за месяц"));
    repl.ps_plus(gettext("Введите дату"));
    repl.plus(" ");
    repl.plus(gettext("(м.р)"));
    if(iceb_menu_vvod1(&repl,&data,8,NULL) != 0)
     return;
    if(iceb_u_rsdat1(&m,&g,data.ravno()) != 0)
     {
      repl.new_plus(gettext("Не верно введена дата !"));
      iceb_menu_soob(&repl,NULL);
      return;
     }

    break;

  case 1:
    repl.new_plus(gettext("Удаление проводок за год"));
    repl.ps_plus(gettext("Введите год"));
    if(iceb_menu_vvod1(&repl,&data,8,NULL) != 0)
     return;
    g=atoi(data.ravno());    
    break;
     
 }
if(data.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели дату!"),NULL);
  return;
 }

repl.new_plus(gettext("Удалить проводки за"));
repl.plus(" ");
repl.plus(data.ravno());
if(iceb_menu_danet(&repl,2,NULL) == 2)
 return;

if(iceb_parol(0,NULL) != 0)
  return;

clprov_r(nomer,m,g);
  
}
