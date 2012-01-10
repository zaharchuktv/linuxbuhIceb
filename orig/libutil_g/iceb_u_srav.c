/*$Id: iceb_u_srav.c,v 1.8 2011-01-13 13:50:05 sasa Exp $*/
/*08.11.2008  28.04.1987       Белых А.И.   iceb_u_srav.c
Программа сравнения двух символьных строчек
если вернули  0  строчки символов равны
если вернули -1 то не равны
*/
#include        <stdio.h>
#include <string.h>
#include "iceb_util.h"
/*********
int iceb_u_SRAV(const char N1[],const char *N2,int N3) //0-полное сравнение 1-первых символов
{
 char strok[strlen(N2)+1];
 memset(strok,'\0',sizeof(strok));
 strcpy(strok,N2);
 return(iceb_u_SRAV(N1,strok,N3));

}
***********/
/*******************************/

int iceb_u_SRAV(const char *N1,const char *N2,int N3) //0-полное сравнение 1-первых символов
{
int     I,I2;
if(N1 == NULL && N2 == NULL )
 return(0);

if(N1 == NULL || N2 == NULL )
 return(1);
  
//printf("iceb_u_SRAV %s %s %d\n",N1,N2,N3);

if(N1[0] == '\0' && N2[0] == '\0')
  return(0);
I2=(-1);

if(N3 == 1)
 for(I = 0; N1[I] != '\0' &&  N2[I] != '\0'; I++)
   {
    I2=0;
    if( N1[I] != N2[I] )
     {
      I2=(-1);
      break;
     }
/*  printf("#### %d %s\t %s\n",I2,N1,N2);       */
   }
else
 for(I = 0; N1[I] != '\0' ||  N2[I] != '\0'; I++)
   {
        I2=0;
        if( N1[I] != N2[I] )
                {
                   I2=(-1);
                   break;
                }
/*  printf("#### %d %s\t %s\n",I2,N1,N2);       */
   }
/*printf("**** %d %s\t %s\n",I2,N1,N2);
for(I=0;I<10;I++) PRINTF("%o:",N1[I]);  PRINTF("\n");
for(I=0;I<10;I++) PRINTF("%o:",N2[I]);  PRINTF("\n");   */
return(I2);
}
