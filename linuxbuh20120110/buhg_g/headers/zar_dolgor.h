/*$Id: zar_dolgor.h,v 1.4 2011-01-13 13:49:55 sasa Exp $*/
/*04.12.2006	04.12.2006	Белых А.И.	zar_dolgor.h
Реквизиты для расчёта долгов предприятия перед работниками
*/
class zar_dolgor_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datad;
   class iceb_u_str podr;
   class iceb_u_str tabnom;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   short metka_ras; //0-по указанным месяцам 1- за период от месяца начисления до даты расчёта   
   short metka_vse; //0-всех 1-только кому должны
   zar_dolgor_rek()
    {
     clear_data();
     datad.poltekdat();     
    }
  
  void clear_data()
   {
    metka_ras=0;
    datan.new_plus("");
    datad.new_plus("");
    podr.new_plus("");
    tabnom.new_plus("");
   }
 };
