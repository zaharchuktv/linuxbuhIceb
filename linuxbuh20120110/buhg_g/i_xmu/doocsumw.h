/*$Id: doocsumw.h,v 1.3 2011-01-13 13:49:49 sasa Exp $*/
/*06.05.2005	06.05.2005	Белых А.И.	doocsumw.h
Реквизиты для получени отчёта по дооценкам
*/
class doocsumw_rr
 {
  public:
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str sklad;

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    sklad.new_plus("");
   }
 };
