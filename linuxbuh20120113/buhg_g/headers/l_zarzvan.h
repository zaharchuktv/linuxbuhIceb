/*$Id: l_zarzvan.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*23.06.2006	23.06.2006	Белых А.И.	l_zarzvan.h
Реквизиты для работы со списком званий
*/
class l_zarzvan_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_zarzvan_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
