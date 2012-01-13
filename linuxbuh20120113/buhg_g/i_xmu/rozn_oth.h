/*$Id: rozn_oth.h,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*02.05.2005	02.05.2005	Белых А.И.	rozn_oth.h
Даты для получения отчётов по розничной продаже
*/
class rozn_oth_rr
 {
  public:
  static iceb_u_str datan;
  static iceb_u_str datak;

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
   }
 };
