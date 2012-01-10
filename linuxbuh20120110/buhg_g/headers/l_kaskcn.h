/*$Id: l_kaskcn.h,v 1.2 2011-01-13 13:49:50 sasa Exp $*/
/*29.09.2006	29.09.2006	Белых А.И.	l_kaskcn.h
Реквизиты для поиска записей в списке кодов целевого назначения
*/
class l_kaskcn_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_kaskcn_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
