/*$Id: iceb_lock_tables.h,v 1.9 2011-01-13 13:49:59 sasa Exp $*/
/*18.02.2009	22.02.2004 	Белых А.И.	iceb_lock_tables.h
Класс для блокировки/разблокировки таблиц
*/

class iceb_lock_tables
 {
  short metka;  //0-не заблокированы 1-заблокированы
  public:
  
   
  iceb_lock_tables(); //конструктор без параметров

  iceb_lock_tables(const char*); //конструктор с параметрами
  iceb_lock_tables(iceb_u_str*); //конструктор с параметрами

  ~iceb_lock_tables(); //деструктор

  int lock(const char *tabl); //блокировка таблиц
  int lock(iceb_u_str *tabl); //блокировка таблиц
  int unlock(); //разблокировка таблиц

 };
