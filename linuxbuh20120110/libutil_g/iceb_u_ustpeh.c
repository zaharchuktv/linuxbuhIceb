/*$Id: iceb_u_ustpeh.c,v 1.4 2011-03-09 08:21:29 sasa Exp $*/
/*25.09.2009	30.09.2009	Белых А.И.	iceb_u_ustpeh.c
Анализ максимальной длинны строки для выполнение команд на ужатие текста
     Ориентация листа портрет
10 (нормальный) - 77 символов в строке
10 (ужатый)     - 131 символ в строке
12 (нормальный) - 93 символа в строке
12 (ужатый)     - 155 символов в строке
     Ориентация листа ландшафт
10 (нормальный) - 106 символов в строке
10 (ужатый)     - 181 символ в строке
12 (нормальный) - 128 символа в строке
12 (ужатый)     - 213 символов в строке

77-93-106-128-131-155-181-213
Если вернули 0 ориентация портрет
             1 ориетация лардшафт
*/
#include <stdio.h>

int iceb_u_ustpeh_free(int dlinna_stroki,FILE *ff);
int iceb_u_ustpeh_port(int dlinna_stroki,FILE *ff);
int iceb_u_ustpeh_lan(int dlinna_stroki,FILE *ff);
int iceb_u_ustpeh_port_lan(int dlinna_stroki,FILE *ff);

