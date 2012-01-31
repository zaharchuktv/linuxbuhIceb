/*$Id: imp_zardokw.h,v 1.2 2011-01-13 13:49:50 sasa Exp $*/
/*09.09.2008	09.09.2008	Белых А.И.	imp_zardokw.h
Импорт начислений/удержаний в документ подсистемы "Заработная плата"
*/

class imp_zardokw_rr
 {
  public:
  class iceb_u_str datadok;
  class iceb_u_str nomdok;
  class iceb_u_str imaf_imp;
  int prn;

//  class iceb_u_spisok imaf;
//  class iceb_u_spisok naimf;
  class iceb_u_str naim_oth; //наименование отчёта
    
  imp_zardokw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    imaf_imp.new_plus("");
    datadok.new_plus("");
    nomdok.new_plus("");
    prn=0;
    imaf_imp.new_plus("");
   }
 };
