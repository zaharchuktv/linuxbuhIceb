/*$Id: iceb_read_card.c,v 1.9 2011-01-13 13:49:59 sasa Exp $*/
/*26.04.2005	16.04.2005	Белых А.И.	iceb_read_card.c
Чтение данных с устройства считывающего информацию с карточек
Если вернули 0 - прочитали
             1 - Не прочитали
            <0 - ошибка чтения

*/
#include "iceb_libbuh.h"
#ifdef READ_CARD

int iceb_read_card(tdcon td_server,char *kod_kart,int razmer,int time_out,GtkWidget *wpredok)
{


int i=0;
char strsql[500];
int kod_oh=0;
sprintf(strsql,"FSTART: READT\n\%d\n",time_out);
//printf("iceb_read_card-%s",strsql);
if((i=td_write_to_server(td_server,strsql)) < 0)
 {
  sprintf(strsql,"Ошибка инициализации считывывателя карточек %d",i); 
  iceb_menu_soob(strsql,wpredok);
  return(i);
 }
char bros[512];
memset(bros,'\0',sizeof(bros));
if((i=td_read_from_server(td_server,bros,sizeof(bros))) < 0)
 {
  sprintf(strsql,"Ошибка чтения считывывателя карточек %d",i); 
  iceb_menu_soob(strsql,wpredok);
  return(i);
 }
//printf("iceb_read_card-i=%d bros=%s\n",i,bros);

if(i == 1) //Не дождались чтения
 return(1); 

//printf("bros=%s\n",bros);
sscanf(bros,"%d %s",&kod_oh,strsql);
if(kod_oh < 0)
 {
  
  sprintf(strsql,"Ошибка считывания карточки !\n%s",bros);
  iceb_menu_soob(strsql,wpredok);    
  return(kod_oh);
 }
memset(kod_kart,'\0',razmer);
strncpy(kod_kart,strsql,razmer-1);


return(0);

}
#endif
