/*$Id: hrasactw.c,v 1.6 2011-02-21 07:35:51 sasa Exp $*/
/*21.07.2003	20.07.2003	Белых А.И.	hrasactw.c
Шапка акта приема передачи материалов
*/
#include        <time.h>
#include        <errno.h>
#include	"buhg_g.h"

extern char	*organ;

void	hrasactw(int *kolstrok,int *kollist,FILE *ff)
{
struct  tm      *bf;
time_t		tmm;

time(&tmm);
bf=localtime(&tmm);
short god=bf->tm_year+1900;


fprintf(ff,"%s\n",organ);
fprintf(ff,"\
                                                                              %s\n\
                                                                       %s___________________ \n\
                                                                        ____________________\n\
                                                                       \"_____\"____________%d%s\n",
gettext("Утверждаю"),
gettext("Руководитель"),
god,
gettext("г."));

fprintf(ff,"%47s %s\n"," ",gettext("АКТ"));

fprintf(ff,"%39s \"_____\"____________%d%s\n",
" ",god,gettext("г."));

fprintf(ff,"\
%s:\n\
%s:\n\
%s:\n\
%s:\n\n\n\n",
gettext("Основание"),
gettext("Коммися в составе"),
gettext("Председатель коммиссии"),
gettext("Члены коммисии"));

fprintf(ff,"\n\
\"_____\"____________%d%s %s\n\n\n",
god,
gettext("г."),
gettext("комиссия составила акт приёмки под отчет товароматериальных ценностей"));

*kolstrok=19;
*kollist=1;

sapkkivw(kolstrok,*kollist,ff);
}
