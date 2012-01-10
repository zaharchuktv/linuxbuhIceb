/*$Id: rnn_imp_ovd.c,v 1.2 2011-01-13 13:49:52 sasa Exp $*/
/*13.12.2009	13.12.2009	Белых А.И.	rnn_imp_ovd.c
Определение вида документа
*/
#include "buhg_g.h"

void rnn_imp_ovd(class iceb_u_str *viddok,const char *kodop,class iceb_u_spisok *spvd,class iceb_u_spisok *spko)
{
viddok->new_plus("");
for(int nom=0; nom < spvd->kolih(); nom++)
 if(iceb_u_proverka(spko->ravno(nom),kodop,0,1) == 0)
  {
   viddok->new_plus(spvd->ravno(nom));
   return;
  }
}
