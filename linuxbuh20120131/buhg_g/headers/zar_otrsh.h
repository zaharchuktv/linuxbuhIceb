/*$Id: zar_otrsh.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*21.11.2006	14.11.2006	Белых А.И.	zar_otrsh.h
Реквизиты для расчёта свода ничислений/удержаний по категориям
*/
class zar_otrsh_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str tabnom;
   class iceb_u_str kod_kat;
   class iceb_u_str kod_tab;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   short metka_ras; //0-расчёт по данным меню 1-расчёт по настройкам в файле
   zar_otrsh_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    metka_ras=0;
    clear_nast();
   }
  void clear_nast()
   {
    podr.new_plus("");
    tabnom.new_plus("");
    kod_kat.new_plus("");
    kod_tab.new_plus("");
   } 
 };
