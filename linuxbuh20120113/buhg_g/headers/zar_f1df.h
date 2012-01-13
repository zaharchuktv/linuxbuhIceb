/*$Id: zar_f1df.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*20.12.2006	20.12.2006	Белых А.И.	zar_f1df.h
Реквизиты для расчёта формы 1ДФ
*/
class zar_f1df_rek
 {
  public:
   class iceb_u_str kvart;
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str tabnom;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_f1df_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    kvart.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    tabnom.new_plus("");
   }
 };
