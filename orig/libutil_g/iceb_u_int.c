/*$Id: iceb_u_int.c,v 1.13 2011-01-13 13:50:05 sasa Exp $*/
/*02.04.2010	15.12.2004	Белых А.И.	iceb_u_int.c
Класс для работы с int-массивами переменной длинны
*/
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"iceb_util.h"


//******************************************
//*************** INT ******************
//******************************************

/****************************/
/*Конструктор объекта       */
/****************************/
iceb_u_int::iceb_u_int(int optimz) //0-Без оптимизации 1-с оптимизацией
{
kolz=0;
optimiz=optimz;
masiv_int=NULL;
}
/****************************/
/*Конструктор объекта без параметров      */
/****************************/
iceb_u_int::iceb_u_int()
{
kolz=0;
optimiz=0;
masiv_int=NULL;
}
/****************************/
/*Деструктор класса*/
/****************************/
iceb_u_int::~iceb_u_int()
{
 iceb_u_int::free_class();
}
/******************************************************/
/*Добавить значение в масив с увеличением его размера*/
/*******************************************************/

int iceb_u_int::plus(int znah) //Значение 
{
return(plus(znah,-1));
}
/******************************************************/
/*Добавить значение в масив с увеличением его размера преобразовав из символьного предсталения в целое число*/
/*******************************************************/

int iceb_u_int::plus(char *znah) //Значение 
{
return(plus(atoi(znah),-1));
}
/******************************************************/
/*Добавить значение в масив с увеличением его размера*/
/*******************************************************/

int iceb_u_int::plus(int znah, //Значение 
int zapis) //-1 в следующюю ячейку иначе в указанную
{
/*
printf("iceb_u_int::add_masiv_int znah=%f zapis=%d kol=%d kolz=%d\n",
znah,zapis,kol,kolz);
OSTANOV();
*/
if(zapis == -1)
 {
  if(kolz == 0)
   {
    if((masiv_int=(int *)calloc(1,sizeof(int))) == NULL)
     {
      printf("\n%s-Не могу выделить память для masiv_int int !!!\n",__FUNCTION__);
      return(-1);
     }
    kolz=1;
   }
  else
   {
    kolz++;
    if((masiv_int=(int *)realloc(masiv_int,kolz*sizeof(int))) == NULL)
     {
      printf("\n%s-Не могу добавить память для masiv_int int !!!\n",__FUNCTION__);
      return(-1);
     }  
   }
  masiv_int[kolz-1]=znah;
 }
else
 {
  if(zapis >= kolz)
   {
    printf("\n%s-Запрошенный елемент больше размера массива zapis=%d >= kolz=%d\n",__FUNCTION__,zapis,kolz);
    return(-1);
   }
  masiv_int[zapis]+=znah;
 } 
return(0);

}
/******************************************************/
/*Добавить значение в масив с увеличением его размера*/
/*******************************************************/

int iceb_u_int::new_plus(int znah, //Значение 
int zapis) //-1 в следующюю ячейку иначе в указанную
{
/*
printf("iceb_u_int::add_masiv_int znah=%f zapis=%d kol=%d kolz=%d\n",
znah,zapis,kol,kolz);
OSTANOV();
*/
if(zapis == -1)
 {
  if(kolz == 0)
   {
    if((masiv_int=(int *)calloc(1,sizeof(int))) == NULL)
     {
      printf("\n%s-Не могу выделить память для masiv_int int !!!\n",__FUNCTION__);
      return(-1);
     }
    kolz=1;
   }
  else
   {
    kolz++;
    if((masiv_int=(int *)realloc(masiv_int,kolz*sizeof(int))) == NULL)
     {
      printf("\n%s-Не могу добавить память для masiv_int int !!!\n",__FUNCTION__);
      return(-1);
     }  
   }
  masiv_int[kolz-1]=znah;
 }
else
 {
  if(zapis >= kolz)
   {
    printf("\n%s-Запрошенный елемент больше размера массива zapis=%d >= kolz=%d\n",__FUNCTION__,zapis,kolz);
    return(-1);
   }
  masiv_int[zapis]=znah; /*Это единственное но очень существенное отличие от предыдущей подпрограммы*/
 } 
return(0);

}

/*********************/
/*Уничтожение массива*/
/**********************/
void iceb_u_int::free_class()
{

kolz=0;
if(masiv_int != NULL)
 free(masiv_int);
}

/*****************************************************/
/*Найти в каком елементе массива находится значение*/
/*****************************************************/
int iceb_u_int::find(int znah)
{

for(int i=0; i < kolz; i++)
 if(znah == masiv_int[i])
  return(i);
return(-1);

}
/*****************************************************/
/*Найти в каком елементе массива находится значение*/
/*****************************************************/
int iceb_u_int::find(char *znah)
{

return(find(atoi(znah)));

}

/*****************************************************/
/*Вернуть значение нужного елемента массива*/
/*****************************************************/
int iceb_u_int::ravno(int elem)
{
if(elem > kolz)
 {
  printf("\n%s-Запрошенный елемент больше размера массива elem=%d > kolz=%d\n",__FUNCTION__,elem,kolz);
//  OSTANOV();  
  return(0);  
 }
if(kolz == 0)
 {
  printf("\n%s-Количество элементов в массиве равно нолю !!!\n",__FUNCTION__);
//  OSTANOV();  
  return(0);  
 }
return(masiv_int[elem]);

}
/***************************/
/*Вернуть елемент в символьном виде*/
/***********************************/
char *iceb_u_int::ravno_char(int elem)
{
static char bros[64];
sprintf(bros,"%d",ravno(elem));
return(bros);
}

/************************************/
/* Создать массив*/
/************************************/
int iceb_u_int::make_class(int razmer) //Размер массива
{
if(razmer == 0)
 return(0);

if(kolz != 0)
 iceb_u_int::free_class();

kolz=razmer;
if((masiv_int=(int *)calloc(razmer,sizeof(int))) == NULL)
  {
   printf("\n%s-Не могу выделить память для masiv_int int !!!\n",__FUNCTION__);
   return(-1);
  }
return(0);
}
/*****************************/
/*Распечатать массив*/
/****************************/
void iceb_u_int::print_masiv()
{
printf("%s: kolz=%d\n",__FUNCTION__,kolz);
for(int i=0; i < kolz; i++)
  printf("%d\n",masiv_int[i]);
//OSTANOV();
}

/************************************************/
/*Вернуть максимальное значение елемента массива*/
/************************************************/
int iceb_u_int::ravno_max()
{
int	max=0;
for(int i=0; i < kolz;i++)
 if(masiv_int[i] > max)
  max=masiv_int[i];
return(max);
}
/****************************************/
/*Вернуть количество элементов в массиве*/
/****************************************/
int iceb_u_int::kolih()
{
return(kolz);
}
/***********************/
/*Получить сумму всех елементов массива*/
/**************************/
int iceb_u_int::suma()
{
int suma=0;
for(int i=0; i < kolz;i++)
  suma+=masiv_int[i];
return(suma);
}
/***************************/
/*Обнулить весь массив*/
/*************************/
void iceb_u_int::clear_class()
{
for(int i=0; i < kolz;i++)
  masiv_int[i]=0;
}