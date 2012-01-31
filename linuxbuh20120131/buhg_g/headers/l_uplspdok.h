/*$Id: l_uplspdok.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*11.03.2008	11.03.2008	Белых А.И.	l_uplspdok.h
Реквизиты поиска путевых листов
*/
class uplspdok_rek
 {
  public:
  class iceb_u_str nomdok;
  class iceb_u_str datan;    
  class iceb_u_str datak;    
  class iceb_u_str kod_vod;    
  class iceb_u_str kod_avt;    
  class iceb_u_str kod_pod;  
  
  short      metka_poi;  //0-нет поиска 1-есть поск
  short      metka_pros; //0-все 1-не списано по объектам топливо
  
  uplspdok_rek()
   {
    clear();
   }
  
  void clear()
   {
    nomdok.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    kod_vod.new_plus("");
    kod_avt.new_plus("");
    kod_pod.new_plus("");
    metka_poi=0;
    metka_pros=0;
   }
 
 };
