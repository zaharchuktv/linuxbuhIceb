/*$Id: iceb_ustpeh.c,v 1.8 2011-03-09 08:21:27 sasa Exp $*/
/*08.08.2010	05.09.2009	Белых А.И.	iceb_ustpeh.c
Определение максимально длинной строки в файле
и установка команд для печати файла с нужным ужатием и ориентацией
*/
#include <errno.h>
#include "iceb_libbuh.h"

void iceb_ustpeh(const char *imaf,int orient,GtkWidget *wpredok) /*0-любая 1-только портрет 2-только ландшафт 3-сначала попытаться портрет и если не помещается ландшафт*/
{
char stroka[10240];
class iceb_u_str imaf_tmp(imaf);
imaf_tmp.plus(".tmp");

FILE *ff;
FILE *ff_tmp;

if((ff=fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,__FUNCTION__,errno,wpredok);
  return;
 }
int max_dlina_str=0;
int dlina_str=0;
int kor=0;
memset(stroka,'\0',sizeof(stroka));
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  kor=0;
  if(stroka[0] == '\f')
   kor=1;
  dlina_str=iceb_u_strlen(stroka)-1-kor;
  if(max_dlina_str < dlina_str)
    max_dlina_str=dlina_str;
 }

if((ff_tmp=fopen(imaf_tmp.ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tmp.ravno(),__FUNCTION__,errno,wpredok);
  return;
 }
iceb_u_startfil(ff_tmp);
iceb_u_ustpeh(max_dlina_str,orient,ff_tmp);

rewind(ff);
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 fprintf(ff_tmp,"%s",stroka);

/*******************
На матричных принтерах если не отменить предыдущие установки
то они остаются и будут применяться для следующей распечатки
*******************/

//fprintf(ff_tmp,"\x1b\x6C%c",1); /*Установка левого поля*/
//fprintf(ff_tmp,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/

fclose(ff);
fclose(ff_tmp);

rename(imaf_tmp.ravno(),imaf);

}

