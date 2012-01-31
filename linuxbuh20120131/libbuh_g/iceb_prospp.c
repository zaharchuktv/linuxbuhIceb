/*$Id: iceb_prospp.c,v 1.8 2011-01-13 13:49:59 sasa Exp $*/
/*16.11.2010	26.10.2005	Белых А.И.	iceb_prospp.c
Просмотр перед печатью файла
*/
#include <stdlib.h>
#include	"iceb_libbuh.h"
#include        <unistd.h>


int iceb_prospp(const char *imaf,GtkWidget *wpredok)
{
int voz=0;

iceb_print_operation(GTK_PRINT_OPERATION_ACTION_PREVIEW,imaf,wpredok);
#if 0
###############################33333
char bros[1024];

class iceb_u_str ppsf;
iceb_poldan("Просмотрщик PostScript файлов",&ppsf,"nastsys.alx",wpredok);
class iceb_u_str filtr_iceb_ps;

iceb_poldan("Фильт Epson->PostScript",&filtr_iceb_ps,"nastsys.alx",wpredok);



char imaf_tmp[32];
sprintf(imaf_tmp,"%s.ps",imaf);

sprintf(bros,"%s %s %s > %s",filtr_iceb_ps.ravno(),klush_s_fil,imaf,imaf_tmp);
printf("iceb_prospp-%s\n",bros);
if((voz=system(bros)) != 0)
 {
  unlink(imaf_tmp);
  return(voz);
 }

sprintf(bros,"%s %s",ppsf.ravno(),imaf_tmp);
printf("iceb_prospp-%s\n",bros);
voz=system(bros);

unlink(imaf_tmp);
############################33
#endif

return(voz);

}
