/*$Id: itogw.c,v 1.11 2011-02-21 07:35:52 sasa Exp $*/
/*14.12.2010	18.02.2003	Белых А.И.	itodw.c
Вывод итога в ведомости на выдачу зарплаты
*/

#include        <errno.h>
#include        <string.h>
#include <unistd.h>
#include        "buhg_g.h"


void itogw(FILE *kaw1,double itogo,GtkWidget *wpredok)
{
char		st[512];

memset(st,'\0',sizeof(st));
iceb_u_prnb(itogo,st);
fprintf(kaw1,"\
------------------------------------------------------------------------------\n\
%*s:%15s\n",iceb_u_kolbait(42,gettext("Итого")),gettext("Итого"),st);



iceb_insfil("zarspv1.alx",kaw1,wpredok);


iceb_podpis(kaw1,wpredok);

}
