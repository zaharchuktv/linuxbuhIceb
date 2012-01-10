/*$Id: kasothw.c,v 1.10 2011-04-14 16:09:35 sasa Exp $*/
/*16.06.2006	10.03.2006	Белых А.И.	kasothw.c
Отчёты в "Учёте касствых ордеров
*/
#include "buhg_g.h"
#include "kasothw.h"

int kasothw_m(class kasothw_rr *rek_ras,GtkWidget *wpredok);
int kasothw_r(class kasothw_rr *datark,GtkWidget *wpredok);
int kasothw_r1(class kasothw_rr *datark,GtkWidget *wpredok);




void kasothw(int metka)
{
static class kasothw_rr data;
data.imaf.free_class();
data.naimf.free_class();

if(metka == 0)
 data.naim_oth.new_plus(gettext("Распечатка списка документов"));
if(metka == 1)
 data.naim_oth.new_plus(gettext("Распечатать свод по операциям"));

if(kasothw_m(&data,NULL) != 0)
 return;

if(metka == 0)
  if(kasothw_r(&data,NULL) != 0)
   return;

if(metka == 1)
  if(kasothw_r1(&data,NULL) != 0)
   return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
