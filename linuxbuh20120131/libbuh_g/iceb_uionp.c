/*$Id: iceb_uionp.c,v 1.5 2011-02-21 07:36:08 sasa Exp $*/
/*22.05.2008	22.05.2008	Белых А.И.	iceb_uionp.c
Удаление из очереди на печать
*/
#include <stdlib.h>
#include "iceb_libbuh.h"


void iceb_uionp(GtkWidget *wpredok)
{
char nomer_r[20];
memset(nomer_r,'\0',sizeof(nomer_r));
if(iceb_menu_vvod1(gettext("Введите номер очереди на печать"),nomer_r,sizeof(nomer_r),wpredok) != 0)
 return;

if(nomer_r[0] == '\0')
 return;
 
class iceb_u_str kom_ud;
if(iceb_poldan("Снять с очереди на печать",&kom_ud,"nastsys.alx",wpredok) != 0)
 {
  kom_ud.new_plus("lprm");
 }

char komanda[100];

sprintf(komanda,"%s %s",kom_ud.ravno(),nomer_r);

system(komanda);

}
