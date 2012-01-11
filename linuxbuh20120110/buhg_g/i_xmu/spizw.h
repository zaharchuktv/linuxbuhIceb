/*$Id: spizw.h,v 1.4 2011-01-13 13:49:53 sasa Exp $*/
/*20.10.2009	17.05.2005	Белых А.И.	spizw.h
Реквизиты для списания изделия в материальном учёте
*/

class spizw_rr
 {
  public:
  class iceb_u_str datas;
  class iceb_u_str sklad;
  class iceb_u_str nomdok;
  class iceb_u_str koliz;
  class iceb_u_str kodop;

  class iceb_u_str kod_izdel; //код изделия материалы которого надо списать
  
  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
  
  void clear_data()
   {
    datas.new_plus("");
    sklad.new_plus("");
    nomdok.new_plus("");
    koliz.new_plus("");
    kodop.new_plus("");
   }
 };
