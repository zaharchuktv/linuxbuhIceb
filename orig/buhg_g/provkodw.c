/*$Id: provkodw.c,v 1.4 2011-01-13 13:49:52 sasa Exp $*/
/*27.04.2007	25.07.2001	Белых А.И.	provkodw.c
Проверка наличия кода в массиве кодов в которых в первой ячейке записан размер массива
Возвращаем номер ячейки в котором находится код
Если не нашли возвращаем -1
*/
#include <stdlib.h>


int provkodw(short *masiv,short kod)
{

if(masiv == NULL)
  return(-1);

for(int i1=1; i1 <= masiv[0] ; i1++)
 if(kod == masiv[i1])
   return(i1-1);

return(-1);

}
int provkodw(short *masiv,char *kod)
{
return(provkodw(masiv,atoi(kod)));
}
