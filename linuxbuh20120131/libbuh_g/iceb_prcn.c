/*$Id: iceb_prcn.c,v 1.3 2011-01-13 13:49:59 sasa Exp $*/
/*08.07.2004	30.10.1997	Белых А.И.	iceb_prcn.c
Преобразование цены так чтобы дробная часть выводилась как
надо. Либо с 2мя нолями либо со всеми значащими цифрами после
запятой.
*/
#include	"iceb_libbuh.h"

extern double	okrcn;  /*Округление цены*/

char            *iceb_prcn(double cn) //Число для преобразования
{
static char     str[1024]; /*Символьная строка с числом*/
char		br[10];

/*printw("\ncn=%.10f okr=%.10f\n",cn,okr);*/
sprintf(str,"%.10g",cn);
iceb_u_pole(str,br,2,'.');
if(strlen(br) < 2)
  sprintf(str,"%.2f",cn);
 
return(str);
}
