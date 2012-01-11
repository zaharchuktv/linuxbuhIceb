/*$Id: zar_otrvr.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*29.11.2006	29.11.2006	Белых А.И.	zar_otrvr.h
Реквизиты для расчёта отработанного времени по работникам
*/
class zar_otrvr_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str tabnom;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_otrvr_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    tabnom.new_plus("");
   }
 };
