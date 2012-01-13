/*$Id: buhpsdw.h,v 1.2 2011-01-13 13:49:49 sasa Exp $*/
/*10.09.2008	10.09.2008	Белых А.И.	buhpsdw.h
Реквизиты расчета акта-сверки по группе контрагента
*/

class buhpsdw_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str shet;
  class iceb_u_str podsys;
  
  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    podsys.new_plus("");
   }
 };

