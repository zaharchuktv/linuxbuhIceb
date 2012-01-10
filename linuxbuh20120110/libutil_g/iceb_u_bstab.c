/*$Id: iceb_u_bstab.c,v 1.9 2011-01-13 13:50:05 sasa Exp $*/
/*15.03.2010    28.09.1990      Белых А.И.      iceb_u_bstab.c
программа вставки в одну символьную строку другой символьной
строки
*/
#include <errno.h>
#include <stdlib.h>
#include <glib.h>
#include "iceb_util.h"

void  iceb_u_bstab_osk(char *STI,const char *STB,int K1,int K2,int hag);
void  iceb_u_bstab_utf8(char *STI,const char *STB,int K1,int K2,int hag);

void  iceb_u_bstab(char *STI, //строка в которую вставляют
short STB,  //строка которую вставляют
int K1,      //номер позиции с которой вставляют
int K2,      //номер позиции до которой вставляют
int hag)     //шаг вставки
{
char str[112];
sprintf(str,"%d",STB);
return(iceb_u_bstab(STI,str,K1,K2,hag));

}
/**************************************/

void  iceb_u_bstab(char *STI, //строка в которую вставляют
const char *STB,  //строка которую вставляют
int K1,      //номер позиции с которой вставляют
int K2,      //номер позиции до которой вставляют
int hag)     //шаг вставки
{
if(strlen(STB) == 0 || STB == NULL)
 return;
if(g_utf8_validate(STI,-1,NULL) == TRUE || g_utf8_validate(STB,-1,NULL) == TRUE) /*текст в utf8 кодировке*/
  iceb_u_bstab_utf8(STI,STB,K1,K2,hag);
else
  iceb_u_bstab_osk(STI,STB,K1,K2,hag);

}
/**************************/

void  iceb_u_bstab_osk(char *STI, //строка в которую вставляют
const char *STB,  //строка которую вставляют
int K1,      //номер позиции с которой вставляют
int K2,      //номер позиции до которой вставляют
int hag)     //шаг вставки
{
int    N1,N;
short  metka=0;
//Проверяем если есть до позиции вставки нулевые байты - заменяем на пробелы

for(N=0;N<K1;N++)
 {
  if(STI[N] == '\n')
   {
    metka++;
    STI[N]=' ';
   }
  
  if(STI[N] == '\0')
   STI[N]=' ';
 }
 
N1=0;
for(N=K1;N<K2;N+=hag)
 {
  if(STI[N] == '\n')
    metka++;
  if(STB[N1] == '\0')
   break;
  STI[N]=STB[N1];
  N1++;
 }

if(metka > 0)
  STI[N]='\n';

}
/****************************/
/*****************************/
void  iceb_u_bstab_utf8(char *STI, //строка в которую вставляют
const char *STB,  //строка которую вставляют
int K1,      //номер позиции с которой вставляют
int K2,      //номер позиции до которой вставляют
int hag)     //шаг вставки
{
int    N1,N;
short  metka=0;
size_t dlinasti=1024; /*Длинна сроки в которую вставляем может не иметь заполнения символами*/
if((size_t)iceb_u_strlen(STI)+1 > dlinasti)
 dlinasti=iceb_u_strlen(STI)+1;
wchar_t sti[dlinasti];

size_t dlinastb=iceb_u_strlen(STB)+1;
size_t utf8_dlina=dlinasti*sizeof(wchar_t);
wchar_t stb[dlinastb];

memset(sti,'\0',sizeof(sti));
memset(stb,'\0',sizeof(stb));

if((mbstowcs(sti,STI,dlinasti)) == (size_t)-1)
 {
  printf("1-%s-Ошибка преобразования в utf8 строку %d\n%s\n%s\n",
  __FUNCTION__,errno,strerror(errno),STI);
  return;
 }
if((mbstowcs(stb,STB,dlinastb)) == (size_t)-1)
 {
  printf("1-%s-Ошибка преобразования в utf8 строку %d\n%s\n%s\n",
  __FUNCTION__,errno,strerror(errno),STB);
  return;
 }

//Проверяем если есть до позиции вставки нулевые байты - заменяем на пробелы
for(N=0;N<K1;N++)
 {
  if(sti[N] == '\n')
   {
    metka++;
    sti[N]=' ';
   }
  
  if(sti[N] == '\0')
   sti[N]=' ';
 }
 
N1=0;
for(N=K1;N<K2;N+=hag)
 {
  if(sti[N] == '\n')
    metka++;
  if(stb[N1] == '\0')
   break;
  sti[N]=stb[N1];
  N1++;
 }

if(metka > 0)
  sti[N]='\n';

if((wcstombs(STI,sti,utf8_dlina)) == (size_t)-1)
 {
  printf("3-%s-Ошибка преобразования в односимвольную строку %d\n%s\n",
  __FUNCTION__,errno,strerror(errno));
 }

}
