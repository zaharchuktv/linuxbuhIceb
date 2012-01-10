/*$Id: rnnvvd.c,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*03.12.2009	03.12.2009	Белых А.И.	rnnvvd.c
Меню для выбора вида документа для "Реестра налоговых накладных"
*/
#include "buhg_g.h"

extern char *organ;

int rnnvvd(class iceb_u_str *viddok,GtkWidget *wpredok)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus("Вибір");


zagolovok.plus(organ,40);
zagolovok.ps_plus("Выберіть потрібне");

punkt_m.plus("ПН  податкова накладна");
punkt_m.plus("РК  розсчёт коригування до податкової накладної");
punkt_m.plus("ВМД вантажна митна декларация"); 
punkt_m.plus("ЧК  товарний (касовий) чек");
punkt_m.plus("ТК  транспортний квиток");
punkt_m.plus("ГР  готельний счёт");
punkt_m.plus("ПЗ  счёт за послуги зв'зку");
punkt_m.plus("ЗО  послуги вартість яких визначається за показниками приладів обліку");
punkt_m.plus("ЗП  заява платника відповідно до підпункту 7.2.6 пункту 7.2 статті 7 Закону");

int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

if(nomer >= 0)
 {
  iceb_u_polen(punkt_m.ravno(nomer),viddok,1,' ');
  return(0);
 } 
return(1);
}
