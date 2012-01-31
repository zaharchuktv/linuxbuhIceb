/*$Id: zar_sprav.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*24.11.2006	24.11.2006	Белых А.И.	zar_sprav.h
Реквизиты для расчёта справок по зарплате
*/
class zar_sprav_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str tabnom;
   
   class iceb_u_str fio;
   short metka_vr;
   
//Если metka_vr= 1 - расчет справки о зарплате
//               2 - расчет средней зарплаты для больничного
//               3 - расчет справки для уволенного для соцстраха
//               4 - расчет справок по датам в счет которых были сделаны начисления
//               5 - удалено !!!! расчет отпускных преподпвателя
//               6 - расчет отпускных
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_sprav_rek()
    {
     clear_data();
     metka_vr=0;
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    tabnom.new_plus("");
    fio.new_plus("");
   }
 };
