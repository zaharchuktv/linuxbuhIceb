/*$Id: dvtmcf2.h,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*19.06.2010	28.11.2004	Белых А.И.	dvtmcf2.h
Реквизиты для получения отчета - движения товатно-материальных ценностей форма 2
*/

class dvtmcf2_rr
 {
  public:
  
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str shet;
  static iceb_u_str sklad;
  static iceb_u_str grupa;
  static iceb_u_str kodmat;
  static iceb_u_str nds;
  static iceb_u_str kontr;
  static iceb_u_str kodop;
  static int pr; /*0-всё 1-приходы 2-расходы*/
  static iceb_u_str nomdok;
  
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
    kontr.new_plus("");
    kodop.new_plus("");
    pr=0;
    nomdok.new_plus("");
    nds.new_plus("");

   }
  
 };
