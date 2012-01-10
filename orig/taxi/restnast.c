/*$Id: restnast.c,v 1.16 2011-02-21 07:36:21 sasa Exp $*/
/*29.10.2010	08.12.2004	Белых А.И.	restnast.c
Чтерие настроечного файла
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "i_rest.h"

short metka_crk=0; //0-расчёт клиентов по подразделениям 1-централизованный
short start_god_rest=0; //Стартовый год в выписке счетов
short startgod; /*Стартовый год просмотров в материальном учёте*/


int restnast()
{
start_god_rest=metka_crk=0;

iceb_u_str imaf("restnast.alx");
//iceb_imafn("restnast.alx",&imaf);
if(iceb_alxout(imaf.ravno(),NULL) != 0)
 return(1);
FILE *ff;
printf("22222222222222\n");
if((ff = fopen(imaf.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imaf.ravno(),gettext("Аварийное завершение работы программы !"),errno,NULL);
  return(1);
 }    
printf("22222222222222\n");

char stroka[1024];
char stroka1[1024];

while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  iceb_u_polen(stroka,stroka1,sizeof(stroka1),1,'|');
  if(iceb_u_SRAV(stroka1,"Стартовый год",0) == 0)
   {
//    printf("Стартовый год\n");
    iceb_u_polen(stroka,stroka1,sizeof(stroka1),2,'|');
    start_god_rest=atoi(stroka1);     
    continue;
   }

  if(iceb_u_SRAV(stroka1,"Централизованный расчёт клиентов",0) == 0)
    {
     iceb_u_polen(stroka,stroka1,sizeof(stroka1),2,'|');
     if(iceb_u_SRAV(stroka1,"Вкл",1) == 0)
        metka_crk=1;
     else
        metka_crk=0;
     
     continue;
    }

 }
fclose(ff);
unlink(imaf.ravno()); 

//iceb_imafn("matnast.alx",&imaf);
imaf.new_plus("matnast.alx");
if(iceb_alxout(imaf.ravno(),NULL) != 0)
 return(1);
 
if((ff = fopen(imaf.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imaf.ravno(),"",errno,NULL);
  return(0);
 }    

while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  iceb_u_polen(stroka,stroka1,sizeof(stroka1),1,'|');
  if(iceb_u_SRAV(stroka1,"Стартовый год",0) == 0)
   {
//    printf("Стартовый год\n");
    iceb_u_polen(stroka,stroka1,sizeof(stroka1),2,'|');
    startgod=atoi(stroka1);     
    continue;
   }

 }
fclose(ff);
unlink(imaf.ravno()); 

return(0);
}

