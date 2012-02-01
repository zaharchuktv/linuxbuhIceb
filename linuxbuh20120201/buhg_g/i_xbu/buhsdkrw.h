/*$Id: buhsdkrw.h,v 1.2 2011-01-13 13:49:49 sasa Exp $*/
/*01.03.2007	01.03.2007	Белых А.И.	buhsdkrw.h
Реквизиты для расчёта шахматки
*/

class buhsdkrw_rr
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str variant_r;
         
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   buhsdkrw_rr()
    {
     clear_data();
    }
   void clear_data()
    {
     variant_r.new_plus("");
     datan.new_plus("");
     datak.new_plus("");
    }    
 }; 
