/* $Id: zaptabelw.c,v 1.6 2011-02-21 07:35:59 sasa Exp $ */
/*08.09.2006	15.05.2001	Белых А.И.	zaptabelw.c
Запись в таблицу строки с табелем
*/
#include        <pwd.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>

extern SQL_baza bd;

int zaptabelw(int mz, //0-новая запись 1-корректировка
short mp,short gp,
int tabn,
int tz,
float dnei,
float has,
float kdnei,
short dn,short mn,short gn,
short dk,short mk,short gk,
float kolrd,
float kolrh,
const char *koment, //Коментарий
int nomz, //Номер записи
int snomz, //Старый номер записи
GtkWidget *wpredok)
{
time_t vrem;
char	strsql[400];


time(&vrem);


dnei=iceb_u_okrug(dnei,0.01);
has=iceb_u_okrug(has,0.01);
kdnei=iceb_u_okrug(kdnei,0.01);
kolrd=iceb_u_okrug(kolrd,0.01);
kolrh=iceb_u_okrug(kolrh,0.01);

if(mz == 0)
 {
  sprintf(strsql,"insert into Ztab \
  values (%d,%d,%d,%d,%f,%f,%f,'%04d-%02d-%02d','%04d-%02d-%02d',%d,%ld,\
  %.2f,%.2f,'%s',%d)",
  gp,mp,tabn,tz,dnei,has,kdnei,gn,mn,dn,
  gk,mk,dk,iceb_getuid(wpredok),vrem,kolrd,kolrh,koment,nomz);

 }
if(mz == 1)
 {
  sprintf(strsql,"update Ztab \
set \
dnei=%f,\
has=%f,\
kdnei=%f,\
datn='%04d-%02d-%02d',\
datk='%04d-%02d-%02d',\
ktoz=%d,\
vrem=%ld, \
kolrd=%.2f, \
kolrh=%.2f,\
kom='%s',\
nomz=%d \
where god=%d and mes=%d and tabn=%d and kodt=%d and nomz=%d",
  dnei,has,kdnei,gn,mn,dn,gk,mk,dk,
  iceb_getuid(wpredok),vrem,kolrd,kolrh,koment,nomz,
  gp,mp,tabn,tz,snomz);
 }
     
//printw("\nmz=%d strsql=%s\n",mz,strsql);
//OSTANOV();
        
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY)
   {
    class iceb_u_str repl;

    sprintf(strsql,"zaptabelw %s ",gettext("Такая запись уже есть !"));
    repl.plus(strsql);

    sprintf(strsql,"%s:%d",gettext("Табельный номер"),tabn);
    repl.ps_plus(strsql);

    sprintf(strsql,"%s:%d",gettext("Код табеля"),tz);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
   }
  else
    iceb_msql_error(&bd,"zartabelw-Ошибка ввода записи табеля !",strsql,wpredok);
  return(1);
 }
return(0);
}
