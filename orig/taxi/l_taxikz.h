/*$Id: l_taxikz.h,v 1.2 2011-01-13 13:50:09 sasa Exp $*/
/*22.06.2006	22.06.2006	Белых А.И.	l_taxikz.h
Реквизиты для работы со списком категорий
*/
class l_taxikz_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_taxikz_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
