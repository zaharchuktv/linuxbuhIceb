/*$Id: rnnvdw.c,v 1.4 2011-03-09 08:21:24 sasa Exp $*/
/*19.07.2010	12.02.2010	Белых А.И.	rnnvdw.c
Определения цифрового кода для вида документа
*/
#include "buhg_g.h"

int rnnvdw(int tipz,const char *viddok)
{

if(tipz == 1) /*приходные налоговые накладные*/
 {
  if(iceb_u_SRAV("ПН",viddok,0) == 0)
   return(1);
  if(iceb_u_SRAV("ВМД",viddok,0) == 0)
   return(2);
  if(iceb_u_SRAV("ЧК",viddok,0) == 0)
   return(3);
  if(iceb_u_SRAV("ПНУ",viddok,0) == 0)
   return(4);
  if(iceb_u_SRAV("ПНУ",viddok,0) == 0)
   return(5);
  if(iceb_u_SRAV("РК",viddok,0) == 0)
   return(6);
  if(iceb_u_SRAV("РКУ",viddok,0) == 0)
   return(7);
  if(iceb_u_SRAV("ВМДУ",viddok,0) == 0)
   return(8);
  if(iceb_u_SRAV("ЧКУ",viddok,0) == 0)
   return(9);
  if(iceb_u_SRAV("ТК",viddok,0) == 0)
   return(10);
  if(iceb_u_SRAV("ТКУ",viddok,0) == 0)
   return(11);
  if(iceb_u_SRAV("ГР",viddok,0) == 0)
   return(12);
  if(iceb_u_SRAV("ГРУ",viddok,0) == 0)
   return(13);
  if(iceb_u_SRAV("ПЗ",viddok,0) == 0)
   return(14);
  if(iceb_u_SRAV("ПЗУ",viddok,0) == 0)
   return(15);
  if(iceb_u_SRAV("ПО",viddok,0) == 0)
   return(16);
  if(iceb_u_SRAV("ПОУ",viddok,0) == 0)
   return(17);
  if(iceb_u_SRAV("ЗП",viddok,0) == 0)
   return(18);
  if(iceb_u_SRAV("ЗПУ",viddok,0) == 0)
   return(19);
  if(iceb_u_SRAV("НП",viddok,0) == 0)
   return(20);
  if(iceb_u_SRAV("НПУ",viddok,0) == 0)
   return(21);
  if(iceb_u_SRAV("ПНР",viddok,0) == 0)
   return(22);
  if(iceb_u_SRAV("РКР",viddok,0) == 0)
   return(23);
  if(iceb_u_SRAV("ВМДР",viddok,0) == 0)
   return(24);
  if(iceb_u_SRAV("ЧКР",viddok,0) == 0)
   return(25);
  if(iceb_u_SRAV("ТКР",viddok,0) == 0)
   return(26);
  if(iceb_u_SRAV("ГРР",viddok,0) == 0)
   return(27);
  if(iceb_u_SRAV("ПЗР",viddok,0) == 0)
   return(28);
  if(iceb_u_SRAV("ПОР",viddok,0) == 0)
   return(29);
  if(iceb_u_SRAV("ЗПР",viddok,0) == 0)
   return(30);
  if(iceb_u_SRAV("НПР",viddok,0) == 0)
   return(31);
 }

if(tipz == 2) /*расходные налоговые накладные*/
 {
  if(iceb_u_SRAV("ПН",viddok,0) == 0)
   return(1);
  if(iceb_u_SRAV("ВМД",viddok,0) == 0)
   return(2);
  if(iceb_u_SRAV("ПНУ",viddok,0) == 0)
   return(5);
  if(iceb_u_SRAV("РК",viddok,0) == 0)
   return(6);
  if(iceb_u_SRAV("РКУ",viddok,0) == 0)
   return(7);
  if(iceb_u_SRAV("ВМДУ",viddok,0) == 0)
   return(8);
  if(iceb_u_SRAV("ПП",viddok,0) == 0)
   return(9);
  if(iceb_u_SRAV("ППУ",viddok,0) == 0)
   return(10);
  if(iceb_u_SRAV("ЗЦ",viddok,0) == 0)
   return(11);
  if(iceb_u_SRAV("ЗЦУ",viddok,0) == 0)
   return(12);
  if(iceb_u_SRAV("БО",viddok,0) == 0)
   return(13);
  if(iceb_u_SRAV("БОУ",viddok,0) == 0)
   return(14);
  if(iceb_u_SRAV("ПН01",viddok,0) == 0)
   return(15);
  if(iceb_u_SRAV("ПН02",viddok,0) == 0)
   return(16);
  if(iceb_u_SRAV("ПН03",viddok,0) == 0)
   return(17);
  if(iceb_u_SRAV("ПН04",viddok,0) == 0)
   return(18);
  if(iceb_u_SRAV("ПН05",viddok,0) == 0)
   return(19);
  if(iceb_u_SRAV("ПН06",viddok,0) == 0)
   return(20);
  if(iceb_u_SRAV("ПН07",viddok,0) == 0)
   return(21);
  if(iceb_u_SRAV("ПН08",viddok,0) == 0)
   return(22);
  if(iceb_u_SRAV("ПН09",viddok,0) == 0)
   return(23);
  if(iceb_u_SRAV("ПН10",viddok,0) == 0)
   return(24);
  if(iceb_u_SRAV("ПН11",viddok,0) == 0)
   return(25);

 }
return(0);
}