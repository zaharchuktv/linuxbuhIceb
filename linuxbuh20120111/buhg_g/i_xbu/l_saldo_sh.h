/*$Id: l_saldo_sh.h,v 1.10 2011-01-13 13:49:50 sasa Exp $*/
/*25.04.2008	30.12.2003	Белых А.И.	l_saldo_sh.h
Класс для поистк стартового сальдо по счетам со свётрнутым сальдо
*/

class  saldo_sh_poi
 {
  public:
  iceb_u_str god;
  iceb_u_str shet;
  iceb_u_str debet;
  iceb_u_str kredit;
  short metka_poi;
  void clear_zero()      
   {
    god.new_plus("");  
    shet.new_plus("");  
    debet.new_plus("");  
    kredit.new_plus("");  
   }
  saldo_sh_poi()
   {
    metka_poi=0;
   }
 };
 
