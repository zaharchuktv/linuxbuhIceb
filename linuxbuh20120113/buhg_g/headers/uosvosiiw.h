/*$Id: uosvosiiw.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*08.01.2008	07.01.2008	Белых А.И.	uosvosiiw.h
Реквизиты для расчёта ведомости износа и остаточной стоимости
*/

class uosvosiiw_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str grupa_nu;
   class iceb_u_str grupa_bu;
   class iceb_u_str podr;
   class iceb_u_str mat_ot;
   class iceb_u_str hau;
   class iceb_u_str shetu;
   class iceb_u_str hzt;
   class iceb_u_str innom;
   class iceb_u_str kod_op;
   int sost_ob;

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   int metka_mat_podr; /*0-по подразделениям 1-по материально-ответственным*/
   int metka_ost; /*0-все инвентарные номера 1-с нулевой остаточной стоимостью 2-с не нулевой остаточной стоимостью*/
   int metka_sort;   /*0-сортировка по группам 1-по счетам учёта*/
   
   uosvosiiw_data()
    {
     clear();
    }   

   void clear()
    {
     sost_ob=0;
     datan.new_plus("");
     datak.new_plus("");
     grupa_nu.new_plus("");
     grupa_bu.new_plus("");
     podr.new_plus("");
     mat_ot.new_plus("");
     shetu.new_plus("");
     hzt.new_plus("");
     innom.new_plus("");
     hau.new_plus("");
     kod_op.new_plus("");
     metka_mat_podr=0;
     metka_ost=0;
     metka_sort=0;
    }

 };
