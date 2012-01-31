/*$Id: l_tippl.h,v 1.2 2011-01-13 13:49:50 sasa Exp $*/
/*12.05.2006	12.05.2006	Белых А.И.	l_tippl.h
Реквизиты для поиска в списке типовых документов
*/
class tippl_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str naim;    
  short      metka_poi;  //0-нет поиска 1-есть поск
    
  tippl_rek()
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

