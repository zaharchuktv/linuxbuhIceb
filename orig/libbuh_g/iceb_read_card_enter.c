/*$Id: iceb_read_card_enter.c,v 1.3 2011-01-13 13:49:59 sasa Exp $*/
/*27.04.2005	27.04.2005	Белых А.И.	iceb_read_card_enter.c
Чтение карточки и вставка результата в меню
*/
#include "iceb_libbuh.h"

#ifdef READ_CARD
extern tdcon           td_server; 
#endif 

void iceb_read_card_enter(class iceb_read_card_enter_data *data)
{

printf("iceb_read_card_enter\n");
iceb_refresh();

char kod_kart[30];
memset(kod_kart,'\0',sizeof(kod_kart));
#ifdef READ_CARD
int i=0;
if((i=iceb_read_card(td_server,kod_kart,sizeof(kod_kart),0,data->window)) < 0)
 {
  gtk_timeout_remove(data->timer);
  return;
 }    

if(i > 0) //Не дождались чтения
 return;

//printf("iceb_mous_klav_rc-kod_kart=%s\n",kod_kart);

gtk_entry_set_text(GTK_ENTRY(data->entry),iceb_u_toutf(kod_kart));
//Читаем только для того, чтобы не останавливался аварийно опрос считывателя
gtk_entry_get_text(GTK_ENTRY(data->entry));


#endif 

}
