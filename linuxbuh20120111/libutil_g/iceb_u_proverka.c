/*$Id: iceb_u_proverka.c,v 1.8 2011-04-06 09:49:21 sasa Exp $*/
/*05.04.2011	06.05.2001	Белых А.И.	iceb_u_proverka.c
Проверка реквизита
Если вернули 0 подходит
             1 нет
*/
#include	<stdio.h>
#include	<string.h>
#include	"iceb_util.h"


int iceb_u_proverka(const char *etalon, 
int rekvizit,
int metka, /*0-полное сравнение 1-совпаденние начала
             2 полное с преобразованием 3 неполное с преобразованием
             4-наличие цепочки символов в любом месте строки с преобразованием
             Должно совпадать с подпрограммой pole1*/
int variant) /*0-если еталона нет подходит 1-если эталона нет неподходит*/
{

char bros[64];
sprintf(bros,"%d",rekvizit);

return(iceb_u_proverka(etalon,bros,metka,variant));

}

int iceb_u_proverka(const char *etalon, 
const char *rekvizit,
int metka, //0-полное сравнение 1-совпаденние начала
int variant) //0-если еталона нет подходит 1-если эталона нет неподходит
{
int i1=0;

if(variant == 0)
 {
  if(etalon == NULL)
    return(0);
  if(etalon[0] == '\0')
    return(0);
  if(rekvizit == NULL)
    return(0);
 }
if(variant == 1)
 {
  if(etalon == NULL)
    return(1);
  if(etalon[0] == '\0')
    return(1);
  if(rekvizit == NULL)
    return(1);
 }
int metka_srav=metka;

if(metka_srav == 2)
 metka_srav=0;
if(metka_srav == 3)
 metka_srav=1;

class iceb_u_str etalon1(etalon);
class iceb_u_str rekvizit1(rekvizit);

i1=strlen(etalon);
if(etalon[i1-1] == '*' && etalon[i1-2] == ',')
  metka=1;

if(etalon[0] != '-')
 {
  if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) != 0)
   {
    if(metka == 4)
     {
      if(iceb_u_strstrm(rekvizit,etalon) == 0)
       return(1);
     }
    else
     {
      if(metka == 2 || metka == 3)
       {
        if(iceb_u_SRAV(etalon1.ravno_tolower(),rekvizit1.ravno_tolower(),metka_srav) != 0)
         return(1);
       }
      else
       if(iceb_u_SRAV(etalon,rekvizit,metka_srav) != 0)
        return(1);
     }
   }
 }

if(etalon[0] == '-')
 {
  if(metka == 4) 
   {
    if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) == 0 || iceb_u_strstrm(rekvizit,etalon) == 1)
     return(1);
   }
  else
   {
    if(metka == 2 || metka == 3) /*с преобразованием*/
     {
      if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) == 0 || iceb_u_SRAV(etalon1.ravno_tolower(),rekvizit1.ravno_tolower(),metka_srav) == 0)
       return(1);
     }
    else
    if(iceb_u_pole1(etalon,rekvizit,',',metka,&i1) == 0 || iceb_u_SRAV(etalon,rekvizit,metka_srav) == 0)
      return(1);
   }
 }
return(0);

}
