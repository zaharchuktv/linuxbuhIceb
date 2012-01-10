/*$Id: l_taxitel.h,v 1.2 2011-01-13 13:50:09 sasa Exp $*/
/*30.05.2009	30.05.2009	Белых А.И.	l_taxitel.h
Реквизиты для работы со списком телефонов
*/
class l_taxitel_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_taxitel_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
