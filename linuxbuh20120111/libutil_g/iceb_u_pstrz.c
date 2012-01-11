/*$Id: iceb_u_pstrz.c,v 1.4 2011-01-13 13:50:05 sasa Exp $*/
/*11.11.2008	20.03.2004	Белых А.И.	iceb_u_pstrz.c
Прибавить к строке новую строку с запятой если прибавление не первое
*/
#include "iceb_util.h"

int  iceb_u_pstrz(char *stroka,int razmer,const char *stroka_plus)
{
int dlzap=strlen(stroka);
int pdlzap=strlen(stroka_plus);

if(razmer-1 < dlzap+pdlzap+1)
 return(1);

if(dlzap != 0)
 strcat(stroka,",");

strcat(stroka,stroka_plus);

return(0);

}
