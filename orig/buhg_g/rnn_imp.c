/*$Id: rnn_imp.c,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*06.05.2008	06.05.2008	Белых А.И.	rnn_imp.c
Импорт налоговых накладных в реестр из подсистем
Еслит вернули 0-был импорт
              1-нет
*/

#include "buhg_g.h"
#include "rnn_imp.h"

int rnn_imp_m(class rnn_imp_data *rek_ras,GtkWidget *wpredok);
int rnn_imp_r1(class rnn_imp_data *datark,GtkWidget *wpredok);
int rnn_imp_r2(class rnn_imp_data *datark,GtkWidget *wpredok);

int rnn_imp(int metka, /*1-иморт полученных 2-выданных*/
GtkWidget *wpredok)
{


static class rnn_imp_data data;


if(rnn_imp_m(&data,wpredok) != 0)
 return(1);

if(metka == 1)
 rnn_imp_r1(&data,wpredok);
if(metka == 2)
 rnn_imp_r2(&data,wpredok);

return(0);

}

