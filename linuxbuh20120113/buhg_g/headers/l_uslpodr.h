/*$Id: l_uslpodr.h,v 1.3 2011-01-13 13:49:51 sasa Exp $*/
/*17.07.2005	17.07.2005	Белых А.И.	uslpodr.h
Реквизиты для работы со списком групп услуг.
*/
class uslpodr_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str naim;    
  short      metka_poi;  //0-нет поиска 1-есть поск
    
  uslpodr_rek()
   {
    clear();
   }
  
  void clear()
   {
    kod.new_plus("");
    naim.new_plus("");
    metka_poi=0;
   }
 
 };
