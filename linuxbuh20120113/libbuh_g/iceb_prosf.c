/*$Id: iceb_prosf.c,v 1.14 2011-01-13 13:49:59 sasa Exp $*/
/*08.08.2010	24.08.2006	Белых А.И.	iceb_prosf.c
Запуск программы просмотра файла
*/
#include <errno.h>
#include <stdlib.h>
#include "iceb_libbuh.h"

void iceb_prosf(const char *imaf,GtkWidget *wpredok)
{

/*Проверяем возможность открытия файла*/
FILE *ff;

if((ff=fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,__FUNCTION__,errno,wpredok);
  return;
 }
fclose(ff);




char prog_pros[512];
memset(prog_pros,'\0',sizeof(prog_pros));

if(iceb_poldan("Просмотрщик файлов для графического интерфейса",prog_pros,"nastsys.alx",wpredok) != 0)
 {
  class iceb_u_str repl;
  repl.plus("iceb_prosf-В файле ");
  repl.plus("nastsys.alx");
  repl.ps_plus("Не найдена настройка \"Просмотрщик файлов для графического интерфейса\" !!!");
  return;
 }
char comanda[1024];
if(strchr(prog_pros,'"') != NULL) /*значит символ есть в строке*/
  sprintf(comanda,"%s %s\"",prog_pros,imaf);
else
  sprintf(comanda,"%s %s",prog_pros,imaf);

iceb_refresh();

  printf("%s-1\n",__FUNCTION__);

class iceb_clock kkk(wpredok);

system(comanda);


}
