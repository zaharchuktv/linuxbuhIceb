/*$Id: taxinast.c,v 1.4 2011-02-21 07:36:21 sasa Exp $*/
/*31.03.2005	31.03.2005	Белых А.И.	taxinast.c
Чтение настроек для диспетчеризации такси
*/


#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "taxi.h"

short start_god_taxi=0;


int taxinast()
{
char stroka[1024];
char stroka1[1024];

start_god_taxi=0;
iceb_u_str imaf("taxinast.alx");
//iceb_imafn("taxinast.alx",&imaf);
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
//    printf("Стартовый год\n");
    iceb_u_polen(stroka,stroka1,sizeof(stroka1),2,'|');
    start_god_taxi=atoi(stroka1);     
    continue;
   }
 }
fclose(ff);
unlink(imaf.ravno()); 
return(0);
}




