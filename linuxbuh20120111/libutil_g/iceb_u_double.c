/*$Id: iceb_u_double.c,v 1.10 2011-01-13 13:50:05 sasa Exp $*/
/*02.06.2009	15.12.2004	Белых А.И.	iceb_u_double.c
Класс для работы с double массивами переменной длинны
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"iceb_util.h"

//******************************************
//*************** DOUBLE ******************
//******************************************

/****************************/
/*Конструктор объекта       */
/****************************/
iceb_u_double::iceb_u_double(int optimz) //0-Без оптимизации 1-с оптимизацией
{
kolz=0;
optimiz=optimz;
masiv_double=NULL;
}
/****************************/
/*Конструктор объекта без параметров      */
/****************************/
iceb_u_double::iceb_u_double()
{
kolz=0;
optimiz=0;
masiv_double=NULL;
}
/****************************/
/*Деструктор класса*/
/****************************/
iceb_u_double::~iceb_u_double()
{
 iceb_u_double::free_class();
}
/*************************/
/*Вернуть количество элементов в массиве*/
/*************************************/

int iceb_u_double::kolih()
{
return(kolz);
}
/*************************************************/
/*Увеличить размер массива на заданное количество елементов*/
/*************************************************/

int iceb_u_double::plus_str(int znah) //Значение 
{
if((masiv_double=(double *)realloc(masiv_double,(kolz+znah)*sizeof(double))) == NULL)
 {
  printf("\n%s-Не могу добавить память !!!\n",__FUNCTION__);
  return(-1);
 }  

for(int ii=kolz; ii < kolz+znah; ii++)
  masiv_double[ii]=0.;
kolz+=znah;

return(0);
}  

/******************************************************/
/*Добавить значение в масив с увеличением его размера*/
/*******************************************************/

int iceb_u_double::plus(double znah, //Значение 
int zapis) //-1 в следующюю ячейку иначе в указанную
{
/*
printf("iceb_u_double::add_masiv_double znah=%f zapis=%d kol=%d kolz=%d\n",
znah,zapis,kol,kolz);
OSTANOV();
*/
if(zapis == -1)
 {
  if(kolz == 0)
   {
    if((masiv_double=(double *)calloc(1,sizeof(double))) == NULL)
     {
      printf("\n%s-Не могу выделить память для masiv_double double !!!\n",__FUNCTION__);
//      OSTANOV();
      return(-1);
     }
    kolz=1;
   }
  else
   {
    kolz++;
    if((masiv_double=(double *)realloc(masiv_double,kolz*sizeof(double))) == NULL)
     {
      printf("\n%s-Не могу добавить память для masiv_double double !!!\n",__FUNCTION__);
//      OSTANOV();
      return(-1);
     }  
   }
  masiv_double[kolz-1]=znah;
 }
else
 {
  if(zapis >= kolz)
   {
    printf("\n%s-Запрошенный елемент больше размера массива zapis=%d >= kolz=%d\n",__FUNCTION__,zapis,kolz);
//    OSTANOV();
    return(-1);
   }
  masiv_double[zapis]+=znah;
 } 
return(0);

}
/******************************************************/
/*Перезаписать значение в масив с увеличением его размера*/
/*******************************************************/

int iceb_u_double::new_plus(double znah, //Значение 
int zapis) //-1 в следующюю ячейку иначе в указанную
{
/*
printf("iceb_u_double::add_masiv_double znah=%f zapis=%d kol=%d kolz=%d\n",
znah,zapis,kol,kolz);
OSTANOV();
*/
if(zapis == -1)
 {
  if(kolz == 0)
   {
    if((masiv_double=(double *)calloc(1,sizeof(double))) == NULL)
     {
      printf("\n%s-Не могу выделить память для masiv_double double !!!\n",__FUNCTION__);
//      OSTANOV();
      return(-1);
     }
    kolz=1;
   }
  else
   {
    kolz++;
    if((masiv_double=(double *)realloc(masiv_double,kolz*sizeof(double))) == NULL)
     {
      printf("\n%s-Не могу добавить память для masiv_double double !!!\n",__FUNCTION__);
//      OSTANOV();
      return(-1);
     }  
   }
  masiv_double[kolz-1]=znah;
 }
else
 {
  if(zapis >= kolz)
   {
    printf("\n%s-Запрошенный елемент больше размера массива zapis=%d >= kolz=%d\n",__FUNCTION__,zapis,kolz);
//    OSTANOV();
    return(-1);
   }
  masiv_double[zapis]=znah;
 } 
return(0);

}

/*********************/
/*Уничтожение массива*/
/**********************/
void iceb_u_double::free_class()
{
kolz=0;
if(masiv_double != NULL)
 {
  free(masiv_double);
  masiv_double=NULL;
 }
}

/*****************************************************/
/*Найти в каком елементе массива находится значение*/
/*****************************************************/
int iceb_u_double::find(double znah)
{

for(int i=0; i < kolz; i++)
 if(znah == masiv_double[i])
  return(i);
return(-1);

}
/*****************************************************/
/*Вернуть значение нужного елемента массива*/
/*****************************************************/
double iceb_u_double::ravno(int elem)
{
if(elem > kolz)
 {
  printf("\n%s-Запрошенный елемент больше размера массива elem=%d > kolz=%d\n",__FUNCTION__,elem,kolz);
//  OSTANOV();  
 }
return(masiv_double[elem]);

}
/************************************/
/* Создать массив*/
/************************************/
int iceb_u_double::make_class(int razmer) //Размер массива
{
if(razmer == 0)
 return(0);
  
if(kolz != 0)
  iceb_u_double::free_class();

kolz=razmer;
if((masiv_double=(double *)calloc(razmer,sizeof(double))) == NULL)
  {
   printf("\n%s-Не могу выделить память для masiv_double double !!!\n",__FUNCTION__);
//   OSTANOV();
   return(1);
  }
return(0);
}
/*****************************/
/*Распечатать массив*/
/****************************/
void iceb_u_double::print_masiv()
{
printf("%s: kolz=%d\n",__FUNCTION__,kolz);
for(int i=0; i < kolz; i++)
  printf("%f\n",masiv_double[i]);
//OSTANOV();
}
/******************/
/*Очистить массив*/
/******************/
void iceb_u_double::clear_class()
{
for(int i=0; i < kolz;i++)
  masiv_double[i]=0.;
}

/**************************************/
/*Вернуть сумму всех элементов массива*/
/**************************************/
double iceb_u_double::suma(void)
{
double suma=0.;
for(int i=0; i < kolz; i++)
  suma+=masiv_double[i];
return(suma);
}
