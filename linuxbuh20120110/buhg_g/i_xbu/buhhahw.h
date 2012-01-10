/*$Id: buhhahw.h,v 1.4 2011-01-13 13:49:49 sasa Exp $*/
/*30.11.2007	20.06.2006	Белых А.И.	buhhahw.h
Реквизиты для расчёта шахматки
*/

class buhhahw_rr
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str variant;
   short metka_ras; //0-обычный расчёт 1-без взаимокореспондирующих счетов
      
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   buhhahw_rr()
    {
     clear_data();
     metka_ras=0;
    }
   void clear_data()
    {
     datan.new_plus("");
     datak.new_plus("");
     variant.new_plus("");
    }    
 }; 
