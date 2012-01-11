/*$Id: imp_kr.h,v 1.4 2011-01-13 13:49:50 sasa Exp $*/
/*17.10.2006	10.10.2006	Белых А.И.	imp_kr.h
Реквизиты для импорта начислений/удержаний из подсистемы "Учёт командировочных расходов".
*/

class imp_kr_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str denz;
   class iceb_u_str kod_zat;
   
   short mz,gz; //месяц просмотра записей
   int kod_nah_ud;
   short prn;
      
  imp_kr_rek()
   {
    clear_rek();
   }
  void clear_rek()
   {
    datan.new_plus("");
    datak.new_plus("");
    denz.new_plus("");
    kod_zat.new_plus("");
   }   
 };
