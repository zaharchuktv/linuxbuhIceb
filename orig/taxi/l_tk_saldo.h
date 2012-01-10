/*$Id: l_tk_saldo.h,v 1.2 2011-01-13 13:50:09 sasa Exp $*/
/*31.03.2005	31.03.2005	Белых А.И.	l_tk_saldo.h
Реквизиты для ввода новой записис и поиска записей
*/

class  tk_saldo_rek_data
 {
  public:
  iceb_u_str god;
  iceb_u_str kod;
  iceb_u_str saldo;

  tk_saldo_rek_data()
   {
    clear_data();
   }

  void clear_data()
   {
    god.new_plus("");
    kod.new_plus("");
    saldo.new_plus("");
   } 
  
 };
