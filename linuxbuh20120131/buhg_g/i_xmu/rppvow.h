/*$Id: rppvow.h,v 1.3 2011-01-13 13:49:53 sasa Exp $*/
/*07.04.2005	07.04.2005	Белых А.И.	rppvow.h
Реквизиты расчёта реестара проводок по видам операций
*/

class rppvow_rr
 {
  public:
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str sklad;
  static iceb_u_str kontr;
  static short     pr_ras; //0-всё 1-только приходы 2-только расходы
  static iceb_u_str kodop;
  static iceb_u_str shet;

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    kontr.new_plus("");
    sklad.new_plus("");
    kodop.new_plus("");
   }
 };
