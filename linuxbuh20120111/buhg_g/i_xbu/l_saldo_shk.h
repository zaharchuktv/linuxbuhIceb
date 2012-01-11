/*$Id: l_saldo_shk.h,v 1.5 2011-01-13 13:49:50 sasa Exp $*/
/*25.04.2008	30.12.2003	Белых А.И.	l_saldo_shk.h
Класс для поиска записей в списке сальдо по счетам
*/
class  saldo_shk_poi
 {
  public:
  iceb_u_str god;
  iceb_u_str shet;
  iceb_u_str kodkontr;
  iceb_u_str debet;
  iceb_u_str kredit;
  short metka_poi;
  void clear_zero()      
   {
    god.new_plus("");  
    shet.new_plus("");  
    kodkontr.new_plus("");  
    debet.new_plus("");  
    kredit.new_plus("");  
   }
  saldo_shk_poi()
   {
    metka_poi=0;
   }
 };
