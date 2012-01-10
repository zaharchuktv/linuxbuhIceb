/*$Id: iceb_u_poldan.c,v 1.10 2011-02-21 07:36:13 sasa Exp $*/
/*11.11.2008    12.03.1993      Белых А.И.      iceb_u_poldan.c
Подпрограмма получения нужных данных из файла настройки
Если вернули 0 образец найден
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <errno.h>
#include        "iceb_util.h"

int iceb_u_poldan(const char *obr, //Образец поиска
class iceb_u_str *str, //Найденная строка
const char *imaf) //Имя файла с настройкой
{
 char stroka[10240];
 int voz;
 memset(stroka,'\0',sizeof(stroka));
 
 if((voz=iceb_u_poldan(obr,stroka,imaf)) == 0)
  str->new_plus(stroka);
 return(voz);
 
}
/****************************/
/***************************/

int iceb_u_poldan(const char *obr, //Образец поиска
char *str, //Найденная строка
const char *imaf) //Имя файла с настройкой
{
char            st[512];
char            st1[512];
FILE            *f;
short           voz;
class iceb_u_str stroka1("");
class iceb_u_str stroka2("");

voz=1;
errno=0;
if((f = fopen(imaf,"r")) == NULL)
   return(errno);

while( fgets(st,sizeof(st),f) != NULL)
 {
  iceb_u_polen(st,st1,sizeof(st1),1,'|');
  /*printf("%s %s\n",obr,st1);*/
  if(iceb_u_SRAV(st1,obr,0) == 0)
    {
//     iceb_u_pole(st,str,2,'|');
     iceb_u_polen(st,&stroka1,2,'|');
     iceb_u_fplus(stroka1.ravno(),&stroka2,f);
     strcpy(str,stroka2.ravno());     
     voz=0;
     break;
    }
 }

fclose(f);
return(voz);
}


