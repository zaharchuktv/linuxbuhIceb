/*$Id: l_ukrzat.h,v 1.2 2011-01-13 13:49:50 sasa Exp $*/
/*10.10.2006	10.10.2006	Белых А.И.	l_ukrzat.h
Реквизиты для поиска записей в списке командировочных расходов
*/
class l_ukrzat_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_ukrzat_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
