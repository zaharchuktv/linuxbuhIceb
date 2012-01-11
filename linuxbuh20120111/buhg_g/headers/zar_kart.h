/*$Id: zar_kart.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*07.12.2006	07.12.2006	Белых А.И.	zar_kart.h
Реквизиты для распечатки карточки работника с начислениями и удержаниями за период
*/
class zar_kart_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str tabnom;
   class iceb_u_str kod_kat;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_kart_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    tabnom.new_plus("");
    kod_kat.new_plus("");
   }
 };
