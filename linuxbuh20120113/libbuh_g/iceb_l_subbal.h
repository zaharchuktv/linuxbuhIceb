/*$Id: iceb_l_subbal.h,v 1.2 2011-01-13 13:49:59 sasa Exp $*/
/*28.04.2006	28.04.2006	Белых А.И.	iceb_l_subbal.h
Реквизиты для поиска записей в списке кодов суббалансов
*/
class iceb_l_subbal_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   iceb_l_subbal_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
