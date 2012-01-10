/*$Id: iceb_u_ud_simv.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*18.02.2009	05.09.2005	Белых А.И.	iceb_u_ud_simv.c
Удаление из строки всех символов которые указаны во второй строке
*/
#include <string.h>

void iceb_u_ud_simv(char *stroka,const char *simvol)
{
if(stroka[0] == '\0')
 return;
int dlinna=strlen(stroka);
char buff[dlinna+1];
strcpy(buff,stroka);
memset(stroka,'\0',dlinna+1);
int nomer=0;
int metka;
for(int i=0; i < dlinna ; i++)
 {
  metka=0;
  for(int kk=0; kk < (int)strlen(simvol); kk++)
   if(buff[i] == simvol[kk]) 
    {
     metka=1;
     break;
    }
  if(metka == 1)
   continue;
  stroka[nomer++]=buff[i];
 }
}
