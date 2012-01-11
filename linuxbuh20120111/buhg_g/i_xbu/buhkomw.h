/*$Id: buhkomw.h,v 1.3 2011-01-13 13:49:49 sasa Exp $*/
/*17.06.2006	07.06.2005	Белых А.И.	buhkomw.h
Реквизиты для получения отчёта по коментрариям в проводках "Главной книги"
*/

class buhkomw_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str shet;

  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
   }
 };
