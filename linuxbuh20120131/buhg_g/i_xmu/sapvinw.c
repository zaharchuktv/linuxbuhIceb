/*$Id: sapvinw.c,v 1.12 2011-02-21 07:35:57 sasa Exp $*/
/*14.12.2010	03.08.2003	Белых А.И.	sapvinw.c
Шапка инвентаризационной ведомости
*/
#include <unistd.h>
#include        "../headers/buhg_g.h"

extern char     *organ;



void sapvinw(const char *matot,const char *mesqc,short g,short gos,int *kolstriv,int *kollistiv,FILE *ff2)
{

*kolstriv+=13;


fprintf(ff2,"%s\n\n\
%40s %s N______\n\
%40s %s\n\
%40s \"_____\"_____________%d%s\n",
organ,
" ",gettext("Инвентаризационная опись"),
" ",gettext("товарно-материальных ценностей"),
" ",gos,gettext("г."));

fprintf(ff2,gettext("\
                          РАСПИСКА\n\
 К началу проведения инвентаризации все расходные и приходные документы\n\
на ценности сданы в бухгалтерию и все ценности, поступившие на мою\n\
(нашу) ответственность, оприходованы, а выбывшие списаны в расход.\n\n"));

fprintf(ff2,"\n\
%s __________________________%s\n\n",
gettext("Материально-ответственное лицо (лица)"),
matot);

*kolstriv=13;

*kolstriv+=iceb_insfil("inventm.alx",ff2,NULL);


fprintf(ff2,"\n\
%*s:%*s \"______\" %s %d%s\n\
%15s %*s \"______\" %s %d%s\n",
iceb_u_kolbait(15,gettext("Инвентаризация")),gettext("Инвентаризация"),
iceb_u_kolbait(10,gettext("начата")),gettext("начата"),
mesqc,g,
gettext("г."),
" ",iceb_u_kolbait(10,gettext("окончена")),gettext("окончена"),
mesqc,g,
gettext("г."));

fprintf(ff2,"\n\%s:\n",gettext("При инвентаризации установлено следующее"));
*kolstriv+=5;

*kollistiv=1;
int kkk=1;
sapkkivw(kolstriv,kkk,ff2);

}
