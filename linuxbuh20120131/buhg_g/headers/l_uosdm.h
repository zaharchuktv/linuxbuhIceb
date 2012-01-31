/*$Id: l_uosdm.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*01.03.2009	01.03.2009	Белых А.И.	l_uosdm.h
Реквизиты для работы со списком драгоценных металлов
*/
class l_uosdm_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str ei;
   
   l_uosdm_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    ei.new_plus("");
   }
 
 };
