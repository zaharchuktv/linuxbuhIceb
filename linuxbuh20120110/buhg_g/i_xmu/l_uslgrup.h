/*$Id: l_uslgrup.h,v 1.4 2011-01-13 13:49:51 sasa Exp $*/
/*17.07.2005	17.07.2005	Белых А.И.	uslgrup.h
Реквизиты для работы со списком групп услуг.
*/
class uslgrup_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str naim;    
  short      metka_poi;  //0-нет поиска 1-есть поск
    
  uslgrup_rek()
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
