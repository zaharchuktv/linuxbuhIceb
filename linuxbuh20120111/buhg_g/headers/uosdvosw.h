/*$Id: uosdvosw.h,v 1.2 2011-01-13 13:49:54 sasa Exp $*/
/*28.12.2007	28.12.2007	Белых А.И.	uosdvosw.h
Реквизиты для расчёта ведомости движения основных средств
*/

class uosdvosw_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str grupa_nu;
   class iceb_u_str grupa_bu;
   class iceb_u_str podr;
   class iceb_u_str mat_ot;
   class iceb_u_str shetu;
   class iceb_u_str hau;
   class iceb_u_str innom;
   int metka_sort; /*0-по счетам учёта 1-по шифрам производственных затрат (счетам амортизационных отчислений)*/
   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   uosdvosw_data()
    {
     clear();
    }   

   void clear()
    {
     metka_sort=0;
     datan.new_plus("");
     datak.new_plus("");
     grupa_nu.new_plus("");
     grupa_bu.new_plus("");
     podr.new_plus("");
     mat_ot.new_plus("");
     shetu.new_plus("");
     hau.new_plus("");
     innom.new_plus("");
    }

 };
