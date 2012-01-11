/*$Id: admin_newuserw.c,v 1.4 2011-02-21 07:35:51 sasa Exp $*/
/*03.09.2010	03.09.2010	Белых А.И.	admin_newuser.c
Ввод нового оператора в таблицу операторов конкретной базы данных
*/
#include <pwd.h>
//zs
//#include "iceb_libbuh.h"
#include <iceb_libbuh.h>
//ze
#include <unistd.h>


void admin_newuser(const char *imabazr,const char *user,GtkWidget *wpredok)
{
char strsql[1024];
class SQLCURSOR cur;
SQL_str row;
class iceb_u_str fio("");

sprintf(strsql,"select fio from %s.icebuser where login='%s'",imabazr,user);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
//  sprintf(strsql,gettext("Логин %s для базы %s уже есть!\n%s"),user,imabazr,row[0]);
//  iceb_t_soob(strsql);
  return;

 }
/***********
class VVOD VVOD1(2);

VVOD1.VVOD_SPISOK_add_MD(gettext("Введите фамилию оператора"));

vvod1(&fio,64,&VVOD1,NULL,stdscr,-1,-1);
****************/
if(iceb_menu_vvod1(gettext("Введите фамилию оператора"),&fio,40,wpredok) != 0)
 return;

int nomer=0;
/*Узнаём свободный номер оператора*/
for(nomer=1;;nomer++)
 {
  int voz=0;
  sprintf(strsql,"select un from %s.icebuser where un=%d",imabazr,nomer);
  if((voz=iceb_sql_readkey(strsql,wpredok)) <= 0)
   {
    if(voz < 0)
     return;
    break;
   }
 }

struct  passwd  *ktoz; /*Кто записал*/
ktoz=getpwuid(getuid());

sprintf(strsql,"insert into %s.icebuser (login,fio,un,logz,vrem) values('%s','%s',%d,'%s',%ld)",
imabazr,user,fio.ravno(),nomer,ktoz->pw_name,time(NULL));
iceb_sql_zapis(strsql,1,0,wpredok);

}
