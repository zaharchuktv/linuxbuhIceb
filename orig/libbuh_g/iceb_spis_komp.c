/*$Id: iceb_spis_komp.c,v 1.9 2011-02-21 07:36:08 sasa Exp $*/
/*20.03.2010	13.12.2006	Белых А.И.	iceb_spis_komp.c
Подпрограмма для передачи файла на компъютер работающий под Windows
Если вернули 0 - файл передан 
             1 - нет
*/
#include <glib.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include	"iceb_libbuh.h"
#include <unistd.h>

int iceb_vperecod(const char *imaf,GtkWidget *wpredok);

int iceb_spis_komp(const char *imaf,GtkWidget *wpredok)
{
char imaf_copy[56];
int metka_perecod=0;
struct passwd *rek_login;

if((metka_perecod=iceb_vperecod(imaf,wpredok))  < 0)
 return(1);


rek_login=getpwuid(getuid());

class iceb_u_str imaf_nast;

imaf_nast.plus(rek_login->pw_dir);
imaf_nast.plus(G_DIR_SEPARATOR_S,".iceB");
imaf_nast.plus(G_DIR_SEPARATOR_S,"spis_komp.alx");
FILE *ff;

if((ff = fopen(imaf_nast.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imaf_nast.ravno(),"",errno,wpredok);
  return(1);
 }
char strok[1024];
class iceb_u_spisok str_nast;
class iceb_u_spisok naim_komp;

char bros[1024];
while(fgets(strok,sizeof(strok),ff) != NULL)
 {
  if(strok[0] == '#')
   continue;
  if(iceb_u_polen(strok,bros,sizeof(bros),1,'|') != 0)
   continue;
  if(bros[0] == '\0')
   continue;
  naim_komp.plus(bros);
  str_nast.plus(strok);   
 }
fclose(ff);
class iceb_u_str ima_mah;
class iceb_u_str ima_kat;
class iceb_u_str parol;
class iceb_u_str prog;

if(naim_komp.kolih() == 0)
 {
  iceb_menu_soob(gettext("Не найдено настроек !"),wpredok);
  return(1);
 }


class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное."));


int voz=0;
if(naim_komp.kolih() > 1)
 {
  voz=iceb_menu_mv(&titl,&zagolovok,&naim_komp,0,wpredok);
  if(voz < 0)
   return(1);
 }



if(metka_perecod > 0)
 {
  sprintf(imaf_copy,"iceb_t_spis_komp%d.tmp",getpid());
  iceb_cp(imaf,imaf_copy,0,wpredok);
  iceb_perecod(metka_perecod,imaf,wpredok);
 } 

iceb_u_polen(str_nast.ravno(voz),&ima_mah,2,'|');   
iceb_u_polen(str_nast.ravno(voz),&ima_kat,3,'|');   
iceb_u_polen(str_nast.ravno(voz),&parol,4,'|');   
iceb_u_polen(str_nast.ravno(voz),&prog,5,'|');   

sprintf(bros,"%s %s %s %s %s %s",prog.ravno(),ima_mah.ravno(),imaf,imaf,ima_kat.ravno(),parol.ravno());

int i=system(bros);

if(metka_perecod > 0)
 rename(imaf_copy,imaf);

class iceb_u_str repl;

if(i != 0)
 {
  char strsql[512];
  sprintf(strsql,gettext("Передача файла не произошла ! Код ошибки %d\n%s"),i,bros);
  repl.plus(strsql);
 }

if(i == 0)
  repl.plus(gettext("Передача файла произошла успешно"));

iceb_menu_soob(&repl,wpredok);


return(i);
}



