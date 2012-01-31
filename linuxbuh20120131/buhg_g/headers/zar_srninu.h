/*$Id: zar_srninu.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*21.11.2006	14.11.2006	Белых А.И.	zar_srninu.h
Реквизиты для расчёта свода ничислений/удержаний по категориям
*/
class zar_srninu_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str kod_nah;
   class iceb_u_str kod_ud;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_srninu_rek()
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
   }
 };
