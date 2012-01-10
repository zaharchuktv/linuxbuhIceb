/*$Id: iceb_u_sqlfiltr.c,v 1.1 2011-03-28 06:55:51 sasa Exp $*/
/*13.03.2010	13.03.2010	Белых А.И.	iceb_u_sqlfiltr.cxx
Фильт для sql запроса
*/
#include "iceb_util.h"

const char *iceb_u_sqlfiltr(const char *stroka)
{

int dlinna=strlen(stroka);
static char *new_str=NULL;
if(new_str != NULL)
 delete [] new_str;

if((new_str=new char[dlinna*2+1]) == NULL)
 {
  printf("%s-не могу выделить память!\n",__FUNCTION__);
  return(stroka);
 }
int nomer_to=0;
int nom=0;
for(nom=0; nom < dlinna; nom++)
 {
  if(stroka[nom] == '\'')
    new_str[nomer_to++]='\'';
  new_str[nomer_to++]=stroka[nom];

 }
new_str[nom]='\0';


return(new_str);
}
