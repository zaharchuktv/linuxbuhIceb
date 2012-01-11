/*$Id: l_uosgrbu.h,v 1.3 2011-01-13 13:49:51 sasa Exp $*/
/*13.04.2009	18.10.2007	Белых А.И.	l_uosgrbu.h
Реквизиты для работы со списком групп налогового учёта в подсистеме "Учёт основных средств"
*/
class l_uosgrbu_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str kof;   
   int metka_ar; /*0-от начальной 1-от остаточной*/
   l_uosgrbu_rek()
    {
     clear_data();
    }
   void clear_data()
    {
     kod.new_plus("");
     naim.new_plus("");
     kof.new_plus("");
     metka_ar=0;
    }
 }; 
