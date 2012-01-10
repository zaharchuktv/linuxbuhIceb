/*$Id: l_zarud.h,v 1.3 2011-02-21 07:35:55 sasa Exp $*/
/*15.01.2011	23.06.2006	Белых А.И.	l_zarud.h
Реквизиты для работы со списком удержаний
*/
class l_zarud_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str shet;
   class iceb_u_str shetb;
   short prov; //0-делать проводки 1-не делать   
   l_zarud_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    shet.new_plus("");
    shetb.new_plus("");
    prov=0;
   }
 
 };
