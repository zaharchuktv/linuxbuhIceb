/*$Id: upl_ovdt.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*02.04.2008	02.04.2008	Белых А.И.	upl_ovdt.h
Реквизиты расчёта оборотной ведомости движения топлива
*/

class upl_ovdt_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kod_vod;
   class iceb_u_str kod_avt;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   upl_ovdt_data()
    {
     clear();
    }   

   void clear()
    {
     datan.new_plus("");
     datak.new_plus("");
     kod_vod.new_plus("");
     kod_avt.new_plus("");
    }

 };
