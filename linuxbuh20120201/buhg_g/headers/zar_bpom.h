/*$Id: zar_bpom.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*08.12.2006	08.12.2006	Белых А.И.	zar_bpom.h
Реквизиты для распечатки списка работников имеющих благотворительную помощь
*/
class zar_bpom_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str tabnom;
   class iceb_u_str kod_kat;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_bpom_rek()
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
