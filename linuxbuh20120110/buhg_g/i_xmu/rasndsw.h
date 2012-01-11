/*$Id: rasndsw.h,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*17.06.2006	24.04.2005	Белых А.И.	rasndsw.h
Реквизиты для получения отчёта - распределение НДС по видам операций
*/

class rasndsw_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str sklad;
  class iceb_u_str kontr;
  short     pr_ras; //0-всё 1-только приходы 2-только расходы
  class iceb_u_str kodop;
  class iceb_u_str shet;
  class iceb_u_str kodgrmat;
  class iceb_u_str kodmat;

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  rasndsw_rr()
   {
    clear_data();
   }
   
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    kontr.new_plus("");
    sklad.new_plus("");
    kodop.new_plus("");
    kodgrmat.new_plus("");
    kodmat.new_plus("");
   }
 };
