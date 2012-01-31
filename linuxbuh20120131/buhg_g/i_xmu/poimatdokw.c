/*$Id: poimatdokw.c,v 1.7 2011-02-21 07:35:56 sasa Exp $*/
/*04.07.2005	11.11.2004	Белых А.И.	poimatdokw.c
Поиск документа в материальном учёте по номеру документа
*/
#include        <stdlib.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

void  l_poimatdok(const char *nomdok,GtkWidget *wpredok);


void poimatdokw(GtkWidget *wpredok)
{

iceb_u_str repl;

repl.plus(gettext("Введите номер документа"));
iceb_u_str nomdok;

if(iceb_menu_vvod1(&repl,&nomdok,20,wpredok) != 0)
 return;

char strsql[320];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select datd,sklad,nomd,tip from Dokummat where nomd='%s' \
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
  iceb_u_str datadok;
  iceb_u_str sklad;
  int tipz=atoi(row[3]);
  short dd,md,gd;
  iceb_u_rsdat(&dd,&md,&gd,row[0],2);
  sprintf(strsql,"%d.%d.%d",dd,md,gd);
  datadok.new_plus(strsql);
  sklad.new_plus(row[1]);
  
  l_dokmat(&datadok,tipz,&sklad,&nomdok,wpredok);
  return;
 }

l_poimatdok(nomdok.ravno(),wpredok);

}
