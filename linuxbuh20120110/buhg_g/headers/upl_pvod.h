/*$Id: upl_pvod.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*30.03.2008	30.03.2008	Белых А.И.	upl_pvod.h
Реквизиты расчёта реестра путевых листов по водителям
*/

class upl_pvod_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kod_vod;
   class iceb_u_str kod_avt;
   class iceb_u_str kod_pod;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   upl_pvod_data()
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
