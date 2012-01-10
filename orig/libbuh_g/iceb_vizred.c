/*$Id: iceb_vizred.c,v 1.10 2011-05-06 08:37:49 sasa Exp $*/
/*21.04.2011	27.09.1999	Белых А.И.	iceb_vizred.c
Запуск тестового редактора
*/
#include <errno.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        "iceb_libbuh.h"


int iceb_vizred(const char *imaf,const char *editor,GtkWidget *wpredok)
{
class iceb_clock kkk(wpredok);

char		faj[512];
int		voz;

/*Проверяем возможность открытия файла*/
FILE *ff;

if((ff=fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,__FUNCTION__,errno,wpredok);
  return(1);
 }
fclose(ff);

class iceb_u_str redaktor("");
if(editor == NULL || editor[0] == '\0')
  iceb_poldan("Редактор для графического интерфейса",&redaktor,"nastsys.alx",wpredok);
else 
 redaktor.new_plus(editor);

sprintf(faj,"%s %s",redaktor.ravno(),imaf);
printf("%s-%s\n",__FUNCTION__,faj);
iceb_refresh();

voz=system(faj);

return(voz);

}
/*********************************/
int iceb_vizred(const char *imaf,GtkWidget *wpredok)
{
return(iceb_vizred(imaf,NULL,wpredok));
}