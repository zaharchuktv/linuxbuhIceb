/*$Id: zar_podr.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*29.01.2007	29.01.2007	Белых А.И.	zar_podr.h
Реквизиты для расчёта свода по подразделениям формы 1 и 2
*/
class zar_podr_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str kod_nah;
   class iceb_u_str kod_ud;
   class iceb_u_str tabnom;
   class iceb_u_str kod_kat;
   class iceb_u_str shet;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_podr_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    kod_nah.new_plus("");
    kod_ud.new_plus("");
    tabnom.new_plus("");
    kod_kat.new_plus("");
    shet.new_plus("");
   }
 };
