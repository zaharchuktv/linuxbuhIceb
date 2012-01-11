/* $Id: iceb_u_sozmas.c,v 1.6 2011-01-13 13:50:05 sasa Exp $ */
/*24.11.2008	07.12.1999	Белых А.И.	iceb_u_sozmas.c
Выделить память и зарядить данные
В нулевом елементе количество кодов
*/
#include <stdlib.h>
#include        "iceb_util.h"

void iceb_u_sozmas(short **kod,char *st1,FILE *ff)
{
short		i,i1;
class iceb_u_str  str("");
char            bros[1024];

if(iceb_u_polen(st1,&str,2,'|') != 0)
  str.new_plus(st1);
class iceb_u_str spisok_kodov("");

iceb_u_fplus(str.ravno(),&spisok_kodov,ff);

if(spisok_kodov.getdlinna() <= 1)
 return;  

i=iceb_u_pole2(spisok_kodov.ravno(),',');
if(i == 0 && spisok_kodov.getdlinna() > 1)
  i=1;

if((*kod=new short[i+1]) == NULL)
 {
  printf("%s-Не могу выделить память для kod !!!\n",__FUNCTION__);
  return;
 }

(*kod)[0]=i;

for(i1=1; i1 < *kod[0]+1; i1++)
 {
  memset(bros,'\0',sizeof(bros));
  if(i == 1)
   strncpy(bros,spisok_kodov.ravno(),sizeof(bros)-1);
  else        
    iceb_u_polen(spisok_kodov.ravno(),bros,sizeof(bros),i1,',');
  (*kod)[i1]=(short)atoi(bros);
 }

}
/*****************************/
void iceb_u_sozmas(short **kod,char *st1)
{
short		i,i1;
char            str[1024];
char            bros[1024];

memset(str,'\0',sizeof(str));
if(iceb_u_polen(st1,str,sizeof(str),2,'|') != 0)
  strncpy(str,st1,sizeof(str)-1);

if(str[0] == '\0')
 return;  

i=iceb_u_pole2(str,',');
if(i == 0 && str[0] != '\0')
  i=1;

if((*kod=new short[i+1]) == NULL)
 {
  printf("%s-Не могу выделить память для kod !!!\n",__FUNCTION__);
  return;
 }

(*kod)[0]=i;

for(i1=1; i1 < *kod[0]+1; i1++)
 {
  memset(bros,'\0',sizeof(bros));
  if(i == 1)
   strncpy(bros,str,sizeof(bros)-1);
  else        
    iceb_u_polen(str,bros,sizeof(bros),i1,',');
  (*kod)[i1]=(short)atoi(bros);
 }

}
