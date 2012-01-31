/*$Id: l_uplpod.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*03.03.2008	03.03.2008	Белых А.И.	l_uplpod.h
Реквизиты для поиска записей в списке подразделений
*/
class l_uplpod_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_uplpod_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
