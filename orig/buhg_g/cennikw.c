/*$Id: cennikw.c,v 1.8 2011-02-21 07:35:51 sasa Exp $*/
/*10.11.2009	07.05.2003	Белых А.И.	cennikw.c
Программа печати ценника на товар
*/
#include	<stdio.h>
#include "buhg_g.h"


void	cennikw(short d,short m,short g, //Дата распечатки ценника
char *kodtv, //Код товара
char *naim,
double cena,
char *shrihkod,
char *organ,
FILE *ff)
{
char  stroka[1024];

fprintf(ff,"\
┌────────────────────────────────────────┐\n");

sprintf(stroka,"%s:%s",gettext("Код товара"),kodtv);
fprintf(ff,"│%-*.*s│\n",iceb_u_kolbait(40,stroka),iceb_u_kolbait(40,stroka),stroka);

fprintf(ff,"│%-*.*s│\n",iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim);

sprintf(stroka,"%s:%.2f",gettext("Цена"),cena);

fprintf(ff,"│");
fprintf(ff,"\x0E"); //Двойная ширина

fprintf(ff,"\x1B\x45"); //Включить выделенный режим
fprintf(ff,"%-*.*s",iceb_u_kolbait(20,stroka),iceb_u_kolbait(20,stroka),stroka);
fprintf(ff,"\x1B\x46"); //Выключить выделенный режим
fprintf(ff,"\x14"); //отмена двойной ширины
fprintf(ff,"│\n");

sprintf(stroka,"%s:%s",gettext("Щтрих код"),shrihkod);
fprintf(ff,"│%-*.*s│\n",iceb_u_kolbait(40,stroka),iceb_u_kolbait(40,stroka),stroka);

sprintf(stroka,"%d.%d.%d %s",d,m,g,organ);
fprintf(ff,"│%-*.*s│\n",iceb_u_kolbait(40,stroka),iceb_u_kolbait(40,stroka),stroka);

fprintf(ff,"\
└────────────────────────────────────────┘\n");

}
