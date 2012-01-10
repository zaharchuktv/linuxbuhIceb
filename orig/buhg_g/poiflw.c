/* $Id: poiflw.c,v 1.12 2011-02-21 07:35:55 sasa Exp $ */
/*29.11.2010    06.07.1995      Белых А.И.      poiflw.c
Поиск в файле заданного табельного номера
*/

#include <stdlib.h>
#include        <errno.h>
#include        "buhg_g.h"
#include <unistd.h>

int poifil(int,const char*,const char*,FILE*,GtkWidget*);

extern SQL_baza bd;

void poiflw(int tbn,GtkWidget *wpredok)
{
char		imafo[56];
FILE		*ff;

sprintf(imafo,"znast%d.lst",getpid());
if((ff = fopen(imafo,"w")) == NULL)
 {
  iceb_er_op_fil(imafo,"",errno,wpredok);
  return;
 }

fprintf(ff,"%s %d\n\
----------------------------------------------------\n",
gettext("Просмотр настройки по табельному номеру"),tbn);

poifil(tbn,"zardolhn.alx",gettext("Записан в файл окладов"),ff,wpredok);

poifil(tbn,"zaralim.alx",gettext("Записан в файл алиментов"),ff,wpredok);

poifil(tbn,"zarkredit.alx",gettext("Записан в файл кредитов"),ff,wpredok);

poifil(tbn,"zarinv.alx",gettext("Записан в файл инвалидов"),ff,wpredok);

poifil(tbn,"zardog.alx",gettext("Записан в файл работающих по договору"),ff,wpredok);

poifil(tbn,"zarproc.alx",gettext("Записан в файл автоматизированного расчета начислений"),ff,wpredok);

poifil(tbn,"zarpens.alx",gettext("Записан в файл пенсионеров"),ff,wpredok);

poifil(tbn,"zarlgot.alx",gettext("Записан в файл льгот для подоходного налога"),ff,wpredok);

poifil(tbn,"zarsppo.alx",gettext("Записан в файл персональных процентов отчисления в пенсионный фонд"),ff,wpredok);

fclose(ff);
iceb_ustpeh(imafo,3,wpredok);
iceb_prosf(imafo,wpredok);
sleep(1);
unlink(imafo);
}

/****************************************/
/*Поиск в файле заданного табельного номера*/
/****************************************/
int poifil(int tbn,const char *imaf,const char *zagl,FILE *ff,GtkWidget *wpredok)
{
short voz=0;
char strsql[512];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;


sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
 {
  return(0);
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if( tbn == atoi(row_alx[0]))
   {
    voz++;
    if(voz == 1)
     fprintf(ff,"%s\n",zagl);
    fprintf(ff,"%s",row_alx[0]);     
   }
 }
return(voz);
}
