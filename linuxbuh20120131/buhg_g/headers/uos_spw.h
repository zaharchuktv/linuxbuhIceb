/*$Id: uos_spw.h,v 1.2 2011-01-13 13:49:54 sasa Exp $*/
/*15.01.2008	15.01.2008	Белых А.И.	uos_spw.h
Реквизиты для расчёта движения основных средств по счетам списания/приобретения
*/

class uos_spw_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kontr;
   class iceb_u_str kod_op;
   class iceb_u_str podr;
   class iceb_u_str mat_ot;
   class iceb_u_str shetu;
   class iceb_u_str shet_sp;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   uos_spw_data()
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
     mat_ot.new_plus("");
     shet_sp.new_plus("");
    }

 };
