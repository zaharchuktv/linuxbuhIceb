/*$Id: poikasdokw.c,v 1.9 2011-02-21 07:35:56 sasa Exp $*/
/*20.06.2010	14.10.2005	Белых А.И.	poikasdokw.c
Поиск документа в учёте услуг по номеру документа
*/
#include        <stdlib.h>
#include "buhg_g.h"

void  l_poikasdok(const char *nomdok,GtkWidget *wpredok);


void poikasdokw(GtkWidget *wpredok)
{

iceb_u_str repl;

repl.plus(gettext("Введите номер документа"));
iceb_u_str nomdok;

if(iceb_menu_vvod1(&repl,&nomdok,20,wpredok) != 0)
 return;

char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select kassa,tp,datd,nomd from Kasord where nomd='%s' \
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
  class iceb_u_str kassa(row[0]);
  
  class iceb_u_str datadok(iceb_u_datzap(row[2]));
  
  int tipz=atoi(row[1]);
  class iceb_u_str nomdok(row[3]);
    
  l_kasdok(datadok.ravno(),tipz,nomdok.ravno(),kassa.ravno(),wpredok);


  return;
 }

l_poikasdok(nomdok.ravno(),wpredok);

}
