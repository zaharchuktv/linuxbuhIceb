/*$Id: zarpdw.h,v 1.1 2011-03-28 06:55:46 sasa Exp $*/
/*09.09.2008	10.03.2006	Белых А.И.	zarpdw.h
Класс для получения отчёта
*/

class zarpdw_rr
 {
  public:
  class iceb_u_str datar;
  class iceb_u_str tabnom;
  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
    
  zarpdw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datar.new_plus("");
    tabnom.new_plus("");
   }
 };
