/*$Id: hrvnks.c,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*09.12.2009	28.08.2003	Белых А.И.	hrvnks.c
Шапка 
*/
#include	<stdio.h>
extern char	*organ;

void	hrvnks(FILE *ff,int *kolstrok)
{
*kolstrok=6;
fprintf(ff,"\
                        С П И С О К   N     від\n\
на зарахування ________________ робітникам %s\n\
      за            перерахованої на карткові рахунки в філію\n\
   \"Вінницьке Центральне відділення ПАТ Промінвестбанк\"\n\
    згідно платіжного доручення N____ від ______________р.\n\n",
organ);

}