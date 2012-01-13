/*$Id: l_uosspdok.h,v 1.3 2011-01-13 13:49:51 sasa Exp $*/
/*14.10.2009	30.10.2007	Белых А.И.	l_uosspdok.h
Реквизиты поиска документов в "Учёте основных средств"
*/
class uosspdok_rek
 {
  public:
  iceb_u_str nomdok;
  iceb_u_str datan;    
  iceb_u_str datak;    
  iceb_u_str kontr;    
  iceb_u_str kodop;    
  iceb_u_str kodpod;    
  iceb_u_str pr_ras;    
  class iceb_u_str mat_ot;
  
  short      metka_poi;  //0-нет поиска 1-есть поск
  short      metka_pros; //0-все 1-не подтверждённые 2-без проводок    
  short metka_opl;  

  uosspdok_rek()
   {
    clear();
    metka_opl=0;
   }
  
  void clear()
   {
    nomdok.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    kontr.new_plus("");
    kodop.new_plus("");
    kodpod.new_plus("");
    pr_ras.new_plus("");
    mat_ot.new_plus("");
    metka_poi=0;
    metka_pros=0;
   }
 
 };
