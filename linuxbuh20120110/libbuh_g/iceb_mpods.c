/*$Id: iceb_mpods.c,v 1.5 2011-02-21 07:36:07 sasa Exp $*/
/*30.07.2008	22.04.2008	Белых А.И.	iceb_mpods.c
Получение по номеру подсистемы её символьной метки
*/
#include <string.h>
#include <libintl.h>

void iceb_mpods(int metka_ps,char *kto)
{

if(metka_ps == 1) /*Материальный учёт*/
  strcpy(kto,gettext("МУ"));

if(metka_ps == 2) /*зароботная плата*/
  strcpy(kto,gettext("ЗП"));

if(metka_ps == 3) /*учёт основных средств*/
  strcpy(kto,gettext("УОС"));

if(metka_ps == 4) /*платёжные поручения*/
  strcpy(kto,gettext("ПЛТ"));

if(metka_ps == 5) /*платёжные требования*/
  strcpy(kto,gettext("ТРЕ"));


if(metka_ps == 6) /*учёт услуг*/
  strcpy(kto,gettext("УСЛ"));

if(metka_ps == 7) /*учёт кассовых ордеров*/
  strcpy(kto,gettext("КО"));

if(metka_ps == 8) /*учёт командировочных расходов*/
  strcpy(kto,gettext("УКР"));

}
