/*$Id: iceb_sql_flag.h,v 1.11 2011-02-21 07:36:08 sasa Exp $*/
/*23.08.2006	16.08.2004	Белых А.И.	iceb_sql_flag.h
Класс для установки/снятия флага
*/

class iceb_sql_flag
 {
  class iceb_u_str flag;
  short      metka; //0-флаг не установлен 1-установлен
  public:
  
  iceb_sql_flag(void); //Конструктор
  iceb_sql_flag(const char*); //Конструктор

  ~iceb_sql_flag();             //Деструктор
  int flag_on(void); //Установить флаг
  int flag_on(const char*); //Установить флаг
  int flag_off(void); //Снять флаг
 
 };
