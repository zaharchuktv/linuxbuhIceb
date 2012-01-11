/*$Id: poiinpdw.h,v 1.2 2011-01-13 13:49:52 sasa Exp $*/
/*16.11.2007	16.11.2007	Белых А.И.	poiinpdw.h
Класс для чтени переменных данных на инвентарный номер
*/

class poiinpdw_data
 {
  public:
  class iceb_u_str shetu; //Счет учета
  class iceb_u_str hzt; //Шифр затрат
  class iceb_u_str hau; //Шифр аналитического учета
  class iceb_u_str hna; //Шифр нормы амортотчислений для налогового учета
  float popkf; //Поправочный коэффициент для налогового учета
  int soso; //Состояние объекта
  class iceb_u_str nomz;  //Номерной знак
  short mz; short gz;  //дата записи
  class iceb_u_str hnaby; //Шифр нормы амортотчислений для бух. учета
  float popkfby;  ///Поправочный коэффициент для бух. учета
  
  poiinpdw_data()
   {
    clear();
   }

  void clear()
   {
    shetu.new_plus("");
    hzt.new_plus("");
    hau.new_plus("");
    hna.new_plus("");
    popkf=0;
    soso=0;
    nomz.new_plus("");
    mz=0;
    gz=0;
    hnaby.new_plus("");
    popkfby=0;
   }

 };

