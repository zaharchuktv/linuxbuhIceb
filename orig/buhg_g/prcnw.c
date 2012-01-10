/* $Id: prcnw.c,v 1.3 2011-01-13 13:49:52 sasa Exp $ */
/*15.11.1997	30.10.1997	Белых А.И.	prcnw.c
Преобразование цены так чтобы дробная часть выводилась как
надо. Либо с 2мя нолями либо со всеми значащими цифрами после
запятой.
*/
#include	"buhg_g.h"

extern double		okrcn;  /*Округление цены*/

char            *prcnw(double cn)
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
