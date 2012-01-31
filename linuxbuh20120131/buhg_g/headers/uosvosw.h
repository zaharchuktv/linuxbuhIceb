/*$Id: uosvosw.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*14.01.2008	14.01.2008	Белых А.И.	uosvosw.h
Реквизиты для расчёта ведомости износа и остаточной стоимости
*/

class uosvosw_data
 {
  public:
   class iceb_u_str datak;
   class iceb_u_str grupa_nu;
   class iceb_u_str grupa_bu;
   class iceb_u_str podr;
   class iceb_u_str mat_ot;
   class iceb_u_str hau;
   class iceb_u_str shetu;
   class iceb_u_str hzt;
   class iceb_u_str innom;
   int sost_ob;

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   int metka_mat_podr; /*0-по подразделениям 1-по материально-ответственным*/
   int metka_ost; /*0-все инвентарные номера 1-с нулевой остаточной стоимостью 2-с не нулевой остаточной стоимостью*/
   int metka_sort;   /*0-сортировка по группам 1-по счетам учёта*/
   
   uosvosw_data()
    {
     clear();
    }   

   void clear()
    {
     sost_ob=0;
     datak.new_plus("");
     grupa_nu.new_plus("");
     grupa_bu.new_plus("");
     podr.new_plus("");
     mat_ot.new_plus("");
     shetu.new_plus("");
     hzt.new_plus("");
     innom.new_plus("");
     hau.new_plus("");
     metka_mat_podr=0;
     metka_ost=0;
     metka_sort=0;
    }

 };
