/*$Id: specrasw.h,v 1.3 2011-01-13 13:49:53 sasa Exp $*/
/*17.05.2005	17.05.2005	Белых А.И.	specrasw.h
Реквизиты для расчёта разузлования изделия
*/

class specrasw_rr
 {
  public:

  iceb_u_str kolih;

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  
  iceb_u_str kod_izdel;
  iceb_u_str naim_izdel;
  specrasw_rr()
   {
    clear_data();
   }
  void clear_data()
   {
    kolih.new_plus("");
   }
 };
