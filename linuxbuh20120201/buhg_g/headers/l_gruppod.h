/*$Id: l_gruppod.h,v 1.2 2011-01-13 13:49:50 sasa Exp $*/
/*21.06.2006	21.06.2006	Белых А.И.	l_gruppod.h
Реквизиты для поиска записей в списке групп подразделений
*/
class l_gruppod_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_gruppod_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
