/* $Id: sapkk1ivw.c,v 1.6 2011-01-13 13:49:53 sasa Exp $ */
/*21.07.2003    01.02.1994      Белых А.И.      sapkk1ivw.c
Счетчик строк
*/
#include	"../headers/buhg_g.h"
extern int kol_strok_na_liste;

void		sapkk1ivw(int *kolstrlist,int *kollist,FILE *ff1)
{

*kolstrlist+=1;
if(*kolstrlist <= kol_strok_na_liste)
 return;

*kollist+=1;
fprintf(ff1,"\f");
*kolstrlist=1;
sapkkivw(kolstrlist,*kollist,ff1);

}
