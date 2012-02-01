/*$Id: l_zarsb.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*10.09.2009	10.09.2009	Белых А.И.	l_zarsb.h
Реквизиты для работы со списком банков
*/
class l_zarsb_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_zarsb_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
