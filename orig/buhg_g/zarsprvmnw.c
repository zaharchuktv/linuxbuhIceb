/*$Id: zarsprvmnw.c,v 1.9 2011-02-21 07:36:00 sasa Exp $*/
/*20.11.2009	08.04.2002	Белых А.И.	zarsprvmnw.c
Получение справки о доходах по датам в счет которых были 
сделаны начисления
*/
#include <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>
#include "zar_sprav.h"

void             rekvizkon(SQL_str,FILE*);

extern char     *organ;/*Наименование организации*/
extern SQL_baza bd;

void zarsprvmnw(class zar_sprav_rek *data,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
char		dolg[512];
char            fam[512];
char		strsql[512];
SQL_str		row;
SQLCURSOR curr;

/****************Расчет*********************/


int		kolstr=0;
char		imaf[32];
FILE		*ff;
char		naipodr[512];
char 		podr[32];
char		harac[512];
short           sovm=0; /*0-нет 1- Совместитель*/
char		inn[32];
short		mes,god;
short           mesz=0,godz=0;

sprintf(strsql,"select fio,sovm,inn,dolg,harac,podr from Kartb where tabn=%d",data->tabnom.ravno_atoi());
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %d !", gettext("Не найден табельный номер"),data->tabnom.ravno_atoi());
  iceb_menu_soob(strsql,wpredok);
  return;
 }

strncpy(podr,row[5],sizeof(podr)-1);
strncpy(fam,row[0],sizeof(fam)-1);
strncpy(dolg,row[3],sizeof(dolg)-1);
memset(harac,'\0',sizeof(harac));
strncpy(harac,row[4],sizeof(harac)-1);
memset(inn,'\0',sizeof(inn));
strncpy(inn,row[2],sizeof(inn)-1);


short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->datak.ravno());
int kolmes=iceb_u_period(1,mn,gn,1,mk,gk,1);
if(kolmes == 0)
 {
  iceb_menu_soob(gettext("Количество месяцев в периоде равно нолю !"),wpredok);
  return;
 }


sprintf(strsql,"%s %d.%d %s %d.%d %s %s\n",
gettext("Период с"),mn,gn,
gettext("до"),mk,gk,data->tabnom.ravno(),data->fio.ravno());

iceb_printw(iceb_u_toutf(strsql),buffer,view);

sprintf(strsql,"%s:%d\n",gettext("Количество месяцев"),kolmes);

iceb_printw(iceb_u_toutf(strsql),buffer,view);



sprintf(strsql,"select suma,godn,mesn from Zarp where tabn=%d and \
prn='1' and suma <> 0. and \
((godn = %d && mesn >= %d) or \
(godn = %d && mesn <= %d) or \
(godn > %d && godn < %d)) \
order by godn,mesn asc",
data->tabnom.ravno_atoi(),gn,mn,gk,mk,gn,gk);

//printw("\n%s\n",strsql);
//OSTANOV();

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не введены начисления !"),wpredok);
  return;
 }

sprintf(imaf,"sprdm%d_%d.lst",data->tabnom.ravno_atoi(),getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

/*Читаем реквизиты организации*/
sprintf(strsql,"select * from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  iceb_menu_soob(gettext("Не найден контрагент с кодом 00 !"),wpredok);
 }
else
 {
  rekvizkon(row,ff);
 }
memset(naipodr,'\0',sizeof(naipodr));

sprintf(strsql,"select naik from Podr where kod=%s",podr);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
  strncpy(naipodr,row[0],sizeof(naipodr)-1);

fprintf(ff,"\n\
	      %s\n\
%s %d.%d%s %s %d.%d%s\n\
%s %s %s ",
gettext("С п р а в к а."),
gettext("о заработной плате за период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."),
gettext("Работник"),
fam,
gettext("работает"));

if(sovm == 0)
  fprintf(ff," %s.\n",gettext("постоянно"));
else
  fprintf(ff," %s.\n",gettext("по совместительству"));
fprintf(ff,"%s %s\n",gettext("Идентификационный код"),inn);
fprintf(ff,"%s: %s\n",gettext("Должность"),dolg);

short		ots=10;
fprintf(ff,"\
%*s-------------------------\n\
%*s%s\n\
%*s-------------------------\n",
ots," ",
ots," ","|  Дата |   Начислено   |",
ots," ");


godz=mesz=0;
double suma=0.;
double sumames=0.;
double sumaitog=0.;

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  suma=atof(row[0]);
  god=atoi(row[1]);
  mes=atoi(row[2]);
  
  if(mesz != mes)
   {
    if(mesz != 0)
     {
      sprintf(strsql,"%02d.%d %15.2f\n",mesz,godz,sumames);
      iceb_printw(iceb_u_toutf(strsql),buffer,view);

      fprintf(ff,"%*s|%02d.%d %15.2f|\n",ots," ",mesz,godz,sumames);
     }
    sumames=0.;
    mesz=mes;
    godz=god;
   }  
  sumames+=suma;
  sumaitog+=suma;
  
 }
sprintf(strsql,"%02d.%d %15.2f\n",mesz,godz,sumames);
iceb_printw(iceb_u_toutf(strsql),buffer,view);

sprintf(strsql,"%*s %15.2f\n",iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),sumaitog);
iceb_printw(iceb_u_toutf(strsql),buffer,view);

fprintf(ff,"%*s|%02d.%d %15.2f|\n",ots," ",mesz,godz,sumames);
memset(strsql,'\0',sizeof(strsql));
iceb_u_preobr(sumaitog,strsql,0);

fprintf(ff,"%*s-------------------------\n\
%*s %*s %15.2f\n\n\
%s\n%s\n",
ots," ",
ots," ",
iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),
sumaitog,
gettext("Общая сумма доходов:"),strsql);

fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/
iceb_podpis(ff,wpredok);
fclose(ff);

data->imaf.plus(imaf);
data->naimf.plus(gettext("Справка о доходах"));
iceb_ustpeh(imaf,1,wpredok);
}


