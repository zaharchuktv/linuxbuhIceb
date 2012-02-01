/*$Id: iceb_connect_dserver.c,v 1.9 2011-01-13 13:49:58 sasa Exp $*/
/*14.11.2008	28.04.2005	Белых А.И.	iceb_connect_dserver.c
Соединение с серверером считывателя карточек
*/
#include "iceb_libbuh.h"

#ifdef READ_CARD
tdcon           td_server; 
#endif

int iceb_connect_dserver(const char *ima_komp,const char *name_dev_card)
{

#ifdef READ_CARD
if((td_server=td_open_server(ima_komp,name_dev_card)) < 0)
 {
  iceb_menu_soob("Ошибка соединения с сервером для работы со считывателями карточек",NULL);
  return(1);
 }
else 
 printf("Сервер открыт успешно\n");
#endif
return(0);
}
