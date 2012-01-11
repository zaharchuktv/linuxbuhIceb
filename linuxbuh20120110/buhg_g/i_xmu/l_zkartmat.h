/*$Id: l_zkartmat.h,v 1.4 2011-01-13 13:49:51 sasa Exp $*/

class zkartmat_poi
 {
  public:
  iceb_u_str datan;
  iceb_u_str datak;    
  iceb_u_str nomdok;
  iceb_u_str kontr;
  iceb_u_str kodop;
  int        pri_ras; //0-всё 1-приходы 2-расходы
  short      metka_poi;  //0-нет поиска 1-есть поск
    
  zkartmat_poi()
   {
    clear();
    metka_poi=0;
   }
  
  void clear()
   {
    datan.new_plus("");
    datak.new_plus("");
    nomdok.new_plus("");
    kontr.new_plus("");
    kodop.new_plus("");
    pri_ras=0;
   }
 
 };
