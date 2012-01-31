/*$Id: iceb_cp.c,v 1.5 2011-02-21 07:36:06 sasa Exp $*/
/*24.09.2009	23.09.2009	Белых А.И.	iceb_cp.c
Копирование файла
Возвращаем 0- если файл скопирова
           1- если входной файл не смогли открыть
           2- если выходной файл не смогли открыть
           3- если коприрование с проверкой и такой файл там уже есть
*/

#include <sys/stat.h>
#include <errno.h>
#include "iceb_libbuh.h"


int iceb_cp(const char *imaf_out,const char *imaf_in,
int metka,/*0-всегда копировать 1-только если там его нет 2-тоже что и 1 только без сообщения*/
GtkWidget *wpredok)
{
/*проверяем может файл там уже есть*/
if(metka > 0)
 {
  struct stat buff;
  if(stat(imaf_in,&buff) == 0) /*файл там уже есть*/
   {
    if(metka == 1)
     {
      class iceb_u_str repl(gettext("Такой файл уже есть!"));
      repl.ps_plus(imaf_in);
      iceb_menu_soob(repl.ravno(),wpredok);
     }
    return(3);
   }
 }
FILE *ff_out;
FILE *ff_in;
if((ff_in = fopen(imaf_in,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_in,"iceb_cp",errno,wpredok);
  return(2);
 }

if((ff_out = fopen(imaf_out,"r")) == NULL)
 {
  iceb_er_op_fil(imaf_out,"iceb_cp",errno,wpredok);
  return(1);
 }

int i=0;
while((i=fgetc(ff_out)) != EOF)
  fputc(i, ff_in);  


fclose(ff_in);

fclose(ff_out);
return(0);
}
