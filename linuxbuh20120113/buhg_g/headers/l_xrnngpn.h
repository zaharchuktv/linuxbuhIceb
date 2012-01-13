/*$Id: l_xrnngpn.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*14.04.2008	14.04.2008	Белых А.И.	l_xrnngpn.h
Реквизиты для поиска записей в списке групп полученных налоговых накладных
*/
class l_xrnngpn_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_xrnngpn_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
