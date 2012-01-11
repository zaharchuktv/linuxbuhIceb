/*$Id: zar_soc_vz.h,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*16.01.2007	16.01.2007	Белых А.И.	zar_soc_vz.h
Реквизиты для расчёта свода отчислений в соц-фонды на выплаченную зарплату
*/
class zar_soc_vz_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str tabnom;
   class iceb_u_str vs_mes;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_soc_vz_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    tabnom.new_plus("");
    vs_mes.new_plus("");
   }
 };
