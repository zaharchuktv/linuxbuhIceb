/*$Id: iceb_u_rsdat1.c,v 1.4 2011-01-13 13:50:05 sasa Exp $*/
/*02.04.2010    11.11.1993      Белых А.И.      iceb_u_rsdat1.c
Подпрограмма расшифровки символьной строки с датой
Если вернули 0 - дата введена правильно
*/
#include	"iceb_util.h"
int iceb_u_rsdat1(short *m,short *g,const char *str)
{
char data[56];
memset(data,'\0',sizeof(data));
strncpy(data,str,sizeof(data)-1);
return(iceb_u_rsdat1(m,g,data));
}

/************************************/

int iceb_u_rsdat1(short *m,short *g,char *str)
{
char            bros[512];
int		i;

for(i=0; str[i] != '\0'; i++)
  if(str[i] == ',')
    str[i]='.';

memset(bros,'\0',sizeof(bros));
iceb_u_polen(str,bros,sizeof(bros),1,'.');
*m=(short)iceb_u_atof(bros);

memset(bros,'\0',sizeof(bros));
iceb_u_polen(str,bros,sizeof(bros),2,'.');
*g=(short)iceb_u_atof(bros);


if(*m > 12 || *m <= 0)
 return(2);

if(*g < 1980 || *g > 3000)
 return(3);

return(0);
}
