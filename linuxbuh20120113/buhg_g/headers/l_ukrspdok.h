/*$Id: l_ukrspdok.h,v 1.2 2011-01-13 13:49:50 sasa Exp $*/
/*21.02.2008	21.02.2008	Белых А.И.	l_ukrspdok.h
Реквизиты поиска документов в "Учёте командировочных расходов"
*/
class ukrspdok_rek
 {
  public:
  class iceb_u_str nomdok;
  class iceb_u_str datan;    
  class iceb_u_str datak;    
  class iceb_u_str kontr;    
  class iceb_u_str vidkom;    
  
  class iceb_u_str nomer_prik;
  class iceb_u_str data_prik;
  class iceb_u_str nomer_ao;
  class iceb_u_str data_ao;
  
  short      metka_poi;  //0-нет поиска 1-есть поск
  short      metka_pros; //0-все 1-не подтверждённые 2-без проводок    
  
  ukrspdok_rek()
   {
    clear();
   }
  
  void clear()
   {
    nomdok.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    kontr.new_plus("");
    vidkom.new_plus("");
    nomer_prik.new_plus("");
    data_prik.new_plus("");
    nomer_ao.new_plus("");
    data_ao.new_plus("");
    metka_poi=0;
    metka_pros=0;
   }
 
 };
