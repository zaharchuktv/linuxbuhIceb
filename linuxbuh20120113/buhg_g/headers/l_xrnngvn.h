/*$Id: l_xrnngvn.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*14.04.2008	14.04.2008	Белых А.И.	l_xrnngvn.h
Реквизиты для поиска записей в списке групп выданных налоговых накладных
*/
class l_xrnngvn_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_xrnngvn_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