int iceb_u_ustpeh(int dlinna_stroki,
int metka_orient, /*0-любая 1-только портрет 2-только ландшафт 3 сначала портрет потом ландшафт*/
FILE *ff)
{

if(metka_orient == 0) /*любая ориентация*/
  return(iceb_u_ustpeh_free(dlinna_stroki,ff));

if(metka_orient == 1) /*только портретная ориентация*/
 return(iceb_u_ustpeh_port(dlinna_stroki,ff));

if(metka_orient == 2) /*только ландшафтная ориентация*/
 return(iceb_u_ustpeh_lan(dlinna_stroki,ff));

if(metka_orient == 3) /*сначала портрет потом ландшафт*/
 return(iceb_u_ustpeh_port_lan(dlinna_stroki,ff));

return(0);
}
/**********************************************/
/*Только портретная ориентация*/
/*********************************/
int iceb_u_ustpeh_port(int dlinna_stroki,
FILE *ff)
{
//printf("iceb_u_ustpeh_port dlinna_stroki=%d\n",dlinna_stroki);
int otstup=0;
if(dlinna_stroki <= 77) /*десять знаков на дюйм в нормальном режиме*/
 {
  fprintf(ff,"\x12"); /*Выключить ужатый режим*/
  fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/

  otstup=77-dlinna_stroki;
  if(otstup > 10)
   otstup=10;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }

if(dlinna_stroki > 77 && dlinna_stroki <= 93) /*двенадцать знаков на дюйм в нормальном режиме*/
 {
  fprintf(ff,"\x12"); /*Выключить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=93-dlinna_stroki;
  if(otstup > 10)
   otstup=10;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }

if(dlinna_stroki > 93 && dlinna_stroki <= 131) /*десять знаков на дюйм портрет ужат*/
 {
  fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  otstup=131-dlinna_stroki;
  if(otstup > 20)
   otstup=20;

  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }

if(dlinna_stroki > 131 ) /*двенадцать знаков на дюйм портрет ужат*/
 {
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=155-dlinna_stroki;

  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }

return(0);

}
/**********************************************/
/*Только ланшафтная ориентация*/
/*********************************/
int iceb_u_ustpeh_lan(int dlinna_stroki,
FILE *ff)
{
//printf("iceb_u_ustpeh_lan dlinna_stroki=%d\n",dlinna_stroki);
int otstup=0;
if( dlinna_stroki <= 106) /*десять знаков на дюйм ландшафт*/
 {
  fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
  fprintf(ff,"\x12"); /*Выключить ужатый режим*/
  fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
  otstup=106-dlinna_stroki;

  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(1);   
 }

if(dlinna_stroki > 106 && dlinna_stroki <= 128) /*двенадцать знаков на дюйм ландшафт*/
 {
  fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
  fprintf(ff,"\x12"); /*Выключить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=128-dlinna_stroki;
  if(otstup > 10)
   otstup=10;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(1);   

 }

if(dlinna_stroki > 128 && dlinna_stroki <= 181) /*десять знаков на дюйм ландшаф ужат*/ 
 {
  fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
  otstup=181-dlinna_stroki;
  if(otstup > 20)
   otstup=20;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(1);   

 }

if(dlinna_stroki > 181 ) /*двенадцать знаков на дюйм ландшаф ужат*/ 
 {
  fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=213-dlinna_stroki;
  if(otstup > 20)
   otstup=20;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(1);   
 }

return(1);

}
/**********************************************/
/*Проверяем сначала портрет, потом ландшафт*/
/*********************************/
int iceb_u_ustpeh_port_lan(int dlinna_stroki,FILE *ff)
{
//printf("iceb_u_ustpeh_port_lan dlinna_stroki=%d\n",dlinna_stroki);

int otstup=0;
if(dlinna_stroki <= 77) /*десять знаков на дюйм в нормальном режиме*/
 {
  fprintf(ff,"\x12"); /*Выключить ужатый режим*/
  fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/

  otstup=77-dlinna_stroki;
  if(otstup > 10)
   otstup=10;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }

if(dlinna_stroki > 77 && dlinna_stroki <= 93) /*двенадцать знаков на дюйм в нормальном режиме*/
 {
  fprintf(ff,"\x12"); /*Выключить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=93-dlinna_stroki;
  if(otstup > 10)
   otstup=10;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }

if(dlinna_stroki > 93 && dlinna_stroki <= 131) /*десять знаков на дюйм портрет ужат*/
 {
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
  otstup=131-dlinna_stroki;

  if(otstup > 20)
   otstup=20;

  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }


if(dlinna_stroki > 131 && dlinna_stroki <= 155) /*двенадцать знаков на дюйм портрет ужат*/
 {
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=155-dlinna_stroki;
  if(otstup > 20)
   otstup=20;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }

if(dlinna_stroki > 155 && dlinna_stroki <= 181) /*десять знаков на дюйм ландшаф ужат*/ 
 {
  fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
  otstup=181-dlinna_stroki;
  if(otstup > 20)
   otstup=20;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(1);   

 }

if(dlinna_stroki > 181 && dlinna_stroki <= 213 ) /*двенадцать знаков на дюйм ландшаф ужат*/ 
 {
  fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=213-dlinna_stroki;
  if(otstup > 20)
   otstup=20;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(1);   
 }


if(dlinna_stroki > 213 ) /*двенадцать знаков на дюйм портрет ужат*/
 {
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  return(0);   
 }

return(0);

}
/******************************/
/*Любая ориентация*/
/*******************************/
int iceb_u_ustpeh_free(int dlinna_stroki,FILE *ff)
{
//printf("iceb_u_ustpeh_free dlinna_stroki=%d\n",dlinna_stroki);
int otstup=0;
if(dlinna_stroki <= 77) /*десять знаков на дюйм в нормальном режиме*/
 {
  fprintf(ff,"\x12"); /*Выключить ужатый режим*/
  fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/

  otstup=77-dlinna_stroki;
  if(otstup > 10)
   otstup=10;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }

if(dlinna_stroki > 77 && dlinna_stroki <= 93) /*двенадцать знаков на дюйм в нормальном режиме*/
 {
  fprintf(ff,"\x12"); /*Выключить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=93-dlinna_stroki;
  if(otstup > 10)
   otstup=10;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }

if(dlinna_stroki > 93 && dlinna_stroki <= 106) /*десять знаков на дюйм ландшафт*/
 {
  fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
  fprintf(ff,"\x12"); /*Выключить ужатый режим*/
  fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
  otstup=106-dlinna_stroki;

  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(1);   
 }
if(dlinna_stroki > 106 && dlinna_stroki <= 128) /*двенадцать знаков на дюйм ландшафт*/
 {
  fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
  fprintf(ff,"\x12"); /*Выключить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=128-dlinna_stroki;
  if(otstup > 10)
   otstup=10;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(1);   

 }
if(dlinna_stroki > 128 && dlinna_stroki <= 131) /*десять знаков на дюйм портрет ужат*/
 {
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
  otstup=131-dlinna_stroki;

  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }

if(dlinna_stroki > 131 && dlinna_stroki <= 155) /*двенадцать знаков на дюйм портрет ужат*/
 {
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=155-dlinna_stroki;

  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(0);   
 }

if(dlinna_stroki > 155 && dlinna_stroki <= 181) /*десять знаков на дюйм ландшаф ужат*/ 
 {
  fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
  otstup=181-dlinna_stroki;
  if(otstup > 20)
   otstup=20;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(1);   

 }

if(dlinna_stroki > 181 && dlinna_stroki <= 213) /*двенадцать знаков на дюйм ландшаф ужат*/ 
 {
  fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=213-dlinna_stroki;
  if(otstup > 20)
   otstup=20;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(1);   
 }

if(dlinna_stroki > 213) /*двенадцать знаков на дюйм портрет ужат*/
 {
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  return(0);   
 }

return(0);
}

