/*$Id: iceb_u_ps_minus.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*22.09.2003	22.09.2003	Белых А.И.	iceb_u_ps_minus.c
Удаление символа перевода строки в конце строки
*/
#include  <string.h>

void     iceb_u_ps_minus(char *stroka)
{
int i=strlen(stroka);
if(stroka[i-1] == '\n')
 stroka[i-1]='\0';

}
