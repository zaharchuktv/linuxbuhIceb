/*$Id: iceb_get_dev.h,v 1.6 2011-01-13 13:49:59 sasa Exp $*/
/*12.09.2005	13.06.2005	Белых А.И.	iceb_get_dev.h
Класс для хранения прочитанных настроек по рабочему месту с которого запустрилась программа
*/

class iceb_get_dev_data
 {
  public:
   iceb_u_str ima_komp;  //компьютер 
   iceb_u_str name_dev_card; //имя порта к которому подключен считыватель карточек
   short metka_screen; //0-сенсорный экран не используетс 1-используется
   short metka_klav;   //0-клавиатура используется 1-не используется
   short metka_mpr; //0-однопользовательская работа 1-многопользовательская работа
   char *workid_answer; //Ответ функции workid
  
   iceb_get_dev_data()
    {
     ima_komp.plus("");
     name_dev_card.plus("");
     metka_screen=0;
     metka_klav=0;
     workid_answer=NULL;
     metka_mpr=0;
    }
 };
