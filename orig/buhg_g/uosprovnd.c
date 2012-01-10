/*$Id: uosprovnd.c,v 1.4 2011-01-13 13:49:55 sasa Exp $*/
/*25.10.2007	25.10.2007	Белых А.И.	uosprovnd.c
Проверка номера документа для подсистемы "Учёт основных средств"
Если вернули 0-документа нет
	       1-есть
 */
#include "buhg_g.h"

int uosprovnd(short g,int tz,const char *ndk,GtkWidget *wpredok)
{
char		strsql[512];

sprintf(strsql,"select nomd from Uosdok where datd >= '%04d-01-01' and \
datd <= '%d-12-31' and nomd='%s'",g,g,ndk);
if(iceb_sql_readkey(strsql,wpredok) >= 1)
 return(1);

return(0);
 
}
