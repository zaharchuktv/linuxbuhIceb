/* $Id: matudw.c,v 1.5 2011-01-13 13:49:51 sasa Exp $ */
/*15.06.2010    16.04.1997      Белых А.И.      matudw.c
Удаление накладной
1-удаляем все записи в накладной
2- удаляем список оплат к документа
3- удаляем услуги
4-удаляем шапку накладной
*/
#include	"buhg_g.h"

extern SQL_baza  bd;

void matudw(const char *nomdok,short d,short m,short g,int skl,
int tipz,GtkWidget *wpredok)
{
char		strsql[512];

//Удаляем услуги
sprintf(strsql,"delete from Dokummat3 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
g,m,d,skl,nomdok);
if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return;

//Удаляем список оплат к документа
sprintf(strsql,"delete from Opldok where ps=1 and datd='%04d-%02d-%02d' and pd=%d and nomd='%s'",
g,m,d,skl,nomdok);
if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return;

/*Удаляем записи в накладной*/

sprintf(strsql,"delete from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' and tipz=%d",
g,m,d,skl,nomdok,tipz);

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return;


sprintf(strsql,"delete from Dokummat2 where god=%d and nomd='%s' and \
sklad=%d",
g,nomdok,skl);

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return;

/*Удаляем накладную*/

sprintf(strsql,"delete from Dokummat where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' and tip=%d",
g,m,d,skl,nomdok,tipz);

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return;

}
