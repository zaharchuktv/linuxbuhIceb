/*$Id: iceb_u_vstavfil.c,v 1.6 2011-02-21 07:36:13 sasa Exp $*/
/*08.01.2010	14.01.2002	Белых А.И.	iceb_u_vstavfil.c
Подпрограмма вставки содержимого одного файла в другой
Если вернули 0 порядок
             1 нет
*/
#include        <stdio.h>
#include        <string.h>
#include        <errno.h>


int 	iceb_u_vstavfil(const char *imafil,FILE *ff)
{
FILE	*ffvs;
char	strok[1024];

//printw("\nvstavfil-imaf=%s\n",imafil);
if((ffvs = fopen(imafil,"r")) == NULL)
 {
  return(1);
 }
memset(strok,'\0',sizeof(strok));
while(fgets(strok,sizeof(strok),ffvs) != NULL)
 {
  if(strok[0] == '#')
   continue;
  fprintf(ff,"%s",strok);
 }
fclose(ffvs);
return(0);
}
