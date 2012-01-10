/* $Id: zapvkrw.c,v 1.9 2011-02-21 07:35:59 sasa Exp $ */
/*20.06.2004    26.09.1997      Белых А.И.      zapvkrw.c
Запись в карточку материалла
*/
#include        <time.h>
#include        <unistd.h>
#include	"buhg_g.h"

extern SQL_baza bd;

int zapvkrw(short dp,short mp,short gp,const char *nn,int skl,
int nk,short d,short m,short g,int tp,double kolih,double cena,GtkWidget *wpredok)
{
char		strsql[512];
char		bros[512];
time_t		vrem;

time(&vrem);

sprintf(strsql,"insert into Zkart \
values (%d,%d,'%s','%04d-%02d-%02d','%04d-%02d-%02d',%d,%.10g,%.10g,%d,%ld)",
skl,nk,nn,gp,mp,dp,g,m,d,tp,kolih,cena,iceb_getuid(wpredok),vrem);

/*printw("strsqk=%s\n",strsql);*/

if(sql_zap(&bd,strsql) != 0)
 {
  sprintf(bros,"%s-%s",__FUNCTION__,gettext("Ошибка записи !"));
  iceb_msql_error(&bd,bros,strsql,wpredok);
  return(1);
 }
return(0);
}
