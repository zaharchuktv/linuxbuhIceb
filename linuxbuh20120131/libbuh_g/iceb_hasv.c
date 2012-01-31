/*$Id: iceb_hasv.c,v 1.7 2011-02-21 07:36:07 sasa Exp $*/
/*02.03.2008	02.03.2008	Белых А.И.	iceb_hasv.c
Распечатка шапки акта сверки
*/
#include "iceb_libbuh.h"

void iceb_hasv(char *shet,FILE *ff,GtkWidget *wpredok)
{
char	strsql[512];
SQL_str row;
SQLCURSOR cur;
char naim[80];

memset(naim,'\0',sizeof(naim));
sprintf(strsql,"select nais from Plansh where ns='%s'",shet);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 strncpy(naim,row[0],sizeof(naim)-1);
 
fprintf(ff,"\n%s:%s %s\n",gettext("Счет"),shet,naim);
fprintf(ff,"\
-------------------------------------------------- ---------------------------------------------------\n");
fprintf(ff,gettext("\
  Дата    |   Коментарий       | Дебет  | Кредит | |  Дата    |   Коментарий       |  Дебет | Кредит |\n"));

fprintf(ff,"\
-------------------------------------------------- ---------------------------------------------------\n");

}
