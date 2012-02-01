/*$Id: iceb_u_print.c,v 1.19 2011-01-13 13:50:05 sasa Exp $*/
/*14.02.2010	21.01.2004	Белых А.И.	iceb_u_print.c
Печать файла на системном принтере
*/
#include <glib.h>
#include <stdlib.h>
#include "iceb_util.h"


int iceb_u_print(const char *imaf,int kolkop,const char *progp,const char *name_printer,const char *kluh_peh,
const char *kluh_name_pr,
const char *kluh_kol_kop)
{
char comanda[1024];

if(kolkop == 0)
 return(0);
 
//переписываем
sprintf(comanda,"cp %s ",imaf);
strcat(comanda,g_get_tmp_dir());
strcat(comanda,G_DIR_SEPARATOR_S);
strcat(comanda,imaf);

system(comanda);


//Печатаем

sprintf(comanda,"%s ",progp);
if(kluh_peh != NULL)
 if(kluh_peh[0] != '\0' && kluh_peh[0] != '\0')
  {
   strcat(comanda,kluh_peh);
   strcat(comanda," ");
  }

if(name_printer != NULL)
 if(name_printer[0] != '\0' && kluh_name_pr[0] != '\0')
  {
//   sprintf(comanda,"%s %s -P%s -#%d /tmp/%s",progp,kluh_peh,name_printer,kolkop,imaf);
//   strcat(comanda,"-P");
   strcat(comanda,kluh_name_pr);
   strcat(comanda,name_printer);
   strcat(comanda," ");
  }
// else
//  sprintf(comanda,"%s %s -#%d /tmp/%s",progp,kluh_peh,kolkop,imaf);

if(kolkop > 1)
 {
  //strcat(comanda,"-#");

  strcat(comanda,kluh_kol_kop);

  char bros[56];
  sprintf(bros,"%d",kolkop);

  strcat(comanda,bros);
  strcat(comanda," ");
 }

//strcat(comanda,"/tmp/");
strcat(comanda,g_get_tmp_dir());
strcat(comanda,G_DIR_SEPARATOR_S);

strcat(comanda,imaf);


printf("%s: %s\n",__FUNCTION__,comanda);

return(system(comanda));


}
