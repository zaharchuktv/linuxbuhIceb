/*$Id: iceb_delfil.c,v 1.7 2011-02-21 07:36:06 sasa Exp $*/
/*29.09.2009	03.04.2008	Белых А.И.	iceb_delfil.c
Удаление всех файлов в каталоге по заданному расширению
Если вернули 0-удалили
             1-нет
*/
#include <errno.h>
#include <unistd.h>
#include	"iceb_libbuh.h"
#include        <string.h>
#include 	<dirent.h>
//#include        <sys/ndir.h>
#include	<stdlib.h>



int iceb_delfil(const char *katalog,const char *rashir,GtkWidget *wpredok)
{
class iceb_u_str imafil("");
char filr[112];
int kol_pol=0;
GDir *dirp;

if((dirp=g_dir_open(katalog,0,NULL)) == NULL)
 {
  perror(__FUNCTION__);
  class iceb_u_str repl;
  repl.plus(__FUNCTION__);
  repl.ps_plus(gettext("Не могу открыть каталог !"));
  repl.ps_plus(katalog);
  repl.ps_plus(strerror(errno));
  iceb_menu_soob(&repl,wpredok);
  
  return(1);
 }
const char *imafil_read;
while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  
  if((kol_pol=iceb_u_pole2(imafil_read,'.')) == 0)
   continue;
  iceb_u_polen(imafil_read,filr,sizeof(filr),kol_pol,'.');
  
  if(iceb_u_proverka(rashir,filr,0,1) != 0)
   continue;

  imafil.new_plus(katalog);
  imafil.plus(G_DIR_SEPARATOR_S,imafil_read);
  unlink(imafil.ravno());
 }
g_dir_close(dirp);
#if 0
#######################3

DIR             *dirp;
struct dirent *dppp;
if((dirp = opendir(katalog)) == NULL) 
 {
  perror(__FUNCTION__);
  class iceb_u_str repl;
  repl.plus("iceb_t_delfil");
  repl.ps_plus(gettext("Не могу открыть каталог !"));
  repl.ps_plus(katalog);
  repl.ps_plus(strerror(errno));
  iceb_menu_soob(&repl,wpredok);
  
  return(1);
 }

while((dppp = readdir(dirp)) != NULL) 
 {
  if(dppp->d_name[0] == '/') /*пропускаем каталоги*/
   continue;
  /*Пропускаем точку и две точки*/
  if(iceb_u_SRAV(dppp->d_name,".",0) == 0)
   continue;  
  if(iceb_u_SRAV(dppp->d_name,"..",0) == 0)
   continue;  
  
  if((kol_pol=iceb_u_pole2(dppp->d_name,'.')) == 0)
   continue;
  iceb_u_polen(dppp->d_name,filr,sizeof(filr),kol_pol,'.');

  if(iceb_u_proverka(rashir,filr,0,1) != 0)
   continue;

  sprintf(imafil,"%s%s%s",katalog,G_DIR_SEPARATOR_S,dppp->d_name);  
  unlink(imafil);
 }

closedir(dirp);
#endif

return(0);


}
