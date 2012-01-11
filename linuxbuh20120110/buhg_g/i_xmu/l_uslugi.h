/*$Id: l_uslugi.h,v 1.4 2011-02-21 07:35:54 sasa Exp $*/
/*04.02.2011	19.07.2005	Белых А.И.	uslugi.h
Реквизиты для работы со списком групп услуг.
*/
class uslugi_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;    
   class iceb_u_str grupa;
   class iceb_u_str ei;
   class iceb_u_str cena;
   class iceb_u_str shet;
   class iceb_u_str artikul;
   int metka_cen;  //0-без НДС 1-с НДС

  short      metka_poi;  //0-нет поиска 1-есть поиск
    
  uslugi_rek()
   {
    clear();
   }
  
  void clear()
   {
    artikul.new_plus("");
    kod.new_plus("");
    naim.new_plus("");
    grupa.new_plus("");
    ei.new_plus("");
    cena.new_plus("");
    shet.new_plus("");
    metka_cen=0;
    metka_poi=0;
   }
 
 };
