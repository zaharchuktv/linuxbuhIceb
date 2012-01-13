/*$Id: glktmpw.h,v 1.4 2011-01-13 13:49:50 sasa Exp $*/
/*11.02.2010	24.02.2009	Белых А.И.	glktmp.h
Класс для хранения данных при расчёте главной книги
*/

class GLKTMP
 {
private:
/****************
  int              kol;    //Количество элементов
  class iceb_u_spisok           shkkmt0; //Счет|код контрагента - до периода
  class iceb_u_spisok           shkkmt1; //Счет|код контрагента - период
  class iceb_u_double deb0; //Дебет - до периода
  class iceb_u_double kre0; //Кредит - до периода
  class iceb_u_double deb1; //Дебет - период
  class iceb_u_double kre1; //Кредит - период
******************/
public:
  class iceb_u_spisok           masf; /*Общий массив счетов*/
  class iceb_u_spisok           msr;  /*Массив счетов с развернутым сальдо*/
  class iceb_u_double sl; /*Массив до периода (шахматка)*/
  class iceb_u_double pe; /*Массив за период (шахматка)*/
  class iceb_u_double ssd; /*Массив стартовых сальдо по счетам*/
  class iceb_u_double ssk; /*Массив стартовых сальдо по счетам*/

  class iceb_u_double deb_end; //Массив конечного сальдо по счетам
  class iceb_u_double kre_end; //Массив конечного сальдо по счетам
/***************
  GLKTMP(int)
  {
  kol=0;    //Количество элементов
  }
  ~GLKTMP()
  {
  kol=0;    //Количество элементов
  }

  //Записать данные
//  int GLKTMP_zapis(char*,char*,int,double,double);

  //Поиск записи
//  int GLKTMP_poisk(char*,char*,int,double*,double*);
**********************/
 };
