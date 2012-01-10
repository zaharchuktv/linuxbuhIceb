/*$Id: restnast_k.c,v 1.9 2011-02-21 07:36:21 sasa Exp $*/
/*29.10.2010	08.12.2004	Белых А.И.	restnast_k.c
Чтерие настроечного файла
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "i_rest.h"

short metka_skreen=0; //0-сенсорный экран не используется 1-используется
short  start_god_rest=0;
iceb_u_str name_dev_card;
iceb_u_str ima_komp;  

int restnast_k()
{
char stroka[1024];
char stroka1[1024];
metka_skreen=start_god_rest=0;
ima_komp.new_plus("");
name_dev_card.new_plus("");

iceb_u_str imaf("restnast.alx");
//iceb_imafn("restnast.alx",&imaf);
if(iceb_alxout(imaf.ravno(),NULL) != 0)
 return(1);
FILE *ff;

if((ff = fopen(imaf.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imaf.ravno(),gettext("Аварийное завершение работы программы !"),errno,NULL);
  return(1);
 }    


while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  iceb_u_polen(stroka,stroka1,sizeof(stroka1),1,'|');

  if(iceb_u_SRAV(stroka1,"Стартовый год",0) == 0)
    {
     iceb_u_polen(stroka,stroka1,sizeof(stroka1),2,'|');
     start_god_rest=atoi(stroka1);     
     continue;
    }

  if(iceb_u_SRAV(stroka1,"Сенсорный экран",0) == 0)
    {
     iceb_u_polen(stroka,stroka1,sizeof(stroka1),2,'|');
     if(iceb_u_SRAV(stroka1,"Вкл",1) == 0)
        metka_skreen=1;
     else
        metka_skreen=0;
     
     continue;
    }
  if(iceb_u_SRAV(stroka1,"Порт",0) == 0)
   {
    iceb_u_polen(stroka,stroka1,sizeof(stroka1),2,'|');
     name_dev_card.new_plus(stroka1);
     continue;
   }
  if(iceb_u_SRAV(stroka1,"Имя компъютера",0) == 0)
   {
     iceb_u_polen(stroka,stroka1,sizeof(stroka1),2,'|');
     ima_komp.new_plus(stroka1);
     continue;
   }
 }
fclose(ff);
unlink(imaf.ravno());
return(0);
}
