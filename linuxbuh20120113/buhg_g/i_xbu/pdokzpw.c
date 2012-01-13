/*$Id: pdokzpw.c,v 1.5 2011-02-21 07:35:55 sasa Exp $*/
/*22.01.2010	13.01.2010	Белых А.И.	pdokzp.c
Просмотр начислений и удержаний в подсистеме "Заработная плата"
*/
#include <stdlib.h>
#include	<pwd.h>
#include	<time.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include <unistd.h>

extern SQL_baza bd;

int pdokzp(const char *datadok,const char *nomdok,GtkWidget *wpredok)
{
short dd,md,gd;
char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
FILE *ff;
char imaf[56];
class iceb_u_str tabnom("");
class iceb_u_str fio("");

if(iceb_prr(ICEB_PS_ZP,wpredok) != 0) /*Проверяем разрешено ли оператору работать с подсистемой "Зароботная плата"*/
 return(1);
/***************
struct  passwd  *ktor; //Кто работает

ktor=getpwuid(getuid());


memset(strsql,'\0',sizeof(strsql));
if(iceb_poldan("Список логинов доступ в программу которым разрешен",strsql,"zarnast.alx") == 0)
  if(iceb_u_proverka(strsql,ktor->pw_name,0,0) != 0)
   {
    sprintf(strsql,"%s %s\n%s",ktor->pw_name,ktor->pw_gecos,gettext("Вам запрещен доступ в эту подсистему !"));  
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
********************/
iceb_u_rsdat(&dd,&md,&gd,datadok,1);


iceb_u_polen(nomdok,&tabnom,2,'-');
sprintf(strsql,"select fio,dolg from Kartb where tabn=%d",tabnom.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  
  sprintf(strsql,"%s %d! %s",gettext("Не найден табельный номер"),tabnom.ravno_atoi(),nomdok);
  iceb_menu_soob(strsql,wpredok);
  return(1); 
 }

sprintf(imaf,"pdokzp%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }


fprintf(ff,"%s\n",gettext("Заработная плата"));
fprintf(ff,"%s:%s %s\n",gettext("Табельный номер"),tabnom.ravno(),row[0]);
fprintf(ff,"%s:%s\n",gettext("Должность"),row[1]);
fprintf(ff,"%s:%02d.%d\n",gettext("Дата начисления заработной платы"),md,gd);

sprintf(strsql,"select * from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d \
and suma <> 0. order by prn,knah asc",gd,md,gd,md,tabnom.ravno_atoi());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 }
fprintf(ff,"\
-------------------------------------------------------------------\n");
fprintf(ff,"\
 Код  |Наименование начисления/удерж.|  Дата    | Счёт |  Сумма   |\n");
fprintf(ff,"\
-------------------------------------------------------------------\n");
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  fio.new_plus("");  
  if(atoi(row[2]) == 1)
   sprintf(strsql,"select naik from Nash where kod=%d",atoi(row[3]));
  if(atoi(row[2]) == 2)
   sprintf(strsql,"select naik from Uder where kod=%d",atoi(row[3]));
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   fio.new_plus(row1[0]);
  fprintf(ff,"%6s %-*.*s %10s %*s %10s %s %s\n",
  row[3],
  iceb_u_kolbait(30,fio.ravno()),
  iceb_u_kolbait(30,fio.ravno()),
  fio.ravno(),
  iceb_u_datzap(row[0]),
  iceb_u_kolbait(6,row[5]),
  row[5],
  row[4],
  iceb_u_vremzap(row[11]),
  iceb_kszap(row[10],1,wpredok));
 }
fprintf(ff,"\
-------------------------------------------------------------------\n");
iceb_podpis(ff,wpredok); 
fclose(ff);

iceb_prosf(imaf,wpredok);
sleep(1); /*для того, чтобы не удалялся файл перед промотром*/
unlink(imaf);

return(0);
}
