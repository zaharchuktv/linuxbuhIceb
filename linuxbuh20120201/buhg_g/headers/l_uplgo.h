/*$Id: l_uplgo.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*09.03.2008	09.03.2008	Белых А.И.	l_uplgo.h
Реквизиты для поиска записей в списке групп объектов списания топлива
*/
class l_uplgo_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_uplgo_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
