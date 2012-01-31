/*$Id: iceb_poldan_vkl.c,v 1.2 2011-04-18 09:10:21 sasa Exp $*/
/*14.04.2011	14.04.2011	Белых А.И..	iceb_poldan_vkl.c
Определение включения настройки 0-включено
                                1-нет
*/
#include "iceb_libbuh.h"

int iceb_poldan_vkl(const char *strpoi,const char *imaf,GtkWidget *wpredok)
{
class iceb_u_str vkl("");
int voz=1;
if(iceb_poldan(strpoi,&vkl,imaf,wpredok) == 0)
 {
  if(iceb_u_SRAV("Вкл",vkl.ravno(),1) == 0)
   voz=0;
 }

return(voz);

}
