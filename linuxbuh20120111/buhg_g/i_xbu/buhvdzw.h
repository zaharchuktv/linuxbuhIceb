/*$Id: buhvdzw.h,v 1.3 2011-01-13 13:49:49 sasa Exp $*/
/*02.03.2007	02.03.2007	Белых А.И.	buhvdzw.h
Реквизиты для расчёта валовых затрат
*/

class buhvdzw_rr
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   int var_ras; //0-доходы и затраты 1-только доходы 2-только затраты      
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   buhvdzw_rr()
    {
     clear_data();
     var_ras=0;
    }
   void clear_data()
    {
     datan.new_plus("");
     datak.new_plus("");
    }    
 }; 
