/*$Id: iceb_u_period.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*18.02.2009    22.06.1994      Белых А.И.      iceb_u_period.c
Определение количества дней между двумя датами или месяцев
Возвращает количество дней или месяцев
*/
#include	"iceb_util.h"

int          iceb_u_period(short dn,short mn,short gn, //Начальная дата (меньшая)
short dk,short mk,short gk, //Конечная дата (большая)
int met) //0-количество дней 1-количество месяцев
{
short           d,m,g;
int             kold;

d=dn;
m=mn;
kold = 0;

/*Определяем количество дней в периоде*/
if(met == 0)
for(g=gn; g<=gk ; g++)
 for( ;     ; m++)
  {
   if(g == gk && m > mk)
     break;
   if(m == 13)
    {
     m=1;
     break;
    }
   for(;   ; d++)
    {
     if(g == gk && m == mk  && d > dk)
	break;


     if(d == 32 && (m == 1 || m == 3 || m == 5 || m == 7 || m == 8
	|| m == 10 || m == 12))
      {
       d=1;
       break;
      }

     if(d == 31 && (m == 4 || m == 6 || m == 9 || m == 11))
      {
       d=1;
       break;
      }
     if(m == 2)
      {
       int metka_vis_god=0;
       if(g % 4 == 0 && g % 100 != 0)
        metka_vis_god=1;
       if(g % 400 == 0)
        metka_vis_god=1;

       if(metka_vis_god == 1)
        {
         if(d > 29)   /*Год високосный*/
          {
           d=1;
           break;
          }
        }
       else
        {
         if(d > 28)   /*Год не високосный*/
          {
           d=1;
           break;
          }
        }
      }
     kold++;
    }
   }

/*Определяем количество месяцев в периоде*/
if(met == 1)
for(g=gn; g<=gk ; g++)
 for( ;     ; m++)
  {
   if(g == gk && m > mk)
     break;
   if(m == 13)
    {
     m=1;
     break;
    }
   kold++;
  }
return(kold);
}
