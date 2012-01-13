/*$Id: buhrpznpw.h,v 1.2 2011-01-13 13:49:49 sasa Exp $*/
/*01.03.2007	01.03.2007	Белых А.И.	buhrpznpw.h
Реквизиты для расчёта распределения административных затрат на доходы
*/

class buhrpznpw_rr
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
      
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   buhrpznpw_rr()
    {
     clear_data();
    }
   void clear_data()
    {
     datan.new_plus("");
     datak.new_plus("");
    }    
 }; 
