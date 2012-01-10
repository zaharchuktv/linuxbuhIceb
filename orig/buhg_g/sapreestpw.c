/*$Id: sapreestpw.c,v 1.6 2011-02-21 07:35:57 sasa Exp $*/
/*04.10.2006	04.10.2006	Белых А.И.	sapreestpw.c
Шапка реестра заработной платы по подразделениям
*/

#include <stdio.h>
#include <libintl.h>

extern char *organ;

void sapreestpw(short mr,short gr,
int metka, //0-не бюджет 1-бюджет
FILE *ffpodr)
{
fprintf(ffpodr,"%s\n\n",organ);
if(metka == 1)
  fprintf(ffpodr,"%s.",gettext("Бюджет"));

fprintf(ffpodr,"%s %02d.%d%s\n",
gettext("Реестр платежных ведомостей за"),mr,gr,
gettext("г."));

fprintf(ffpodr,"\
--------------------------------------------------------\n");
fprintf(ffpodr,gettext("\
Код|     Наименование подразделения         |  Сумма   |\n"));
fprintf(ffpodr,"\
--------------------------------------------------------\n");
}
