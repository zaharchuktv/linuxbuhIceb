/*$Id: vdndw.c,v 1.10 2011-01-13 13:49:55 sasa Exp $*/
/*08.08.2004	20.07.2004	Белых А.И.	vdndw.c
Вызов подпрограмм ввода и корректировки шапки приходных и расходных документов
*/
#include "buhg_g.h"

int  vdnd1(iceb_u_str*,iceb_u_str *skl,iceb_u_str *nomdok,const char *kodop,int,GtkWidget *wpredok);
int  vdnd2(iceb_u_str*,iceb_u_str *skl,iceb_u_str *nomdok,const char *kodop,int,GtkWidget *wpredok);

int vdndw(int tipz,iceb_u_str *datad,iceb_u_str *skl,iceb_u_str *nomdok,iceb_u_str *kodop,GtkWidget *wpredok)
{
int voz=-1;

if(tipz == 1)
 voz=vdnd1(datad,skl,nomdok,kodop->ravno(),0,wpredok);
if(tipz == 2)
 voz=vdnd2(datad,skl,nomdok,kodop->ravno(),0,wpredok);

return(voz);
}
