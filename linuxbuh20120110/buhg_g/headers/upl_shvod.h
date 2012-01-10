/*$Id: upl_shvod.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*02.04.2008	02.04.2008	Белых А.И.	upl_shvod.h
Реквизиты расчёта оборотной ведомости движения топлива по автомобилям
*/

class upl_shvod_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kod_vod;
   class iceb_u_str kod_avt;
   class iceb_u_str shet;
   class iceb_u_str kod_top;
      
   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   upl_shvod_data()
    {
     clear();
    }   

   void clear()
    {
     datan.new_plus("");
     datak.new_plus("");
     kod_vod.new_plus("");
     kod_avt.new_plus("");
     shet.new_plus("");
     kod_top.new_plus("");
    }

 };
