/*$Id: xdkudgod.c,v 1.5 2011-02-21 07:35:59 sasa Exp $*/
/*18.05.2006	18.05.2006	Белых А.И.	xdkudgod.c
Удаление данных за год
*/
#include <stdlib.h>
#include "buhg_g.h"

int xdkudgod_r(short god,GtkWidget *wpredok);


void xdkudgod()
{
iceb_u_str repl;
repl.plus(gettext("Удаление документов за год."));
repl.ps_plus(gettext("Введите год"));
char god_h[6];
memset(god_h,'\0',sizeof(god_h));

if(iceb_menu_vvod1(&repl,god_h,5,NULL) != 0)
 return;

short god=atoi(god_h);

char strsql[512];
sprintf(strsql,gettext("Удалить документы за %d год ? Вы уверены ?"),god);

if(iceb_menu_danet(strsql,2,NULL) == 2)
  return;
if(iceb_parol(0,NULL) != 0)
 return;

xdkudgod_r(god,NULL);


}
