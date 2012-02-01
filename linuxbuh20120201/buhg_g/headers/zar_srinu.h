/*$Id: zar_srinu.h,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*28.11.2006	28.11.2006	Белых А.И.	zar_srinu.h
Реквизиты для расчёта свода начислений/удержаний по категориям
*/
class zar_srinu_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str kod_nah;
   class iceb_u_str kod_ud;
   class iceb_u_str tabnom;
   class iceb_u_str kod_kat;
   class iceb_u_str shet;
   class iceb_u_str v_mes; //В счёт какого месяца начисление
   
   short metka_pr; //0-приходы 1-расходы
         
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_srinu_rek()
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
    shet.new_plus("");
    v_mes.new_plus("");
    metka_pr=0;
   }
 };
