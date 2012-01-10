/*$Id: upl_shkg.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*02.04.2008	02.04.2008	Белых А.И.	upl_shkg.h
Реквизиты ведомости списания топлива по счетам в киллограммах
*/

class upl_shkg_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kod_vod;
   class iceb_u_str shet;
   class iceb_u_str kod_top;
      
   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   upl_shkg_data()
    {
     clear();
    }   

   void clear()
    {
     datan.new_plus("");
     datak.new_plus("");
     kod_vod.new_plus("");
     shet.new_plus("");
     kod_top.new_plus("");
    }

 };
