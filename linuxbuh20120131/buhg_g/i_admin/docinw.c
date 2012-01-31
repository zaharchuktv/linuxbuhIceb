/*$Id: docinw.c,v 1.4 2011-02-21 07:35:51 sasa Exp $*/
/*17.10.2010	17.10.2010	Белых А.И.	Белых А.И.	docinw.c
Загрузка в таблицу документации
*/
#include <glib.h>
#include <errno.h>
//zs
//#include  "buhg_g.h"
#include  "../headers/buhg_g.h"
//ze

void docinw(const char *put_na_doc,GtkWidget *wpredok)
{
int nomer_str=0;
char stroka[1024];
char strsql[512];
FILE *ff;

GDir *dirp;
if((dirp=g_dir_open(put_na_doc,0,NULL)) == NULL)
 {
  perror(__FUNCTION__);
  class iceb_u_str repl;
  repl.plus(__FUNCTION__);
  repl.ps_plus(gettext("Не могу открыть каталог !"));
  repl.ps_plus(put_na_doc);
  repl.ps_plus(strerror(errno));
  iceb_menu_soob(repl.ravno(),wpredok);
  
 }

if(iceb_sql_zapis("delete from Doc",1,0,wpredok) != 0)
   return;

class iceb_u_str imafil;
class iceb_u_str rashir;
const char *imafil_read;
class iceb_u_str strin("");
while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  if(iceb_u_polen(imafil_read,&rashir,2,'.') != 0)
   continue;

  if(iceb_u_SRAV("txt",rashir.ravno(),0) != 0 && iceb_u_SRAV("TXT",rashir.ravno(),0) != 0)
   continue;

  
  imafil.new_plus(put_na_doc);
  imafil.plus(G_DIR_SEPARATOR_S,imafil_read);
//  wprintw(win,"Загрузка файла-%s\n",imafil.ravno());
//  wrefresh(win);

  if((ff=fopen(imafil.ravno(),"r")) == NULL)
   {
    iceb_er_op_fil(imafil.ravno(),"",errno,wpredok);
    return;
   }

  while(fgets(stroka,sizeof(stroka),ff) != NULL)
   {
    strin.new_plus(stroka);  
    sprintf(strsql,"insert into Doc values('%s',%d,'%s')",
    imafil_read,++nomer_str,strin.ravno_filtr());

    iceb_sql_zapis(strsql,1,0,wpredok);
   }
  fclose(ff);

 }
g_dir_close(dirp);
}
