/*$Id: zar_pensf.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*15.01.2007	15.01.2007	Белых А.И.	zar_pensf.h
Реквизиты для расчёта данных для перегруза в программу Пенсионного фонда Украины
*/
class zar_pensf_rek
 {
  public:
   class iceb_u_str mesn;
   class iceb_u_str mesk;
   class iceb_u_str god;
   class iceb_u_str tabnom;
   class iceb_u_str datad;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_pensf_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    mesn.new_plus("");
    mesk.new_plus("");
    god.new_plus("");
    tabnom.new_plus("");
    datad.new_plus("");
   }
 };
