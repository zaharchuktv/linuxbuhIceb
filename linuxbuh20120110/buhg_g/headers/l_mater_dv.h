/*$Id: l_mater_dv.h,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*21.06.2004	21.06.2004	Белых А.И.	l_mater_dv.h
Структура для получения отчета движения по материаллу
*/

class mater_dv_data
 {
  public:
  static iceb_u_str sklad;
  static iceb_u_str datan;
  static iceb_u_str datak;
  
  mater_dv_data()
   {
    clear();
   }

  void clear()
   {
    sklad.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    
   }
    
  
 };
