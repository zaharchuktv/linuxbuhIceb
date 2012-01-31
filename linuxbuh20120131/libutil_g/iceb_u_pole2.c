/*$Id: iceb_u_pole2.c,v 1.4 2011-01-13 13:50:05 sasa Exp $*/
/*11.11.2008    27.11.1996      Белых А.И.      iceb_u_pole2.c
Получение количества полей в строке
*/

int iceb_u_pole2(const char *str,char raz)
{
int             i;
int             kp;

kp=0;
for(i=0; str[i] != '\0' ; i++)
 if(str[i] == raz)
  kp++;

if(kp != 0)
 kp++;
return(kp);
}
