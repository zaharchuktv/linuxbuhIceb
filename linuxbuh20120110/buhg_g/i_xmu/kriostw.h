/*$Id: kriostw.h,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*16.06.2006	06.05.2005	Белых А.И.	kriostw.h
Реквизиты для расчёта критических остатков
*/
class kriostw_rr
 {
  public:
  class iceb_u_str datao;
  class iceb_u_str sklad;
  class iceb_u_str kodgrmat;
  class iceb_u_str kodmat;
  class iceb_u_str shet;

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;

  kriostw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datao.new_plus("");
    sklad.new_plus("");
    kodgrmat.new_plus("");
    kodmat.new_plus("");
    shet.new_plus("");
   }
 };
