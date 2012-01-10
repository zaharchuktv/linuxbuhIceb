/*$Id: iceb_u_cpbstr.c,v 1.2 2011-01-13 13:50:05 sasa Exp $*/
/*04.06.2009	04.06.2009	Белых А.И.	iceb_u_cpbstr.c
Копировать одну строку в другую без заданной цепочки символов 
*/
#include "iceb_util.h"

void iceb_u_cpbstr(char *strin,int dlstrin,const char *strout,const char *udstr)
{
memset(strin,'\0',dlstrin);
int dlud=strlen(udstr);
int shstrout=0;
int dlstrout=strlen(strout);
for(int ii=0; ii < dlstrin && shstrout < dlstrout; ii++)
 {
  if(iceb_u_SRAV(&strout[shstrout],udstr,0) == 0)
   {
    shstrout+=dlud;
   }
  strin[ii]=strout[shstrout++];
  
 } 


}
/*******************/

void iceb_u_cpbstr(class iceb_u_str *strin,const char *strout,const char *udstr)
{
int dlstrin=strlen(strout)+1;
char strinb[dlstrin];

iceb_u_cpbstr(strinb,dlstrin,strout,udstr);
strin->new_plus(strinb);
}

void iceb_u_cpbstr(class iceb_u_spisok *strin,const char *strout,const char *udstr)
{
int dlstrin=strlen(strout)+1;
char strinb[dlstrin];

iceb_u_cpbstr(strinb,dlstrin,strout,udstr);
strin->plus(strinb);
}

