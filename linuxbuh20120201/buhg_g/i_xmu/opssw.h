/*$Id: opssw.h,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*16.06.2006	09.05.2005	Белых А.И.	opssw.h
Реквизиты для получения отчёта по счетам списания материалов
*/
class opssw_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str sklad;
  class iceb_u_str kodgrmat;
  class iceb_u_str kodmat;
  class iceb_u_str shet_sp;
  class iceb_u_str shet_uh;
  class iceb_u_str kodop;
  class iceb_u_str kontr;

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  
  opssw_rr()
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
    shet_sp.new_plus("");
    shet_uh.new_plus("");
    kodop.new_plus("");
    kontr.new_plus("");

   }
 };
