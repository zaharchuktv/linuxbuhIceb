/*$Id: l_uslop.h,v 1.4 2011-01-13 13:49:51 sasa Exp $*/
/*29.10.2007	28.07.2005	Белых А.И.	l_uslop.h
Реквизиты для работы со списком операций подсистемы "Учёт услуг"
*/
class uslop_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str naim;    
  iceb_u_str shet;
  short vido; //0-внешняя 1-внутренняя 2-смена стоимости
  short prov; //0-проводки нужно делать 1-не нужно
  short      metka_poi;  //0-нет поиска 1-есть поск
    
  uslop_rek()
   {
    clear();
   }
  
  void clear()
   {
    kod.new_plus("");
    naim.new_plus("");
    shet.new_plus("");
    vido=0;
    prov=0;
    metka_poi=0;
   }
 
 };
