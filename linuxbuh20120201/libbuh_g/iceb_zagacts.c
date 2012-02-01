/*$Id: iceb_zagacts.c,v 1.7 2011-02-21 07:36:08 sasa Exp $*/
/*11.11.2008	18.12.2003	Белых А.И.	iceb_zagacts.c
Заголовок акта сверки
*/
#include <stdio.h>
#include <time.h>
#include "iceb_libbuh.h"

extern char *organ;

void   iceb_zagacts(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kodkontr,const char *naimkont,FILE *ff)
{
time_t vrem;
struct tm *bf;

time(&vrem);
bf=localtime(&vrem);



fprintf(ff,gettext("\
                                       АКТ СВЕРКИ ВЗАИМНЫХ РАСЧЕТОВ.\n\n\
   Мы ниже подписавшиеся, главный бухгалтер %s\n\
________________________________________________________________________________________________________\n\
                    (наименование учереждения, фамилия и инициалы)\n\
и главный бухгалтер %s\n\
_________________________________________________________________________________________________________\n\
                   (наименование учереждения, фамилия и инициалы)\n\
составили настоящий акт о том, что проведена проверка взаимных расчетов.\n"),
organ,naimkont);

fprintf(ff,"%s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
gettext("За период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));

fprintf(ff,"%s:%02d.%02d.%d %s\n\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,gettext("г."));

fprintf(ff,"%s:\n",gettext("При проверке установлено"));

fprintf(ff,"\
%s %-*s %s %s %s\n",
gettext("По"),
iceb_u_kolbait(51,organ),organ,
gettext("По"),
kodkontr,naimkont);



}
