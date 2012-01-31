/*$Id: buhgspw.h,v 1.2 2011-01-13 13:49:49 sasa Exp $*/
/*01.07.2007	01.03.2007	Белых А.И.	buhgspw.h
Реквизиты для расчёта по сприскам групп контрагентов
*/

class buhgspw_rr
 {
  public:
   class iceb_u_str shet;
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kod_spis_kontr;
      
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   buhgspw_rr()
    {
     clear_data();
    }
   void clear_data()
    {
     shet.new_plus("");
     kod_spis_kontr.plus("");
     datan.new_plus("");
     datak.new_plus("");
    }    
 }; 
