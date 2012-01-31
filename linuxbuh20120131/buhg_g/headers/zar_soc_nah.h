/*$Id: zar_soc_nah.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*16.01.2007	16.01.2007	Белых А.И.	zar_soc_nah.h
Реквизиты для расчёта свода отчислений в соц-фонды на фонд оплаты труда
*/
class zar_soc_nah_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str tabnom;
   class iceb_u_str kod_soc;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_soc_nah_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    tabnom.new_plus("");
    kod_soc.new_plus("");
   }
 };
