/*$Id: iceb_u_rsdat.c,v 1.13 2011-02-21 07:36:13 sasa Exp $*/
/*11.11.2008    11.11.1993      Белых А.И.      iceb_u_rsdat.c
Подпрограмма расшифровки символьной строки с датой
Если вернули 0 - дата введена правильно
             1 - день не правильно
             2 - месяц не правильно
             3 - год не правильно
             (отключено)4 - введена будущая дата больше чем год вперед
             5 - в строке для расшифровки ничего нет
*/
#include        <stdio.h>
#include        <string.h>
#include        <time.h>
#include	"iceb_util.h"


int iceb_u_rsdat(short *d,short *m,short *g,const char *str,int mr)
 {
  char data[56];
  memset(data,'\0',sizeof(data));
  strncpy(data,str,sizeof(data)-1);
  return(iceb_u_rsdat(d,m,g,data,mr));
 }

/*******************************************/

int             iceb_u_rsdat(short *d,short *m,short *g,char *str,int mr)
/*
    		mr =  0-автоматическое определение даты
                      1 дд.мм.гггг
                      2 гггг-мм-дд
                      3 дд месяц гггг
                      */
{
char            bros[512];
struct  tm      *bf;
time_t          tmm;
//short           god,mes;
int		i;

*d=0;
*m=0;
*g=0;

if(str == NULL)
 return(5);

if(str[0] == '\0')
 return(5);
 
time(&tmm);
bf=localtime(&tmm);
memset(bros,'\0',sizeof(bros));
if(mr == 0 || mr == 1)
 {
  for(i=0; str[i] != '\0'; i++)
   {
    if(str[i] == ',')
      str[i]='.';
   }      

  if(iceb_u_polen(str,bros,sizeof(bros),1,'.') == 0)
   {
    *d=(short)iceb_u_atof(bros);

    memset(bros,'\0',sizeof(bros));
    iceb_u_polen(str,bros,sizeof(bros),2,'.');
    *m=(short)iceb_u_atof(bros);

    memset(bros,'\0',sizeof(bros));
    iceb_u_polen(str,bros,sizeof(bros),3,'.');
    *g=(short)iceb_u_atof(bros);

   }
//  printf("rsdat_g-%d.%d.%d\n",*d,*m,*g);
 }
if(mr == 0 || mr == 2)
if(iceb_u_polen(str,bros,sizeof(bros),1,'-') == 0)
 {
  *g=(short)iceb_u_atof(bros);

  memset(bros,'\0',sizeof(bros));
  iceb_u_polen(str,bros,sizeof(bros),2,'-');
  *m=(short)iceb_u_atof(bros);

  memset(bros,'\0',sizeof(bros));
  iceb_u_polen(str,bros,sizeof(bros),3,'-');
  *d=(short)iceb_u_atof(bros);

 }

if(mr == 0 || mr == 3)
if(iceb_u_polen(str,bros,sizeof(bros),1,' ') == 0 || iceb_u_polen(str,bros,sizeof(bros),1,'-') == 0)
 {
  *d=(short)iceb_u_atof(bros);

  if(iceb_u_strstrm(str,gettext("янв")) != 0)
   *m=1;

  if(iceb_u_strstrm(str,gettext("фев")) != 0)
   *m=2;

  if(iceb_u_strstrm(str,gettext("мар")) != 0)
   *m=3;

  if(iceb_u_strstrm(str,gettext("апр")) != 0)
   *m=4;

  if(iceb_u_strstrm(str,gettext("мая")) != 0)
   *m=5;

  if(iceb_u_strstrm(str,gettext("июн")) != 0)
   *m=6;

  if(iceb_u_strstrm(str,gettext("июл")) != 0)
   *m=7;

  if(iceb_u_strstrm(str,gettext("авг")) != 0)
   *m=8;

  if(iceb_u_strstrm(str,gettext("сен")) != 0)
   *m=9;

  if(iceb_u_strstrm(str,gettext("окт")) != 0)
   *m=10;

  if(iceb_u_strstrm(str,gettext("ноя")) != 0)
   *m=11;

  if(iceb_u_strstrm(str,gettext("дек")) != 0)
   *m=12;
  if(*m == 0)
   {
    if(iceb_u_strstrm(str,"янв") != 0)
     *m=1;

    if(iceb_u_strstrm(str,"фев") != 0)
     *m=2;

    if(iceb_u_strstrm(str,"мар") != 0)
     *m=3;

    if(iceb_u_strstrm(str,"апр") != 0)
     *m=4;

    if(iceb_u_strstrm(str,"мая") != 0 || iceb_u_strstrm(str,"май") != 0)
     *m=5;

    if(iceb_u_strstrm(str,"июн") != 0)
     *m=6;

    if(iceb_u_strstrm(str,"июл") != 0)
     *m=7;

    if(iceb_u_strstrm(str,"авг") != 0)
     *m=8;

    if(iceb_u_strstrm(str,"сен") != 0)
     *m=9;

    if(iceb_u_strstrm(str,"окт") != 0)
     *m=10;

    if(iceb_u_strstrm(str,"ноя") != 0)
     *m=11;

    if(iceb_u_strstrm(str,"дек") != 0)
     *m=12;
   }   
  memset(bros,'\0',sizeof(bros));
  if(iceb_u_polen(str,bros,sizeof(bros),3,' ') == 0  || iceb_u_polen(str,bros,sizeof(bros),3,'-') == 0 )
    *g=(short)iceb_u_atof(bros);
 }

if(*d > 31 || *d < 1)
 return(1);

if((*m == 4 || *m == 6 || *m == 9 || *m == 11) && *d > 30)
 return(1);

if(*m == 2)
 {
  int metka_vis_god=0;

  if(*g % 4 == 0 && *g % 100 != 0)
   metka_vis_god=1;

  if(*g % 400 == 0)
   metka_vis_god=1;

  if(metka_vis_god == 1)
   {
    if(*d > 29) /*Год високосный*/
      return(1);
   } 
  else
    if(*d > 28) /*Год не високосный*/
      return(1);
 }   

if(*m > 12 || *m < 1)
 return(2);

if(*g < 1900 || *g > bf->tm_year+1900+1000)
 return(3);
//god=bf->tm_year+1900+1;
//mes=bf->tm_mon+1;
//if(iceb_u_sravmydat(1,mes,god,*d,*m,*g) < 0)
// return(4);

return(0);
}
