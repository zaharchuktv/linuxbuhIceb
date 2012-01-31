/*$Id: upl_pavt.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*31.03.2008	31.03.2008	Белых А.И.	upl_pavt.h
Реквизиты расчёта оборотной ведомости движения топлива по автомобилям
*/

class upl_pavt_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kod_vod;
   class iceb_u_str kod_avt;
   class iceb_u_str kod_pod;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   upl_pavt_data()
    {
     clear();
    }   

   void clear()
    {
     datan.new_plus("");
     datak.new_plus("");
     kod_vod.new_plus("");
     kod_avt.new_plus("");
     kod_pod.new_plus("");
    }

 };
