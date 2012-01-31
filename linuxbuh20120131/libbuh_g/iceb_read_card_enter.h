/*$Id: iceb_read_card_enter.h,v 1.2 2011-01-13 13:49:59 sasa Exp $*/
/*27.04.2005	27.04.2005	Белых А.И.	iceb_read_card_enter.h
Класс для чтения карточек и записи данных в меню
*/

class iceb_read_card_enter_data
 {
  public:
  GtkWidget *window;
  GtkWidget *entry;
  guint     timer;
 };

