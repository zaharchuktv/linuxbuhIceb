/*$Id: l_ukrvk.h,v 1.2 2011-01-13 13:49:50 sasa Exp $*/
/*18.02.2008	18.02.2008	Белых А.И.	l_ukrvk.h
Реквизиты для поиска записей в списке видов командировок
*/
class l_ukrvk_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_ukrvk_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
