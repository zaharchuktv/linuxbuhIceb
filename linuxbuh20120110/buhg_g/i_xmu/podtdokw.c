/* $Id: podtdokw.c,v 1.11 2011-02-21 07:35:55 sasa Exp $ */
/*11.11.2008     30.5.1995       Белых А.И.      podtdokw.c
Подтверждение всех записей в документе сразу
*/
#include        <math.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze


void podtdokw(short dd,short md,short gd,int skl,const char *nomdok,
int tipz,int skl1,const char *nomdokp,GtkWidget *wpredok)
{
short           d1,m1,g1;
char		strsql[512];

iceb_u_str repl;

repl.plus(gettext("Подтвердить весь документ ? Вы уверены ?"));


if(iceb_menu_danet(&repl,2,wpredok) == 2)
 return;


sprintf(strsql,"%d.%d.%d",dd,md,gd);

repl.new_plus(gettext("Введите дату подтверждения"));

if(iceb_menu_vvod1(&repl,strsql,11,wpredok) != 0)
   return;


if(iceb_u_rsdat(&d1,&m1,&g1,strsql,0) != 0)
 {
  repl.new_plus(gettext("Не верно введена дата !"));
  iceb_menu_soob(&repl,wpredok);
  return;
 }

if(iceb_u_sravmydat(d1,m1,g1,dd,md,gd) < 0)
 {
  repl.new_plus(gettext("Дата менше даты выписки документа ! Подтверждение невозможно !"));
  iceb_menu_soob(&repl,wpredok);
  return;
 }


if(iceb_pbpds(m1,g1,wpredok) != 0)
  return;

class iceb_gdite_data gdite;
iceb_gdite(&gdite,1,wpredok);

podtdok1w(dd,md,gd,skl,nomdok,tipz,d1,m1,g1,0,wpredok);

gdite.close();

if(skl1 != 0 && nomdokp[0] != '\0')
 {

  repl.new_plus(gettext("Подтвердить парный документ ?"));

  if(iceb_menu_danet(&repl,2,wpredok) == 2)
    return;

  iceb_gdite(&gdite,1,wpredok);

  if(tipz == 1)
      podtdok1w(dd,md,gd,skl1,nomdokp,2,d1,m1,g1,0,wpredok);

  if(tipz == 2)
      podtdok1w(dd,md,gd,skl1,nomdokp,1,d1,m1,g1,0,wpredok);

  podvdokw(dd,md,gd,nomdokp,skl1,wpredok);

 }

}

