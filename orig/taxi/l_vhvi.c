/*$Id: l_vhvi.c,v 1.4 2011-01-13 13:50:09 sasa Exp $*/
/*13.12.2005	13.12.2005	Белых А.И.	l_vhvi.c
Корневая программа работы с просмотром входов-выходов
*/
#include "i_rest.h"
#include "l_vhvi.h"

int l_vhvi_p(class l_vhvi_rp *datap,GtkWidget *wpredok);
int  l_vhvi_r(class l_vhvi_rp *datap,GtkWidget *wpredok);

void l_vhvi()
{

class l_vhvi_rp data;


for(;;)
 {
  if(l_vhvi_p(&data,NULL) == 0)
   return;


   l_vhvi_r(&data,NULL);
 }
}
