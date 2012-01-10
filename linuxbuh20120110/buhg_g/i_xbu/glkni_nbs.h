/*$Id: glkni_nbs.h,v 1.4 2011-01-13 13:49:50 sasa Exp $*/
/*08.02.2005	22.01.2004	Белых А.И.	glkni_nbs.h
Реквизиты расчета главной книги для небалансовых счетов
*/

class glkni_nbs_rr
 {
  public:
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str shet;
  static short metka_r;  //0-расчет по субсчетам 1-по счетам
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    metka_r=0;
   }
 };

