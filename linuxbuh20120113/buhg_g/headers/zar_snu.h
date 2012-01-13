/*$Id: zar_snu.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*30.11.2006	30.11.2006	Белых А.И.	zar_snu.h
Реквизиты для расчёта свода начислений/удержаний по работникам
*/
class zar_snu_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str kod_nah;
   class iceb_u_str kod_ud;
   class iceb_u_str tabnom;
   class iceb_u_str kod_kat;
   class iceb_u_str kolih_kol;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   short metka_vr;  //0-сортировка по табельным номерам 1-по фамилиям
   zar_snu_rek()
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
    kolih_kol.new_plus("");
    metka_vr=0;
   }
 };
