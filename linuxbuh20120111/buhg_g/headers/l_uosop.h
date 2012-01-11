/*$Id: l_uosop.h,v 1.3 2011-01-13 13:49:51 sasa Exp $*/
/*28.12.2005	28.12.2005	Белых А.И.	l_uosop.h
Реквизиты для работы со списком операций подсистемы "Учёт услуг"
*/
class uosop_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str naim;    
  short vido; //0-внешняя 1-внутренняя
  short prov; //0-проводки нужно делать 1-не нужно
  short      metka_poi;  //0-нет поиска 1-есть поск
    
  uosop_rek()
   {
    clear();
   }
  
  void clear()
   {
    kod.new_plus("");
    naim.new_plus("");
    vido=0;
    prov=0;
    metka_poi=0;
   }
 
 };
