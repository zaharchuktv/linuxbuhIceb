/*$Id: l_uplsal.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*07.03.2008	07.03.2008	Белых А.И.	l_uplsal.h
Реквизиты для поиска записей в списке сальдо по топливу
*/
class l_uplsal_rek
 {
  public:
   class iceb_u_str god;
   class iceb_u_str kod_vod;
   class iceb_u_str kod_avt;
   class iceb_u_str kod_top;
   class iceb_u_str saldo;
   l_uplsal_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    god.new_plus("");
    kod_vod.new_plus("");
    kod_avt.new_plus("");
    kod_top.new_plus("");
    saldo.new_plus("");
   }
 
 };
