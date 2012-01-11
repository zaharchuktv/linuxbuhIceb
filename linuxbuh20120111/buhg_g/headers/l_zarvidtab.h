/*$Id: l_zarvidtab.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*23.06.2006	23.06.2006	Белых А.И.	l_zarvidtab.h
Реквизиты для работы со списком категорий
*/
class l_zarvidtab_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_zarvidtab_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
