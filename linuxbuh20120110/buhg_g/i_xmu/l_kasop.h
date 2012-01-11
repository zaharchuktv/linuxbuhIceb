/*$Id: l_kasop.h,v 1.7 2011-01-13 13:49:50 sasa Exp $*/
/*29.09.2006	18.01.2006	Белых А.И.	l_kasop.h
Реквизиты для работы со списком операций подсистемы "Учёт услуг"
*/
class kasop_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str naim;    
  iceb_u_str shet;  //Счёт
  iceb_u_str shetk; //Счёт корреспондент
  iceb_u_str kod_cn;
  short prov; //0-проводки не нужно делать 1-нужно

  short      metka_poi;  //0-нет поиска 1-есть поск
    
  kasop_rek()
   {
    clear();
   }
  
  void clear()
   {
    kod.new_plus("");
    naim.new_plus("");
    shet.new_plus("");
    shetk.new_plus("");
    kod_cn.new_plus("");
    metka_poi=0;
    prov=0;
   }
 
 };
