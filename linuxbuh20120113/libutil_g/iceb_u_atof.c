/*$Id: iceb_u_atof.c,v 1.6 2011-01-13 13:50:05 sasa Exp $*/
/*13.02.2010  22.04.1987          Белых А.И.    iceb_u_atof.c
Функция преобразования символьной строки в число
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>

double iceb_u_atof(const char *S)
{
int		l,I;
char		*S1;
double		sum;

l = strlen(S);

S1=new char[l+1];

strcpy(S1,S);

for (I=0; I<l; I++) 
 {
  switch (S1[I]) 
    {
#if 0
###########################
//    case 'о':  /*Русские*/
    case 'О':
//    case 'o': /*Английские*/
    case 'O':
      S1[I]='0';
      break;
//    case 'з':  /*Русские*/
    case 'З':
      S1[I]='3';
      break;
#######################3
#endif
    case ',':
      S1[I]='.';
      break;
    }
  
               
 }

sum=(atof(S1));
delete [] S1;
return(sum);
}
