/*$Id: poiupldokw.c,v 1.7 2011-02-21 07:35:56 sasa Exp $*/
/*27.02.2008	27.02.2008	Белых А.И.	poiupldokw.c
Поиск документа в учёте путевых листов по номеру документа
*/
#include        <stdlib.h>
#include "buhg_g.h"

void  l_poiupldok(const char *nomdok,GtkWidget *wpredok);


void poiupldokw(GtkWidget *wpredok)
{

iceb_u_str repl;

repl.plus(gettext("Введите номер документа"));
class iceb_u_str nomdok;

if(iceb_menu_vvod1(&repl,&nomdok,20,wpredok) != 0)
 return;

char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select datd,kp from Upldok where nomd='%s' \
order by datd desc",nomdok.ravno());

int koldok=iceb_sql_readkey(strsql,&row,&cur,wpredok);

if(koldok == 0)
 {
  repl.new_plus(gettext("Не найдено ни одного документа с номером"));
  repl.plus(" ");
  repl.plus(nomdok.ravno());
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);
  return;
 }

if(koldok == 1)
 {
  short dd,md,gd;
  iceb_u_rsdat(&dd,&md,&gd,row[0],2);
  sprintf(strsql,"%d.%d.%d",dd,md,gd);

  l_upldok(strsql,nomdok.ravno(),row[1],wpredok);


  return;
 }

l_poiupldok(nomdok.ravno(),wpredok);

}
