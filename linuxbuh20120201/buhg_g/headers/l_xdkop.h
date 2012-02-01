/*$Id: l_xdkop.h,v 1.3 2011-01-13 13:49:51 sasa Exp $*/
/*18.01.2006	18.01.2006	Белых А.И.	l_xdkop.h
Реквизиты для работы со списком операций подсистемы "Платёжные документы"
*/
class xdkop_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str naim;    
  short      metka_poi;  //0-нет поиска 1-есть поск
    
  xdkop_rek()
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
