/*$Id: read_nast_indw.h,v 1.5 2011-04-06 09:49:17 sasa Exp $*/
/*04.04.2011	30.01.2009	Белых А.И.	read_nast_indw.h
Класс для хранения данных для расчёта индексации зарплаты
*/

class index_ua
 {
  public:
/***********Индексация начислений *******************/
  class iceb_u_int mr; //Месяцы расчёта
  class iceb_u_int gr; //Года расчёта
//  class iceb_u_double prog_min;
  
  /****************/
  class iceb_u_int mi; //Месяцы коэффициентов 
  class iceb_u_int gi; //Годы коэффициентов
  class iceb_u_double koef; //Коэффициенты индексации

/********** Индексация зарплаты выплаченной с задержкой**************/
  class iceb_u_int miv; //Месяцы коэффициентов 
  class iceb_u_int giv; //Годы коэффициентов
  class iceb_u_double koefv; //Коэффициенты индексации
  int kninvz; /*Код начисления индексации не вовремя выплаченной зарплаты*/
  int kodpzns; /*Код перечисления зарплаты на счёт*/
  int kodpzvk; /*Код получения зарплаты в кассе*/

  void free_class()
   {
    mr.free_class();
    gr.free_class();
//    prog_min.free_class();
    mi.free_class();
    gi.free_class();
    koef.free_class();
    
    miv.free_class();
    giv.free_class();
    koefv.free_class();
   }
 };
