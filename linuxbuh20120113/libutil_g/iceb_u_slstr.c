/*$Id: iceb_u_slstr.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*02.04.2010    27.12.1993      Белых А.И.      iceb_u_slstr.c
Складывание строки
*/
#include "iceb_util.h"

void iceb_u_slstr(char *st,/*Исходная строка*/
short dl, /*Длинна участка*/
char *str) /*Массив со сложенной строкой*/
{
short           i,i1;
char            bros[512];
char            bros1[512];

memset(bros,'\0',sizeof(bros));
memset(bros1,'\0',sizeof(bros1));
i1=0;
if(iceb_u_polen(st,bros,sizeof(bros),1,' ') == 0)
 {
  for(i=1; iceb_u_polen(st,bros,sizeof(bros),i,' ') == 0 ;i++)
   {
    if((int)strlen(bros) >= dl)
     {
      if(i1 != 0)
       {
	sprintf(&str[i1],"%s",bros1);
	memset(bros1,'\0',sizeof(bros1));
	i1+=dl;
       }
      strncat(bros1,bros,dl-1);
      sprintf(&str[i1],"%s",bros1);
      i1+=dl;
      memset(bros1,'\0',sizeof(bros1));
      continue;
     }

    if((int)strlen(bros1)+(int)strlen(bros)+1 < dl)
     {
      strcat(bros1,bros);
      strcat(bros1," ");
     }
    else
     {
/*    printf("bros1-%s\n",bros1);*/
      sprintf(&str[i1],"%s",bros1);
      memset(bros1,'\0',sizeof(bros1));
      strcat(bros1,bros);
      strcat(bros1," ");
      i1+=dl;
     }
   }
  sprintf(&str[i1],"%s",bros1);
 }
else
  sprintf(str,"%s",st);

}
