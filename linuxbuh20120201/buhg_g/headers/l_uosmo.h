/*$Id: l_uosmo.h,v 1.3 2011-01-13 13:49:51 sasa Exp $*/
/*20.11.2007	18.10.2007	Белых А.И.	l_uosmo.h
Реквизиты для работы со списком материально-ответственных в подсистеме "Учёт основных средств"
*/
class l_uosmo_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   int metka_sost; /*0-используется 1-не используется*/   
   
   l_uosmo_rek()
    {
     clear_data();
    }
   void clear_data()
    {
     metka_sost=0;
     kod.new_plus("");
     naim.new_plus("");
    }
 }; 
