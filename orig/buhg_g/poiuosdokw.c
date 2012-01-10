/*$Id: poiuosdokw.c,v 1.7 2011-02-21 07:35:56 sasa Exp $*/
/*15.01.2008	15.01.2008	Белых А.И.	poiuosdokw.c
Поиск документа в учёте основных средств по номеру документа
*/
#include        <stdlib.h>
#include "buhg_g.h"

void  l_poiuosdok(const char *nomdok,GtkWidget *wpredok);


void poiuosdokw(GtkWidget *wpredok)
{

iceb_u_str repl;

repl.plus(gettext("Введите номер документа"));
iceb_u_str nomdok;

if(iceb_menu_vvod1(&repl,&nomdok,20,wpredok) != 0)
 return;

char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select datd from Uosdok where nomd='%s' \
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
  l_uosdok(strsql,nomdok.ravno(),wpredok);

  return;
 }

l_poiuosdok(nomdok.ravno(),wpredok);

}
