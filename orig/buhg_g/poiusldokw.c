/*$Id: poiusldokw.c,v 1.6 2011-02-21 07:35:56 sasa Exp $*/
/*14.10.2005	14.10.2005	Белых А.И.	poiusldokw.c
Поиск документа в учёте услуг по номеру документа
*/
#include        <stdlib.h>
#include "buhg_g.h"

void  l_poiusldok(const char *nomdok,GtkWidget *wpredok);


void poiusldokw(GtkWidget *wpredok)
{

iceb_u_str repl;

repl.plus(gettext("Введите номер документа"));
iceb_u_str nomdok;

if(iceb_menu_vvod1(&repl,&nomdok,20,wpredok) != 0)
 return;

char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select datd,podr,nomd,tp from Usldokum where nomd='%s' \
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
  int podr=atoi(row[1]);
  int tipz=atoi(row[3]);
  short dd,md,gd;
  iceb_u_rsdat(&dd,&md,&gd,row[0],2);
  
  l_usldok(dd,md,gd,tipz,podr,nomdok.ravno(),wpredok);


  return;
 }

l_poiusldok(nomdok.ravno(),wpredok);

}
