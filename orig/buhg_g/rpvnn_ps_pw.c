/*$Id: rpvnn_ps_pw.c,v 1.4 2011-01-13 13:49:53 sasa Exp $*/
/*16.11.2009	31.08.2005	Белых А.И.	prvnn_ps_pw.c
Шапка для протокола расчёта реестра налогвых накладных в подсистеме
"Реестр налоговых накладных"
*/

#include <stdio.h>

void rpvnn_ps_pw(FILE *ff_prot)
{

fprintf(ff_prot,"\
---------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_prot,"\
  N  |Дата п/в  |Номер н.н.|Сумма по д.|Сумма НДС|M|Груп.|Инд.нал.ном.|Подс.|Дата док. |Номер док.|Наимeнование контрагента\n");
/*
12345 1234567890 1234567890 1234567890 12345678 1 12345 123456789012 12345 1234567890 1234567890 
*/
fprintf(ff_prot,"\
---------------------------------------------------------------------------------------------------------------------------\n");

}
