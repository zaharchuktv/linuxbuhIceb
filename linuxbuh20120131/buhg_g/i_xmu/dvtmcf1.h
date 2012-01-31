/*$Id: dvtmcf1.h,v 1.4 2011-01-13 13:49:49 sasa Exp $*/
/*22.11.2004	22.11.2004	Белых А.И.	dvtmcf1.h
Движение товарно-материальных ценностей форма 1
*/

class dvtmcf1_rr
 {
  public:
  
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str shet;
  static iceb_u_str sklad;
  static iceb_u_str grupa;
  static iceb_u_str kodmat;
  static iceb_u_str nds;
  static int        metka_sort;  

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  
  void clear_data()
   {
    metka_sort=0;
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    sklad.new_plus("");
    grupa.new_plus("");
    kodmat.new_plus("");
    nds.new_plus("");

   }
  
 };
