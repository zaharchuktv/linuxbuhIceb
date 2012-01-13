/*$Id: rppvouw.h,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*23.12.2005	23.12.2005	Белых А.И.	rppvouw.h
Реквизиты расчёта реестара проводок по видам операций
*/

class rppvouw_rr
 {
  public:
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str podr;
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
    podr.new_plus("");
    kodop.new_plus("");
   }
 };
