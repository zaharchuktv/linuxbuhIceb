/*$Id: iceb_u_polen.c,v 1.16 2011-01-13 13:50:05 sasa Exp $*/
/*03.06.2009    16.03.1989      Белых А.И.      iceb_u_polen.c
Подпрограмма извлечения из строки символов нужного номера поля
с контролем размера массива
Если вернули "0" значит заказанный номер поля существует
если вернули "1" - нет
*/
#include                <stdio.h>
#include                <string.h>
#include <stdlib.h>
#include "iceb_util.h"

/***********************************/
int     iceb_u_polen(const char *D, //Строка из которой извлекают
double *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char strok[strlen(D)+1];
strok[0]='\0';

if(iceb_u_polen(D,strok,sizeof(strok),N,R) == 0)
 {
  *K=iceb_u_atof(strok);
  return(0);
 }
return(1);
}
/***********************************/
int     iceb_u_polen(const char *D, //Строка из которой извлекают
float *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char strok[strlen(D)+1];
strok[0]='\0';
if(iceb_u_polen(D,strok,sizeof(strok),N,R) == 0)
 {
  *K=iceb_u_atof(strok);
  return(0);
 }
return(1);
}
/***********************************/
int     iceb_u_polen(const char *D, //Строка из которой извлекают
int *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char strok[1024];
if(iceb_u_polen(D,strok,sizeof(strok),N,R) == 0)
 {
  *K=atoi(strok);
  return(0);
 }
return(1);
}
/***********************************/
int     iceb_u_polen(const char *D, //Строка из которой извлекают
short *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char strok[strlen(D)+1];
strok[0]='\0';
if(iceb_u_polen(D,strok,sizeof(strok),N,R) == 0)
 {
  *K=atoi(strok);
  return(0);
 }
return(1);
}

/***********************************/
int     iceb_u_polen(const char *D, //Строка из которой извлекают
class iceb_u_spisok *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char stroka[strlen(D)+1];
memset(stroka,'\0',sizeof(stroka));
if(iceb_u_polen(D,stroka,sizeof(stroka),N,R) == 0)
 {
  K->plus(stroka);
  return(0);
 }
return(1);
}

/***********************************/
int iceb_u_polen(const char *D, //Строка из которой извлекают
class iceb_u_str *K, //Поле которое выделили из строки
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
char stroka[strlen(D)+1];
memset(stroka,'\0',sizeof(stroka));
if(iceb_u_polen(D,stroka,sizeof(stroka),N,R) == 0)
 {
  K->new_plus(stroka);
  return(0);
 }
K->new_plus("");
return(1);
}
/*************************************/
int iceb_u_polen(const char *D, //Строка из которой извлекают
char *K, //Поле которое выделили из строки
int razmer, //Размер К массива
int N, //Порядковый номер поля которое нужно выделить
char R) //Разделитель которым ограничено поле
{
int             I,Z,V;
short           l,ll;


/*Проверяем наличие хоть одного символа*/
Z=0;
for(I=0;D[I] != '\0' && D[I] != '\r' ;I++)
 {
  if(D[I]==R)
   {
    Z++;
    break;
   } 
 }
if( Z == 0)
 {
  K[0]='\0';
  return(1);
 }

Z=1;

for(I=0;D[I] != '\0' && D[I] != '\r' ;I++)
 {
  if(D[I]==R)
   Z++;
  if(Z == N)
   {
    if(N>1)
     I++;
    V=0;
    for(; D[I]!=R && V < razmer-1 ;I++)
     {
      if(D[I] == '\0' && N == 1)
       {
	K[0]='\0';
	return(1);
       }
      if(D[I] == '\0' || D[I] == '\r')
	break;
      K[V++]=D[I];
     }
    K[V]='\0';
    l=strlen(K);
    /*Убираем пробелы в конце строки*/
/*    printf("%d\n",l);*/
    for(ll=l-1; K[ll] == ' '|| K[ll] == '\t'|| K[ll] == '\n' ||
    K[ll] == '\r';ll--)
     {
      K[ll]='\0';
     }
/*    printf("R - %c поле - %s  %d %d \n",R,K,strlen(K),K[l]);*/
    return(0);
   }
  }
K[0]='\0';
return(1);
}
