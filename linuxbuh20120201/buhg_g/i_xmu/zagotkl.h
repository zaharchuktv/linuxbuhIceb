/*$Id: zagotkl.h,v 1.5 2011-01-13 13:49:55 sasa Exp $*/
/*23.11.2004	23.11.2004	Белых А.И.	zagotkl.h
Движение товарно-материальных ценностей форма 1
*/

class zagotkl_rr
 {
  public:
  
  static iceb_u_str datao;
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
    datao.new_plus("");
    shet.new_plus("");
    sklad.new_plus("");
    grupa.new_plus("");
    kodmat.new_plus("");
    nds.new_plus("");

   }
  
 };
