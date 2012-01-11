/*$Id: iceb_kodkon.c,v 1.3 2011-01-13 13:49:59 sasa Exp $*/
/*24.06.2004	24.06.2004	Белых А.И.	iceb_kodkon.c
извлечение кода контрагента из заданной стороки
*/
#include <string.h>
void  iceb_kodkon(char *kodkon,char *str)
{

if(str[0] == '0' && str[1] == '0' && str[2] == '-')
 strcpy(kodkon,"00");
else
 strcpy(kodkon,str);
 
}
