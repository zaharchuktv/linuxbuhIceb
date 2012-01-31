/*$Id: rspvk.h,v 1.5 2011-01-13 13:49:53 sasa Exp $*/
/*07.02.2005	28.03.2004	Белых А.И.	rspvk.h
Реквизиты расчета сальдо по всем контрагентам
*/

class rspvk_rr
 {
  public:
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str shet;
  static iceb_u_str kodgr;
  static iceb_u_str kontr;
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    kodgr.new_plus("");
    kontr.new_plus("");
   }
 };

