/*$Id: iceb_podpis.c,v 1.13 2011-02-21 07:36:07 sasa Exp $*/
/*19.09.2010	09.10.2000	Белых А.И.	iceb_podpis.c
Подпись в отчетах
*/
#include        <stdio.h>
#include        <pwd.h>
#include        <unistd.h>
#include        "iceb_libbuh.h"

extern char     *name_system;
extern char     *version;

void iceb_podpis(uid_t kod_operatora,FILE *ff,GtkWidget *wpredok)
{


fprintf(ff,"\n\n%s \"%s\" %s %s",
gettext("Система"),
name_system,
gettext("Версия"),version);

char bros[512];
memset(bros,'\0',sizeof(bros));
if(iceb_poldan("Печатать фамилию оператора в распечатках отчетов",bros,"nastsys.alx",wpredok) == 0)
 if(iceb_u_SRAV(bros,"Вкл",1) != 0)
  return;  

 struct  passwd  *ktor; /*Кто работает*/

 if((ktor=getpwuid(kod_operatora)) != NULL)
  {
   fprintf(ff," %s:%d %.*s",
   gettext("Распечатал"),
   kod_operatora,
   iceb_u_kolbait(40,ktor->pw_gecos),
   ktor->pw_gecos);
  }
 else
  {
   fprintf(ff," %s:%d %s",
   gettext("Распечатал"),
   kod_operatora,
   gettext("Неизвестный логин"));
  }

fprintf(ff,"\n");

} 
/**************************************/
/**************************************/

void iceb_podpis(FILE *ff,GtkWidget *wpredok)
{
fprintf(ff,"\n\n%s \"%s\" %s %s",
gettext("Система"),
name_system,
gettext("Версия"),version);

char bros[512];
memset(bros,'\0',sizeof(bros));
if(iceb_poldan("Печатать фамилию оператора в распечатках отчетов",bros,"nastsys.alx",wpredok) == 0)
 if(iceb_u_SRAV(bros,"Вкл",1) != 0)
  return;

int nomer_op=iceb_getuid(wpredok);
class iceb_u_str fioop(iceb_getfioop(wpredok));
fprintf(ff," %s:%d %.*s",
gettext("Распечатал"),
nomer_op,
iceb_u_kolbait(40,fioop.ravno()),
fioop.ravno());

}
