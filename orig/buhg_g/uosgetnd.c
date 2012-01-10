/*$Id: uosgetnd.c,v 1.5 2011-02-21 07:35:58 sasa Exp $*/
/*23.07.2007	23.07.2007	Белых А.И.	uosgetnd.c
Программа получения номера документа для подсистемы "Учёт основных средств"
*/
#include <stdlib.h>
#include "buhg_g.h"

extern SQL_baza bd;

void uosgetnd(short g,short tipz,class iceb_u_str *nomdok,GtkWidget *wpredok)
{
int             mpn;  /*Максимальный порядковый номер*/
int		i;
long		kolstr;
SQL_str         row;
char		strsql[512];
if(g == 0)
 return;
mpn=0;

sprintf(strsql,"select nomd from Uosdok where datd >= '%04d-01-01' and \
datd <= '%04d-12-31' and tipz=%d",g,g,tipz);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {

  i=0;

  if(tipz == 1)
   if(iceb_u_polen(row[0],strsql,sizeof(strsql),2,'+') == 0)
    i=atoi(strsql);

  if(tipz == 2)
   if(iceb_u_polen(row[0],strsql,sizeof(strsql),2,'-') == 0)
    i=atoi(strsql);

  if(i > mpn)
    mpn=i;

 }

mpn++;
memset(strsql,'\0',sizeof(strsql));
if(tipz == 1)
  sprintf(strsql,"+%d",mpn);
if(tipz == 2)
  sprintf(strsql,"-%d",mpn);
nomdok->new_plus(strsql);

}
