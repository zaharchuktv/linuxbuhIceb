/*$Id: kasothkcn.c,v 1.5 2011-02-21 07:35:52 sasa Exp $*/
/*01.10.2006	01.10.2006	Белых А.И.	kasothkcn.c
Отчёты в "Учёте касствых ордеров
*/
#include "buhg_g.h"
#include "kasothkcn.h"

int   kasothkcn_m(class kasothkcn_rr *rek_ras);
int kasothkcn_r(class kasothkcn_rr *datark,GtkWidget *wpredok);




void kasothkcn()
{
static class kasothkcn_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(kasothkcn_m(&data) != 0)
 return;

if(kasothkcn_r(&data,NULL) != 0)
   return;


iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
