/*$Id: prdppw.c,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*13.06.2004	13.06.2004	Белых А.И.	prdppw.c
Получение даты первого прихода в карточку
*/
#include "buhg_g.h"

int		prdppw(int skl,int nk,short *dp,short *mp,short *gp,GtkWidget *wpredok)
{
SQLCURSOR cur;
SQL_str         row;
char		strsql[512];
short		d,m,g;

*dp=0;
*mp=0;
*gp=0;

sprintf(strsql,"select datdp from Zkart where sklad=%d and nomk=%d and \
tipz=1 order by datdp asc limit 1",skl,nk);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
  iceb_u_rsdat(&d,&m,&g,row[0],2);
else
  return(1);

*dp=d;
*mp=m;
*gp=g;

return(0);

}
