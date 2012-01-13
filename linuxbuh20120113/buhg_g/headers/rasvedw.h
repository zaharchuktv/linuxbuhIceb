/*$Id: rasvedw.h,v 1.7 2011-01-13 13:49:52 sasa Exp $*/
/*02.11.2006	31.10.2006	Белых А.И.	rasvedw.h
Реквизиты для расчёта ведомости заработной платы
*/
class rasvedw_rek
 {
  public:
   class iceb_u_str data;
   class iceb_u_str podr;
   class iceb_u_str kod_nah;
   class iceb_u_str tabnom;
   class iceb_u_str kod_kat;
   class iceb_u_str shet;
   class iceb_u_str proc_vip;
   class iceb_u_str kod_grup_pod;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   short metka_sort;
   short metka_saldo;   //0- с учётом сальдо 1-без
   rasvedw_rek()
    {
     metka_sort=0;
     clear_data();
    }
  
  void clear_data()
   {
    data.new_plus("");
    podr.new_plus("");
    kod_nah.new_plus("");
    tabnom.new_plus("");
    kod_kat.new_plus("");
    shet.new_plus("");
    proc_vip.new_plus("100");
    kod_grup_pod.new_plus("");
    metka_saldo=0;
   }
 
 };
