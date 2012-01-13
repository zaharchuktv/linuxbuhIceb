/*$Id: arz.h,v 1.2 2011-01-13 13:49:49 sasa Exp $*/
/*08.11.2006	08.11.2006	Белых А.И.	arz.h
Реквизиты для автоматической розноски зарплаты
*/
class arz_rek
 {
  public:
   class iceb_u_str data;
   class iceb_u_str podr;
   class iceb_u_str kod_nah;
   class iceb_u_str tabnom;
   class iceb_u_str kod_kat;
   class iceb_u_str data_per;
   class iceb_u_str max_sum;
   class iceb_u_str proc_vip;
   class iceb_u_str kod_grup_pod;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   short metka_saldo;   //0- с учётом сальдо 1-без

   arz_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    data.new_plus("");
    podr.new_plus("");
    kod_nah.new_plus("");
    tabnom.new_plus("");
    kod_kat.new_plus("");
    proc_vip.new_plus("100");
    kod_grup_pod.new_plus("");
    max_sum.new_plus("");
    data_per.new_plus("");
    metka_saldo=0;
   }
 
 };
