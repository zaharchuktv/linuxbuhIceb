/*$Id: zar_spdr.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*24.11.2006	24.11.2006	Белых А.И.	zar_spdr.h
Реквизиты для распечатки списка работников с датой рождения
*/
class zar_spdr_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str kod_kat;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_spdr_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    kod_kat.new_plus("");
  
   }
 };
