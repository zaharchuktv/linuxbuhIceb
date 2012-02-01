/*$Id: iceb_u_ud_lp.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*15.03.2005	15.03.2005	Белых А.И.	iceb_u_ud_lp.c
Удаление в строке лидирующих пробелов
*/
#include <string.h>
void iceb_u_ud_lp(char *stroka)
{
int razmer=strlen(stroka)+1;
char str1[razmer];
memset(str1,'\0',razmer);
int sh=0;
for(int i=0; i < razmer;i++)
 {
  if(sh == 0)  
   if(stroka[i] == ' ')
    continue;
  str1[sh++]=stroka[i];
 }
strcpy(stroka,str1);
}
