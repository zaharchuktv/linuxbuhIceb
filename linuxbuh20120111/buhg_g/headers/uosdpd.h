/*$Id: uosdpd.h,v 1.2 2011-01-13 13:49:54 sasa Exp $*/
/*13.01.2008	13.01.2008	Белых А.И.	uosdpd.h
Реквизиты для расчёта движения по документам
*/

class uosdpd_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kontr;
   class iceb_u_str kod_op;
   class iceb_u_str podr;
   class iceb_u_str mat_ot;
   class iceb_u_str shetu;
   class iceb_u_str grup_nu;
   class iceb_u_str grup_bu;
   int prih_rash; /*0-всё 1-приход 2-расход*/

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   uosdpd_data()
    {
     clear();
    }   

   void clear()
    {
     podr.new_plus("");
     shetu.new_plus("");
     kontr.new_plus("");
     kod_op.new_plus("");
     datan.new_plus("");
     datak.new_plus("");
     prih_rash=0;
     mat_ot.new_plus("");
     grup_nu.new_plus("");
     grup_bu.new_plus("");
    }

 };
