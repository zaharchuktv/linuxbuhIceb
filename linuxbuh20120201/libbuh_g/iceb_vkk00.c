/*$Id: iceb_vkk00.c,v 1.1 2011-02-21 07:36:08 sasa Exp $*/
/*17.02.2011	17.02.2011	Белых А.И.	iceb_vkk00.c
Ввод наименования контрагента с кодом 00
*/
#include <stdlib.h>
#include <unistd.h>
#include "iceb_libbuh.h"

extern char *organ;

void iceb_vkk00(GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str naim_kontr("");

if(iceb_menu_vvod1(gettext("Введите наименование вашей организации"),&naim_kontr,40,wpredok) != 0)
 {  
  if((organ=(char *)malloc((strlen(gettext("Не введено код контрагента 00 !"))+1)*sizeof(char))) == NULL)
     iceb_menu_soob("Не могу выделить пямять для organ !",wpredok);
  strcpy(organ,gettext("Не введено код контрагента 00 !"));
 }
else
 {
  sprintf(strsql,"insert into Kontragent (kodkon,naikon,ktoz,vrem) values ('00','%s',%d,%ld)",naim_kontr.ravno(),getuid(),time(NULL));
  iceb_sql_zapis(strsql,1,0,wpredok);
  if((organ=(char *)malloc(naim_kontr.getdlinna()*sizeof(char))) == NULL)
     iceb_menu_soob("Не могу выделить пямять для organ !",wpredok);
  strcpy(organ,naim_kontr.ravno());
 }    
}
