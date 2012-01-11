/*$Id: l_uoshz.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*18.10.2007	18.10.2007	Белых А.И.	l_uoshz.h
Реквизиты для работы со списком шифров затрат в подсистеме "Учёт основных средств"
*/
class l_uoshz_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_uoshz_rek()
    {
     clear_data();
    }
   void clear_data()
    {
     kod.new_plus("");
     naim.new_plus("");
    }
 }; 
