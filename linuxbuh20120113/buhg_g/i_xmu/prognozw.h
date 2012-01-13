/*$Id: prognozw.h,v 1.4 2011-01-13 13:49:52 sasa Exp $*/
/*17.06.2006	01.05.2005	Белых А.И.	prognozw.h
Реквизиты для получения отчёта - прогноз реализации
*/

class prognozw_rr
 {
  public:

  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str sklad;
  class iceb_u_str kodgrmat;
  class iceb_u_str kodmat;
  class iceb_u_str kolden;

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;

  prognozw_rr()
   {
    clear_data();
   }
   
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    sklad.new_plus("");
    kodgrmat.new_plus("");
    kodmat.new_plus("");
    kolden.new_plus("");
   }
 };
