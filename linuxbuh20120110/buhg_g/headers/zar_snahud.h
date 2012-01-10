/*$Id: zar_snahud.h,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*29.07.2008	30.01.2007	Белых А.И.	zar_snahud.h
Реквизиты для расчёта свода начислений и удержаний
*/
class zar_snahud_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str tabnom;
   class iceb_u_str shet;
   class iceb_u_str kateg;
      
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_snahud_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    tabnom.new_plus("");
    shet.new_plus("");
    kateg.new_plus("");
   }
 };
