/*$Id: iceb_u_pole1.c,v 1.6 2011-01-13 13:50:05 sasa Exp $*/
/*25.03.2009    11.11.1996      Белых А.И.      iceb_u_pole1.c
Поиск в массиве заданного образца
Если вернули "0" значит образец найден
если вернули "1" - нет
В переменной voz возвращаем номер поля в котором нашли образец
*/
#include                <stdio.h>
#include                <string.h>
#include                "iceb_util.h"

short iceb_u_pole1(const char *mas, /*массив в котором ищем образец*/
const char *obr, /*образец*/
char raz, /*разделитель полей*/
short met, /*0-полное сравнение 1-не полное 2-полное с преобразованием 3-не полное с преобразованием 4-поиск образца в любом месте строки с перобрзованием*/
int *voz) /*возвращаемый номер поля в котором найден образец*/
{
short           i,i1;
char            obr1[512];
short		metkat=met;
char		mast[strlen(mas)+1];
char		obrt[strlen(obr)+1];

*voz=0;

if(mas == NULL)
  return(1);
  
strcpy(obrt,obr);
strcpy(mast,mas);

if(met == 2 || met == 3)
 {
/***********
  i1=strlen(mast);
  for(i=0; i < i1; i++)
    mast[i]=iceb_u_prsimv(mast[i],2);

  i1=strlen(obrt);
  for(i=0; i < i1; i++)
    obrt[i]=iceb_u_prsimv(obrt[i],2);
****************/
  iceb_u_tolower_str(mast);
  iceb_u_tolower_str(obrt);
  if(met == 2)
   metkat=0;

  if(met == 3)
   metkat=1;

 }


memset(obr1,'\0',sizeof(obr1));
i1=0;
for(i=0; mast[i] != '\0' && mast[i] != '\n';i++)
 {
  if(mast[i] != raz)
   {
    obr1[i1]=mast[i];
    i1++;
   }
  else
   {

    obr1[i1]='\0';
    /*
    printw("obr-%s obr1-%s\n",obr,obr1);
    refresh();
      */
    if(met == 4)
     {
      if(iceb_u_strstrm(obrt,obr1) == 1) /*Перед сравнением переводит всё в один регистр*/
       return(0);
     }
    else
     {
      if(iceb_u_SRAV(obrt,obr1,metkat) == 0)
       return(0);
     }
    *voz=*voz+1;
    i1=0;
   }
 }
/*Последнее поле*/
obr1[i1]='\0';
if(met == 4)
 {
  if(iceb_u_strstrm(obrt,obr1) == 1) /*Перед сравнением переводит всё в один регистр*/
   return(0);
 }
else
 {
  if(iceb_u_SRAV(obrt,obr1,metkat) == 0)
     return(0);
 }

return(1);
}
