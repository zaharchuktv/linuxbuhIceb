/*$Id: iceb_f_redfil.c,v 1.16 2011-05-06 08:37:49 sasa Exp $*/
/*28.12.2010	19.10.2001	Белых А.И.	iceb_f_redfil.c
Проверка возможности редактирования файла и запуск редактирования
Если вернули 0 - вход в файл был
             1 - нет
*/
#include <errno.h>
#include        <stdlib.h>
#include        <unistd.h>
#include	"iceb_libbuh.h"


extern SQL_baza	bd;
extern char *imabaz;

int iceb_f_redfil(const char *imafil,int metka, //0-из базы 1-из текущего каталога 2- из базы без проверки доступа
const char *editor,
GtkWidget *wpredok)
{
int metka_vr=0; /*0- разрешено редактирование 1-только просмотр*/

if(wpredok != NULL)
  class iceb_clock kk(wpredok);


if(metka == 0 || metka == 2)
 {
  class iceb_u_str flag;
  //Установить флаг работы с файлом
  flag.plus(imabaz);
  flag.plus(imafil);

  class iceb_sql_flag flagrf;


  if(metka == 0)
    metka_vr=iceb_prn(wpredok); /*проверяем возможность корректировки настроек*/

  if(metka_vr == 0)
  if(flagrf.flag_on(flag.ravno()) != 0)
   {

    iceb_menu_soob(gettext("С файлом уже работает другой оператор !"),wpredok);
    return(1);
   }

  if(iceb_alxout(imafil,wpredok) != 0)
   return(1);

  rename(imafil,flag.ravno());
  
  if(metka_vr == 0)
   {
    iceb_vizred(flag.ravno(),editor,wpredok);
    rename(flag.ravno(),imafil);
    iceb_alxin(imafil,wpredok);
   }
  else
   {
    iceb_prosf(flag.ravno(),wpredok);
    rename(flag.ravno(),imafil);
   }
  unlink(imafil);
 }
else
 {
  printf("2222222222222222222\n");
  
  iceb_vizred(imafil,editor,wpredok);

 }
return(0);
}
/****************************************/
int iceb_f_redfil(const char *imafil,int metka, //0-из базы 1-из текущего каталога 2- из базы без проверки доступа
GtkWidget *wpredok)
{
return(iceb_f_redfil(imafil,metka,NULL,wpredok));

}
