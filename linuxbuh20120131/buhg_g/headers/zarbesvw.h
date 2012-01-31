/*$Id: zarbesvw.h,v 1.1 2011-04-14 16:09:36 sasa Exp $*/
/*10.04.2011	10.04.2011	Белых А.И.	zarbesvw.h
Класс для получения отчёта
*/

class zarbesvw_rr
 {
  public:
  class iceb_u_str datan;

  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
    
  zarbesvw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datan.new_plus("");
   }
 };
