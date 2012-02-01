/*$Id: admin_cp_alxw.c,v 1.6 2011-02-21 07:35:50 sasa Exp $*/
/*12.09.2010	12.03.2010	Белых А.И.	admin_cp_alx.c
Установка настроечных файлов в нужный каталог
*/
#include <glib.h>
#include <sys/stat.h>
#include        <errno.h>
//zs
//#include  "buhg_g.h"
#include  "../headers/buhg_g.h"
//ze


int admin_cp_alx(const char *from_alx,const char *to_alx,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
char stroka[2048];

umask(0000); 

if(mkdir(to_alx,0775) != 0)
 {
//  printw("%d\n%d\n%d\n%d\n%d\n%d\n",EPERM,EFAULT,EACCES,ENAMETOOLONG,ENOENT,EEXIST);
  if(errno != EEXIST) //Уже есть такой каталог
   { 
    class iceb_u_str repl;
    repl.plus(gettext("Не могу создать каталог !"));
    sprintf(stroka,"%d %s",errno,strerror(errno));
    repl.ps_plus(stroka);
    
    repl.ps_plus(to_alx);
    iceb_menu_soob(&repl,wpredok);
    umask(0117); /*Установка маски для записи и чтения группы*/
    return(1);
   }
 }

umask(0117); /*Установка маски для записи и чтения группы*/

class iceb_u_str imafil_from;
class iceb_u_str imafil_to;

imafil_from.new_plus(from_alx);
imafil_from.plus(G_DIR_SEPARATOR_S,"icebstart.alx");

imafil_to.new_plus(to_alx);
imafil_to.plus(G_DIR_SEPARATOR_S,"icebstart.alx");

sprintf(stroka,"%s -> %s\n",imafil_from.ravno(),to_alx);
iceb_printw(stroka,buffer,view);

iceb_cp(imafil_from.ravno(),imafil_to.ravno(),2,wpredok);



imafil_from.new_plus(from_alx);
imafil_from.plus(G_DIR_SEPARATOR_S,"icebstartw.alx");

imafil_to.new_plus(to_alx);
imafil_to.plus(G_DIR_SEPARATOR_S,"icebstartw.alx");


sprintf(stroka,"%s -> %s\n",imafil_from.ravno(),to_alx);
iceb_printw(stroka,buffer,view);
iceb_cp(imafil_from.ravno(),imafil_to.ravno(),2,wpredok);

/**************************************
GDir *dirp;
if((dirp=g_dir_open(from_alx,0,NULL)) == NULL)
 {
  perror(__FUNCTION__);
  class iceb_u_str repl;
  repl.plus(__FUNCTION__);
  repl.ps_plus(gettext("Не могу открыть каталог !"));
  repl.ps_plus(from_alx);
  repl.ps_plus(strerror(errno));
  iceb_menu_soob(&repl,wpredok);
  
  return(1);
 }
class iceb_u_str imafil_from;
class iceb_u_str imafil_to;
const char *imafil_read;
while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  

  imafil_from.new_plus(from_alx);
  imafil_from.plus(G_DIR_SEPARATOR_S,imafil_read);

  imafil_to.new_plus(to_alx);
  imafil_to.plus(G_DIR_SEPARATOR_S,imafil_read);
  
  sprintf(stroka,"%s -> %s\n",imafil_from.ravno(),to_alx);
  iceb_printw(stroka,buffer,view);
  iceb_cp(imafil_from.ravno(),imafil_to.ravno(),2,wpredok);
 }
g_dir_close(dirp);
******************************/

return(0);
}