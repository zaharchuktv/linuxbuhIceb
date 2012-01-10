/*$Id: poizardokw.c,v 1.8 2011-02-21 07:35:56 sasa Exp $*/
/*27.07.2008	27.07.2008	Белых А.И.	poizardokw.c
Поиск документа в подсистеме заработной платы по номеру документа
*/
#include        <stdlib.h>
#include "buhg_g.h"

void  l_poizardok(const char *nomdok,GtkWidget *wpredok);


void poizardokw(GtkWidget *wpredok)
{

iceb_u_str repl;

repl.plus(gettext("Введите номер документа"));
iceb_u_str nomdok;

if(iceb_menu_vvod1(&repl,&nomdok,20,wpredok) != 0)
 return;

char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select datd,nomd,td from Zardok where nomd='%s' order by datd desc",nomdok.ravno());

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
  
  char datadok[11];
  strncpy(datadok,iceb_u_datzap(row[0]),sizeof(datadok)-1);
  
  short prn=atoi(row[2]);
  char nomdok[32];
  strncpy(nomdok,row[1],sizeof(nomdok)-1);
  l_zar_dok_zap(prn,datadok,nomdok,wpredok);
    


  return;
 }

l_poizardok(nomdok.ravno(),wpredok);

}
