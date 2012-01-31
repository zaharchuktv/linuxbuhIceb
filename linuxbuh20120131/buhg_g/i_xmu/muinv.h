/*$Id: muinv.h,v 1.4 2011-01-13 13:49:51 sasa Exp $*/
/*16.06.2006	21.11.2004	Белых А.И.	muinv.h
Класс для воода данных для расчёта инвентаризационной ведомости
в материальном учёте
*/
class muinv_data
 {
  public:
  
  class iceb_u_str sklad;
  class iceb_u_str grupa;
  class iceb_u_str shet;
  class iceb_u_str kodmat;
  class iceb_u_str datao;
  int        metkainnom; //0-Все карточки 1-только с инвентарными номерами  

  iceb_u_spisok imaf;  //Имя файла с распечаткой
  iceb_u_spisok naimf; // Наименование распечатки
  
  muinv_data()
   {
    clear_data();
   }

  void clear_data()
   {
    metkainnom=0;
    sklad.new_plus("");
    grupa.new_plus("");
    shet.new_plus("");
    kodmat.new_plus("");
    datao.new_plus("");
    
   }
 };
 
