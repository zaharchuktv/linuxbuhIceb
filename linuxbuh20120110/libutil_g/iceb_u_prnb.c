/*$Id: iceb_u_prnb.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*09.03.2006    16.08.1993      Белых А.И.      iceb_u_prnb.c
Подпрограмма преобразования числа в символьную строку
со точек
Возвращаемое значение в списке параметров
*/
#include <stdio.h>
#include <string.h>

void    iceb_u_prnb(double nb, //Число для преобразования
char *str) //Символьная строка с числом
{
char            nb1[30],nb2[30];
short           i,i1;
short           mt;

sprintf(nb1,"%.2f",nb);
i1=strlen(nb1);
memset(nb2,' ',sizeof(nb2));

nb2[29]='\0';

for(i=sizeof(nb2)-2; i1>=0; i--)
 {
  if(nb1[i1] == '-')
   {
    nb2[i]=nb1[i1];
    break;
   }
  if(i == 21 || i == 17 || i == 13 || i == 9  || i == 5 || i == 1)
   {
    nb2[i]='.';
    i--;
   }
  if(nb1[i1] == '.')
    nb2[i]='-';
  else
    nb2[i]=nb1[i1];
  i1--;
 }
memset(nb1,'\0',sizeof(nb1));
mt=i1=0;
for(i = 0 ; i <= 29 ; i++)
 {
  if(nb2[i] != ' ' && mt == 0)
    mt++;
  if(mt !=0)
   {
    nb1[i1] = nb2[i];
    i1++;
   }
 }
strcpy(str,nb1);
}

