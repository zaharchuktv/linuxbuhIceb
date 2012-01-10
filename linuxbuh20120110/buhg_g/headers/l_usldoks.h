/*$Id: l_usldoks.h,v 1.8 2011-01-13 13:49:51 sasa Exp $*/
/*14.10.2009	07.08.2005	Белых А.И.	l_usldoks.h
Реквизиты поиска документов на услуги 
*/
class usldoks_rek
 {
  public:
   class iceb_u_str nomdok;
   class iceb_u_str datan;    
   class iceb_u_str datak;    
   class iceb_u_str kontr;    
   class iceb_u_str kodop;    
   class iceb_u_str kodpod;    
   class iceb_u_str pr_ras;    
   class iceb_u_str naim_kontr;    
   class iceb_u_str nalnak;    
   class iceb_u_str dover;
   
  short      metka_poi;  //0-нет поиска 1-есть поск
  short      metka_pros; //0-все 1-не подтверждённые 2-без проводок    
  short metka_opl;
  usldoks_rek()
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
    naim_kontr.new_plus("");
    nalnak.new_plus("");
    pr_ras.new_plus("");
    dover.new_plus("");
    metka_poi=0;
    metka_pros=0;
   }
 
 };
