/*$Id: poixuddokw.c,v 1.5 2011-02-21 07:35:56 sasa Exp $*/
/*21.04.2009	22.04.2009	Белых А.И.	poixuddokw.c
Поиск документа в учёте путевых листов по номеру документа
*/
#include        <stdlib.h>
#include "buhg_g.h"

void  l_poixuddok(const char *nomdok,GtkWidget *wpredok);


void poixuddokw(GtkWidget *wpredok)
{

iceb_u_str repl;

repl.plus(gettext("Введите номер документа"));
class iceb_u_str nomdok;

if(iceb_menu_vvod1(&repl,&nomdok,20,wpredok) != 0)
 return;

char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select datd from Uddok where nomd='%s' order by datd desc",nomdok.ravno());

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
  l_dover(iceb_u_datzap(row[0]),nomdok.ravno(),wpredok);
  return;
 }

l_poixuddok(nomdok.ravno(),wpredok);

}
