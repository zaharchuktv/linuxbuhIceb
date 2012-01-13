/*$Id: iceb_close_dserver.c,v 1.9 2011-01-13 13:49:58 sasa Exp $*/
/*16.08.2005	28.04.2005	Белых А.И.	iceb_close_dserver.c
Прекращение работы с сервером считывателя карточек
*/
#include "iceb_libbuh.h" //Обязятелен
#ifdef READ_CARD
extern tdcon           td_server; 
#endif

void iceb_close_dserver()
{

#ifdef READ_CARD
printf("Закрыта работа клиента\n");
td_close_server(td_server);
#endif

}
