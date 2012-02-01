/*$Id: iceb_u_strstrm.c,v 1.5 2011-01-13 13:50:05 sasa Exp $*/
/*11.10.2009    21.04.1994      Белых А.И.      iceb_u_strstrm.c
Определение вхождения части одной строки в другую
Переводит все перед сравнением в нижний регистр
Возвращаем 1 - есть образец
	   0 - нет
*/
#include <glib.h>
#include <ctype.h>
#include        <stdio.h>
#include        <string.h>
#include        "iceb_util.h"

int iceb_u_strstrm_nor(const char *s1,const char *s2);
int iceb_u_strstrm_utf8(const char *s1,const char *s2);

int iceb_u_strstrm(const char *s1, //Строка в которой ищют образец
const char *s2)  //Образец для поиска
{
int voz=0;
if(g_utf8_validate(s1,-1,NULL) == FALSE)
 {
//  printf("iceb_u_strstrm-koi8u\n");
  voz=iceb_u_strstrm_nor(s1,s2);
 }
else
 {
//  printf("iceb_u_strstrm-utf8\n");
  voz=iceb_u_strstrm_utf8(s1,s2);
 }
return(voz);
}
/*****************************************/

int iceb_u_strstrm_nor(const char *s1, //Строка в которой ищют образец
const char *s2)  //Образец для поиска
{
short           i,i1;
short           d2;
char            a1,a2;
d2=strlen(s2);
for(i=0; s1[i] != '\0';i++)
 {
  for(i1=0; s2[i1] != '\0' && s1[i+i1] != '\0'; i1++)
   {
/*******
    a1=iceb_u_prsimv(s1[i+i1],2);
    a2=iceb_u_prsimv(s2[i1],2);
***********/
    a1=tolower(s1[i+i1]);
    a2=tolower(s2[i1]);
  
    if(a1 != a2)
     break;
   }
  if(i1 >= d2)
   return(1);
 }
return(0);
}

/***************************/

int iceb_u_strstrm_utf8(const char *s1, //Строка в которой ищют образец
const char *s2)  //Образец для поиска
{
short           i=0,i1=0;
short           d2;
d2=strlen(s2);
char s11[strlen(s1)*2+1];
char s21[strlen(s2)*2+1];
memset(s11,'\0',sizeof(s11));
memset(s21,'\0',sizeof(s21));

strcpy(s11,s1);
strcpy(s21,s2);

iceb_u_tolower_str(s11);
iceb_u_tolower_str(s21);
for(i=0; s11[i] != '\0';i++)
 {
  for(i1=0; s21[i1] != '\0' && s11[i+i1] != '\0'; i1++)
   {
    if(s11[i+i1] != s21[i1])
     break;
   }
  if(i1 >= d2)
   {    
    return(1);
   }
 }
return(0);
}

