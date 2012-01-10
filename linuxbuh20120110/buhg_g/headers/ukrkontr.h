/*$Id: ukrkontr.h,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*12.08.2009	28.02.2008	Белых А.И.	ukrkontr.h
Реквизиты для расчёта отчёта по контрагентам
*/

class ukrkontr_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kontr;
   class iceb_u_str vidkom;
   class iceb_u_str kod_zat;
   class iceb_u_str kod_gr_pod;
   class iceb_u_str shetu;

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   ukrkontr_data()
    {
     clear();
    }   

   void clear()
    {
     kod_zat.new_plus("");
     kontr.new_plus("");
     vidkom.new_plus("");
     datan.new_plus("");
     datak.new_plus("");
     kod_gr_pod.new_plus("");
     shetu.new_plus("");
    }

 };
