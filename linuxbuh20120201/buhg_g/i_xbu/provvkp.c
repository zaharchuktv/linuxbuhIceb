/* $Id: provvkp.c,v 1.5 2011-02-21 07:35:56 sasa Exp $ */
/*11.11.2008	07.04.1998	Белых А.И.	prvkp.c
Проверка возможности корректировки или удаления проводки
Если вернули 0- работать можно
*/
#include        <stdio.h>
#include        <locale.h>
#include        <ctype.h>
#include        <string.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze


int provvkp(const char *kto,GtkWidget *wpredok)
{
 
if(iceb_u_SRAV(kto,gettext("МУ"),0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Материальный учет\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(1);
 } 
if(iceb_u_SRAV(kto,gettext("УОС"),0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Учет основных средств\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(2);
 } 
if(iceb_u_SRAV(kto,gettext("ЗП"),0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Заработная плата\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(3);
 } 

if(iceb_u_SRAV(kto,gettext("ПЛТ"),0) == 0 || \
iceb_u_SRAV(kto,gettext("ТРЕ"),0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Платежные документы\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(4);
 } 

if(iceb_u_SRAV(kto,gettext("КО"),0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Учет кассовых ордеров\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(5);
 } 

if(iceb_u_SRAV(kto,gettext("УСЛ"),0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Учет услуг\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(6);
 } 

if(iceb_u_SRAV(kto,gettext("УКР"),0) == 0)
 {
  iceb_u_str SOOB;
  SOOB.plus(gettext("С этой проводкой можно работать только из программы"));
  SOOB.ps_plus(gettext("\"Учет командировочных расходов\" !"));
  iceb_menu_soob(&SOOB,wpredok);
  return(6);
 } 

return(0);

}
