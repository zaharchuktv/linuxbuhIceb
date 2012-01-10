/*$Id: iceb_u_sravmydat.c,v 1.6 2011-01-13 13:50:05 sasa Exp $*/
/*13.11.2008	06.05.2001	Белых А.И.	iceb_u_sravmydat.c
Сравнение двух дат
Если даты равны возвращаем      =  0
Если первая дата меньше         <  -1
Если первая дата больше         >  +1
*/
#include	<stdio.h>
#include	<stdlib.h>
#include "iceb_util.h"

int iceb_u_sravmydat(short d1,short m1,short g1,short d2,short m2,short g2)
{

if(g1 == g2 && m1 == m2 && d1 == d2)
  return(0);
  
if(g1 > g2)
  return(1);

if(g1 < g2)
  return(-1);

//Годы равны
if(m1 > m2)
 return(2);
if(m1 < m2)
 return(-2);

//Месяцы равны

if(d1 > d2)
 return(3);
if(d1 < d2)
 return(-3);

return(0);

}

int iceb_u_sravmydat(const char *data1,const char *data2)
{
char data11[50];
memset(data11,'\0',sizeof(data11));
char data22[50];
memset(data22,'\0',sizeof(data22));
short d1,m1,g1,d2,m2,g2;

strncpy(data11,data1,sizeof(data11)-1);
strncpy(data22,data2,sizeof(data22)-1);

iceb_u_rsdat(&d1,&m1,&g1,data11,1);
iceb_u_rsdat(&d2,&m2,&g2,data22,1);

return(iceb_u_sravmydat(d1,m1,g1,d2,m2,g2));

}
