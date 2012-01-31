/*$Id: pdokukrw.c,v 1.5 2011-02-21 07:35:55 sasa Exp $*/
/*22.01.2010	21.01.2010	Белых А.И.	pdokukr.c
Просмотр документа из подсистемы "Учёт командировочных расходов"
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

int pdokukr(const char *datadok,const char *nomdok,GtkWidget *wpredok)
{
short dd,md,gd;
char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
FILE *ff;
char imaf[56];
class iceb_u_str naim("");

iceb_u_rsdat(&dd,&md,&gd,datadok,1);

sprintf(strsql,"select * from Ukrdok \
where god=%d and nomd='%s'",gd,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s\n%d.%d.%d %s",gettext("Не найден документ!"),
  dd,md,gd,nomdok);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

sprintf(imaf,"pdokukr%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }

fprintf(ff,"%s\n",gettext("Учёт расходов на командировки"));

fprintf(ff,"%s:%d.%d.%d %s:%s\n",
gettext("Дата документа"),dd,md,gd,
gettext("Номер документа"),nomdok);

naim.new_plus("");
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),row[0],naim.ravno());

naim.new_plus("");
sprintf(strsql,"select naik from Ukrvkr where kod='%s'",row[13]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Код операции"),row[13],naim.ravno());


fprintf(ff,"%s:%s\n",gettext("Записал"),iceb_kszap(row[16],1,wpredok));
fprintf(ff,"%s:%s\n",gettext("Дата записи"),iceb_u_vremzap(row[17]));
fprintf(ff,"%s:%s\n",gettext("Пунк назначения"),row[4]);
fprintf(ff,"%s:%s\n",gettext("Организация"),row[5]);
fprintf(ff,"%s:%s\n",gettext("Цель командировки"),row[6]);
fprintf(ff,"%s:%s\n",gettext("Дата начала командировки"),iceb_u_datzap(row[7]));
fprintf(ff,"%s:%s\n",gettext("Дата конца командировки"),iceb_u_datzap(row[8]));
fprintf(ff,"%s:%s\n",gettext("Номер приказа"),row[9]);
fprintf(ff,"%s:%s\n",gettext("Дата приказа"),iceb_u_datzap(row[10]));
fprintf(ff,"%s:%s\n",gettext("Номер авансового отчёта"),row[14]);
fprintf(ff,"%s:%s\n",gettext("Дата авансового отчёта"),iceb_u_datzap(row[15]));



sprintf(strsql,"select * from Ukrdok1 where datd='%04d-%02d-%02d' and nomd='%s'",gd,md,dd,nomdok);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Код  |Наименование таваров и услуг  | Счёт |Единиц|Сумма без |Сумма НДС | Сумма с  |Контр.|Ном.док.|Дата док.|\n\
      |                              |      |измере|  НДС     |          |  НДС     |Контр.|Ном.док.|Дата док.|\n");
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------\n");
double suma=0.;
double isuma_bez_nds=0.;
double isuma_nds=0.;
double isuma_s_nds=0.;
while(cur.read_cursor(&row) != 0)
 {
  naim.new_plus("");
  sprintf(strsql,"select naim from Ukrkras where kod=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   naim.new_plus(row1[0]);

  suma=atof(row[5])+atof(row[7]);
  isuma_bez_nds+=atof(row[5]);
  isuma_nds+=atof(row[7]);
  isuma_s_nds+=suma;
  
  fprintf(ff,"%6s %-*.*s %-*s %-*s %10.2f %10.2f %10.2f %-*s %-*s %s %s %s\n",
  row[2],
  iceb_u_kolbait(30,naim.ravno()),
  iceb_u_kolbait(30,naim.ravno()),
  naim.ravno(),
  iceb_u_kolbait(6,row[3]),row[3],
  iceb_u_kolbait(6,row[6]),row[6],
  atof(row[5]),
  atof(row[7]),
  suma,
  iceb_u_kolbait(6,row[12]),row[12],
  iceb_u_kolbait(8,row[13]),row[13],
  iceb_u_sqldata(row[15]),
  iceb_u_vremzap(row[9]),
  iceb_kszap(row[8],1,wpredok));
  
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f %10.2f\n",
iceb_u_kolbait(51,gettext("Итого")),
gettext("Итого"),
isuma_bez_nds,
isuma_nds,
isuma_s_nds);


iceb_podpis(ff,wpredok);
fclose(ff);

iceb_prosf(imaf,wpredok);

sleep(1);

unlink(imaf);

return(0);
}

