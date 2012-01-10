/*$Id: iceb_progforlst.c,v 1.5 2011-08-29 07:13:46 sasa Exp $*/
/*20.03.2010	20.03.2010	Белых А.И.	iceb_t_progforlst.c
Чтение и выбор настройки для выполнения программы
Возвращаем 0- программа выполнена
           1- нет
*/
#include <glib.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include "iceb_libbuh.h"
#include <unistd.h>


int iceb_progforlst(const char *imaf,GtkWidget *wpredok)
{
class iceb_u_spisok str_menu;
class iceb_u_spisok prog;

char strok[1024];
char bros[1024];
//struct passwd *rek_login;

//rek_login=getpwuid(getuid());

class iceb_u_str imaf_nast;

//imaf_nast.plus(rek_login->pw_dir);
imaf_nast.plus(g_get_home_dir());
imaf_nast.plus(G_DIR_SEPARATOR_S,".iceB");
imaf_nast.plus(G_DIR_SEPARATOR_S,"progforlst.alx");

FILE *ff;

if((ff = fopen(imaf_nast.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imaf_nast.ravno(),"",errno,wpredok);
  return(1);
 }

while(fgets(strok,sizeof(strok),ff) != NULL)
 {
  if(strok[0] == '#')
   continue;
  if(iceb_u_polen(strok,bros,sizeof(bros),1,'|') != 0)
   continue;
  if(bros[0] == '\0')
   continue;

  str_menu.plus(bros);

  iceb_u_polen(strok,bros,sizeof(bros),2,'|');
  prog.plus(bros);
 }
fclose(ff);

if(str_menu.kolih() == 0)
 {
  iceb_menu_soob(gettext("Не найдено настроек !"),wpredok);
  return(1);
 }

int voz=0;
if(str_menu.kolih() > 1)
 {
//  int kod_kl=0;
//  voz=dirmasiv(&str_menu,-1,-1,0,gettext("Выберите нужное и нажмите Enter"),1,&kod_kl);
  class iceb_u_spisok naim_kol;
  naim_kol.plus(gettext("Наименование операции"));
  iceb_u_str naim_menu;
  naim_menu.plus(gettext("Выбор базы данных"));
  if((voz=iceb_l_spisokm(&str_menu,&prog,&naim_kol,&naim_menu,0,wpredok)) < 0)
    return(1);

 }

sprintf(bros,"%s %s",prog.ravno(voz),imaf);

int kodv=system(bros);
if(kodv != 0)
 {
  sprintf(strok,"%s %d!\n%s",gettext("Ошибка выполнения программы"),kodv,bros);
  iceb_menu_soob(strok,wpredok);
 } 
return(kodv);
}



