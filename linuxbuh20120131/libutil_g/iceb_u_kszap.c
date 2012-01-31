/*$Id: iceb_u_kszap.c,v 1.17 2011-02-21 07:36:13 sasa Exp $*/
/*23.01.2011	19.09.2003	Белых А.И.	iceb_u_kszap.c
Расшифровка логина и формирование символьной строки с логином и фамилией оператора
*/
#include <glib.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <pwd.h>
#include        <nl_types.h>
#include        "iceb_util.h"


const char *iceb_u_kszap(char *kto,
int metka_kod)  //0-с перекодировкой в UTF8 1-без
{
if(kto != NULL)
 return(iceb_u_kszap(atoi(kto),metka_kod));
else
 return("NULL");
}

/**************************************/
/**************************************/

const char *iceb_u_kszap(int kto,
int metka_kod)  //0-с перекодировкой в UTF8 1-без
{
static char stroka[512];
memset(stroka,'\0',sizeof(stroka));
struct  passwd  *ktoz; /*Кто записал*/

if((ktoz=getpwuid(kto)) != NULL)
 {
  if(metka_kod == 0)
     {
      if(g_utf8_validate(ktoz->pw_gecos,-1,NULL) == TRUE) /*Проверка на utf8 кодировку*/
        sprintf(stroka,"%d %.*s",kto,iceb_u_kolbait(20,ktoz->pw_gecos),ktoz->pw_gecos);
      else
       sprintf(stroka,"%d %.*s",kto,iceb_u_kolbait(20,iceb_u_toutf(ktoz->pw_gecos)),iceb_u_toutf(ktoz->pw_gecos));
     }
  else
   sprintf(stroka,"%d %.*s",kto,iceb_u_kolbait(20,ktoz->pw_gecos),ktoz->pw_gecos);
  
 }
else
 {
  if(metka_kod == 0)
   sprintf(stroka,"%d %s",kto,gettext("Неизвестный логин"));
  else
   sprintf(stroka,"%d %s",kto,gettext("Неизвестный логин"));
 }
return(stroka);

}


