/*$Id: l_uplost.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*09.03.2008	09.03.2008	Белых А.И.	l_uplost.h
Реквизиты для поиска записей в списке объектов списания топлива
*/
class l_uplost_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str kod_gr;
   class iceb_u_str shet;   
   l_uplost_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    kod_gr.new_plus("");
    shet.new_plus("");
   }
 
 };
