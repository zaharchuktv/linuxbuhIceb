/*$Id: dvtmcf3.h,v 1.4 2011-01-13 13:49:50 sasa Exp $*/
/*30.11.2004	30.11.2004	Белых А.И.	dvtmcf3.h
Реквизиты для расчёта движения товатно-материальных ценностей форма 3
*/
class dvtmcf3_rr
 {
  public:
  
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str shet;
  static iceb_u_str sklad;
  static iceb_u_str grupa_mat;
  static iceb_u_str grupa_kontr;
  static iceb_u_str kodmat;
  static iceb_u_str kontr;
  static iceb_u_str kodop;
  static iceb_u_str pr;
  static iceb_u_str nomdok;
  static iceb_u_str vcena;  
  static int        metka_ceni;  
  static int        metka_ras;
  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  
  void clear_data()
   {
    metka_ras=0; 
    metka_ceni=0;
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    sklad.new_plus("");
    grupa_mat.new_plus("");
    grupa_kontr.new_plus("");
    kodmat.new_plus("");
    kontr.new_plus("");
    kodop.new_plus("");
    pr.new_plus("");
    nomdok.new_plus("");
    vcena.new_plus("");
   }
  
 };
