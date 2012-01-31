/*$Id: l_zargni.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*23.06.2006	23.06.2006	Белых А.И.	l_zargni.h
Реквизиты для работы со списком удержаний
*/
class l_zargni_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str kontr;

   l_zargni_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    kontr.new_plus("");
   }
 
 };
