/*$Id: uosvamotw.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*02.01.2008	31.12.2007	Белых А.И.	uosvamotw.h
Реквизиты для расчёта ведомости движения основных средств
*/

class uosvamotw_data
 {
  public:
   class iceb_u_str mesn;
   class iceb_u_str mesk;
   class iceb_u_str god;
   class iceb_u_str grupa;
   class iceb_u_str podr;
   class iceb_u_str mat_ot;
   class iceb_u_str shetu;
   class iceb_u_str hzt;
   class iceb_u_str innom;
   int sost_ob;

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   uosvamotw_data()
    {
     clear();
    }   

   void clear()
    {
     sost_ob=0;
     mesn.new_plus("");
     mesk.new_plus("");
     god.new_plus("");
     grupa.new_plus("");
     podr.new_plus("");
     mat_ot.new_plus("");
     shetu.new_plus("");
     hzt.new_plus("");
     innom.new_plus("");
    }

 };
