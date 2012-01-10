/*$Id: l_uplout.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*05.03.2008	05.03.2008	Белых А.И.	l_uplout.h
Реквизиты для поиска записей в списке объектов учёта топлива
*/
class l_uplout_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str sklad;
   class iceb_u_str kontr;
   int metka_kod; /*0-заправка 1-водитель*/
      
   l_uplout_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    sklad.new_plus("");
    kontr.new_plus("");
    metka_kod=0;
   }
 
 };
