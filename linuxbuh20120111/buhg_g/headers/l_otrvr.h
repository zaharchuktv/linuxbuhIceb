/*$Id: l_otrvr.h,v 1.2 2011-01-13 13:49:50 sasa Exp $*/
/*19.09.2006	19.09.2006	Белых А.И.	l_otrvr.h
Реквизиты для поиска записей в спислке табелей с отработанным временем
*/

class l_otrvr_rek
 {
  public:
   class iceb_u_str tabnom;
   class iceb_u_str kod_tab;
   class iceb_u_str koment;
   class iceb_u_str podr;
   
   l_otrvr_rek()
    {
     clear_rek();
    }
   void clear_rek()
    {
     tabnom.new_plus("");
     kod_tab.new_plus("");
     koment.new_plus("");
     podr.new_plus("");
    }
 };
  


