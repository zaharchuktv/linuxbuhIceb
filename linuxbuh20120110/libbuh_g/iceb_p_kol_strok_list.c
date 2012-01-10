/*$Id: iceb_p_kol_strok_list.c,v 1.5 2011-01-13 13:49:59 sasa Exp $*/
/*14.11.2004	14.11.2004	Белых А.И.	iceb_p_kol_strok_list.c
Получить количество строк на стандартном листе
*/
#include <stdlib.h>
#include "iceb_libbuh.h"

int  iceb_p_kol_strok_list(GtkWidget *wpredok)
{

int kol_strok=64;
char bros[512];

if(iceb_poldan("Количество строк на стандартном листе",bros,"nastsys.alx",wpredok) == 0)
  kol_strok=atoi(bros);

return(kol_strok);

}
