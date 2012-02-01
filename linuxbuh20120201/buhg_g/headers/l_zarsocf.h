/*$Id: l_zarsocf.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*23.06.2006	23.06.2006	Белых А.И.	l_zarsocf.h
Реквизиты для работы со списком удержаний
*/
class l_zarsocf_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str shet;
   class iceb_u_str proc;
   class iceb_u_str kod_n_vras;
   l_zarsocf_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    shet.new_plus("");
    proc.new_plus("");
    kod_n_vras.new_plus("");
   }
 
 };
