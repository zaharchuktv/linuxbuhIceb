/*$Id: buhrpzw.h,v 1.3 2011-01-13 13:49:49 sasa Exp $*/
/*01.03.2007	01.03.2007	Белых А.И.	buhrpzw.h
Реквизиты для расчёта видов затрат по элементам
*/

class buhrpzw_rr
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
      
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   buhrpzw_rr()
    {
     clear_data();
    }
   void clear_data()
    {
     datan.new_plus("");
     datak.new_plus("");
    }    
 }; 
