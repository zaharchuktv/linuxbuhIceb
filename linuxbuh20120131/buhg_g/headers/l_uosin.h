/*$Id: l_uosin.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*20.12.2007	20.12.2007	Белых А.И.	l_uosin.h  
Реквизиты поиска инвентарных номеров в списке
*/

class l_uosin_poi
 {
  public:
   class iceb_u_str naim;
   class iceb_u_str zavod_iz;
   class iceb_u_str pasport;
   class iceb_u_str model;
   class iceb_u_str god_iz;
   class iceb_u_str zav_nomer;
   class iceb_u_str data_vvoda;
   class iceb_u_str shetu;
   class iceb_u_str hzt;
   class iceb_u_str hau;
   class iceb_u_str hna;
   class iceb_u_str popkf_nu;
   class iceb_u_str popkf_bu;
   class iceb_u_str nom_znak;
   class iceb_u_str mat_ot;
   class iceb_u_str podr;
   class iceb_u_str hnabu;
   short metka_poi;
      
   l_uosin_poi()
    {
     metka_poi=0; /*0-без поиска 1-с поиском*/
     clear();
    }
   void clear()
    {
    
     naim.new_plus("");
     zavod_iz.new_plus("");
     pasport.new_plus("");
     model.new_plus("");
     god_iz.new_plus("");
     zav_nomer.new_plus("");
     data_vvoda.new_plus("");
     shetu.new_plus("");
     hzt.new_plus("");
     hau.new_plus("");
     hna.new_plus("");
     popkf_nu.new_plus("");
     popkf_bu.new_plus("");
     nom_znak.new_plus("");
     mat_ot.new_plus("");
     podr.new_plus("");
     hnabu.new_plus("");
     
    }

 };

