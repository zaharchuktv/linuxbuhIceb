/*$Id: pib_end_dokw.c,v 1.5 2011-01-13 13:49:52 sasa Exp $*/
/*30.10.2009	28.02.2005	Белых А.И.	pib_end_dokw.c
Выдача концовки в файл
*/

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

void pib_end_dokw(int kol_na_liste,double sumalist,double itogoo,int nomer,
const char *rukov,const char *glavbuh,
FILE *ffkartr)
{
char strsql[512];
fprintf(ffkartr,"%-*s:%8.2f\n\n",iceb_u_kolbait(16,"Всього по листу"),"Всього по листу",sumalist);
fprintf(ffkartr,"Кількість одержувачів коштів по листу:%d чоловік\n",kol_na_liste);
memset(strsql,'\0',sizeof(strsql));

iceb_u_preobr(sumalist,strsql,0);

fprintf(ffkartr,"на суму:%s\n",strsql);

fprintf(ffkartr,"%*s:%8.2f\n",iceb_u_kolbait(16,"Разом"),"Разом",itogoo);


fprintf(ffkartr,"\nКількість одержувачів коштів: %d чоловік\n",nomer);

memset(strsql,'\0',sizeof(strsql));

iceb_u_preobr(itogoo,strsql,0);

fprintf(ffkartr,"На суму:%s\n\n",strsql);

fprintf(ffkartr,"\
              Керівник _______________%s\n\n\
    Головний бухгалтер _______________%s\n",rukov,glavbuh);

}
