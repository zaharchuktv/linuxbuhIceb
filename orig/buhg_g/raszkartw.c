/*$Id: raszkartw.c,v 1.9 2011-08-29 07:13:44 sasa Exp $*/
/*12.08.2011	25.07.2002	Белых А.И.	raszkartw.c
Распечатать карточку на работника
*/
#include <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>

extern char	*organ;
extern SQL_baza bd;

void raszkartw(int tabn,GtkWidget *wpredok)
{
int kolstr=0;
SQL_str		row,row1;
SQLCURSOR cur,cur1;
char		strsql[512];
FILE		*ff;
char		imaf[32];
short		dn,mn,gn,dk,mk,gk;
char		fio[512];
char		dolg[512];
char		kateg[32];
char		naimkateg[512];
char		shetu[32];
char		naimshet[512];
char		inn[32];
char		adres[512];
class iceb_u_str pasp_nom; /*номер паспорта*/
class iceb_u_str pasp_dv; /*Дата выдачи паспорта*/
class iceb_u_str pasp_kv; /*Кем выдан паспорт*/
class iceb_u_str kodss(""); /*код спецстажа*/
char		telef[512];
char		zvan[32];
char		naimzvan[512];
char		podr[16];
char		naimpodr[512];
short		sovm=0;
int kodgni=0;
char naim_gni[512];
char har_rab[512];
char kod_lg[32];
char kart_shet[32];
char tip_kk[32];
short dr=0,mr=0,gr=0;
short dppz=0,mppz=0,gppz=0;
short pol=0; 

sprintf(strsql,"select * from Kartb where tabn=%d",tabn);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabn);
  iceb_menu_soob(strsql,wpredok);
  return;
 }
strncpy(fio,row[1],sizeof(fio)-1);
strncpy(dolg,row[2],sizeof(dolg)-1);
strncpy(zvan,row[3],sizeof(zvan)-1);
strncpy(podr,row[4],sizeof(podr)-1);
strncpy(kateg,row[5],sizeof(kateg)-1);
iceb_u_rsdat(&dn,&mn,&gn,row[6],2);
iceb_u_rsdat(&dk,&mk,&gk,row[7],2);
strncpy(shetu,row[8],sizeof(shetu)-1);
sovm=atoi(row[9]);
strncpy(inn,row[10],sizeof(inn)-1);
strncpy(adres,row[11],sizeof(adres)-1);

pasp_nom.plus(row[12]);
pasp_dv.plus(iceb_u_datzap(row[19]));
pasp_kv.plus(row[13]);


strncpy(telef,row[14],sizeof(telef)-1);
strncpy(har_rab,row[16],sizeof(har_rab)-1);
strncpy(kod_lg,row[17],sizeof(kod_lg)-1);
strncpy(kart_shet,row[18],sizeof(kart_shet)-1);
strncpy(tip_kk,row[20],sizeof(tip_kk)-1);
iceb_u_rsdat(&dr,&mr,&gr,row[21],2);
iceb_u_rsdat(&dppz,&mppz,&gppz,row[23],2);
pol=atoi(row[22]);

kodss.new_plus(row[25]);

memset(naimzvan,'\0',sizeof(naimzvan));
sprintf(strsql,"select naik from Zvan where kod=%s",zvan);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  strncpy(naimzvan,row[0],sizeof(naimzvan)-1);

memset(naimpodr,'\0',sizeof(naimpodr));
sprintf(strsql,"select naik from Podr where kod=%s",podr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  strncpy(naimpodr,row[0],sizeof(naimpodr)-1);

memset(naimkateg,'\0',sizeof(naimkateg));
sprintf(strsql,"select naik from Kateg where kod=%s",kateg);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  strncpy(naimkateg,row[0],sizeof(naimkateg)-1);

memset(naimshet,'\0',sizeof(naimshet));
sprintf(strsql,"select nais from Plansh where ns='%s'",shetu);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  strncpy(naimshet,row[0],sizeof(naimshet)-1);

memset(naim_gni,'\0',sizeof(naim_gni));
sprintf(strsql,"select naik from Gnali where kod=%d",kodgni);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 strncpy(naim_gni,row[0],sizeof(naim_gni)-1);

sprintf(imaf,"kart%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
   iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }


iceb_u_zagolov(gettext("Карточка работника"),0,0,0,0,0,0,organ,ff);

fprintf(ff,"\n");

if(sovm == 1)
 fprintf(ff,"%s\n",gettext("Совместитель")); 

if(pol == 0)
 fprintf(ff,"%s:%s\n",gettext("Пол"),gettext("Мужчина"));
if(pol == 1)
 fprintf(ff,"%s:%s\n",gettext("Пол"),gettext("Женщина"));

fprintf(ff,"\
%-*s %d\n\
%-*s %s\n\
%-*s %s\n\
%-*s %s\n\n\
%-*s %s\n\
%-*s %s\n\
%-*s %s\n\n\
%-*s %s\n\
%-*s %s\n\
%-*s %s %s\n\
%-*s %s %s\n\
%-*s %s %s\n\
%-*s %s\n\
%-*s %s %s\n",
iceb_u_kolbait(35,gettext("Табельный номер")),gettext("Табельный номер"),tabn,
iceb_u_kolbait(35,gettext("Индивидуальный налоговый номер")),gettext("Индивидуальный налоговый номер"),inn,
iceb_u_kolbait(35,gettext("Фамилия")),gettext("Фамилия"),fio,
iceb_u_kolbait(35,gettext("Адрес")),gettext("Адрес"),adres,
iceb_u_kolbait(35,gettext("Паспорт")),gettext("Паспорт"),pasp_nom.ravno(),
iceb_u_kolbait(35,gettext("Дата выдачи паспорта")),gettext("Дата выдачи паспорта"),pasp_dv.ravno(),
iceb_u_kolbait(35,gettext("Кем выдан паспорт")),gettext("Кем выдан паспорт"),pasp_kv.ravno(),
iceb_u_kolbait(35,gettext("Телефон")),gettext("Телефон"),telef,
iceb_u_kolbait(35,gettext("Должность")),gettext("Должность"),dolg,
iceb_u_kolbait(35,gettext("Звание")),gettext("Звание"),zvan,naimzvan,
iceb_u_kolbait(35,gettext("Подразделение")),gettext("Подразделение"),podr,naimpodr,
iceb_u_kolbait(35,gettext("Счет учета")),gettext("Счет учета"),shetu,naimshet,
iceb_u_kolbait(35,gettext("Код спецстажа")),gettext("Код спецстажа"),kodss.ravno(),
iceb_u_kolbait(35,gettext("Категория")),gettext("Категория"),kateg,naimkateg);

fprintf(ff,"%-*s %d %s\n",iceb_u_kolbait(35,gettext("Город налоговой инспекции")),gettext("Город налоговой инспекции"),kodgni,naim_gni);
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(35,gettext("Характер работы")),gettext("Характер работы"),har_rab);
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(35,gettext("Код льготы")),gettext("Код льготы"),kod_lg);
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(35,gettext("Номер карт-счёта")),gettext("Номер карт-счёта"),kart_shet);
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(35,gettext("Тип кредитной карты")),gettext("Тип кредитной карты"),tip_kk);
fprintf(ff,"%-*s %02d.%02d.%d\n",iceb_u_kolbait(35,gettext("Дата рождения")),gettext("Дата рождения"),dr,mr,gr);
fprintf(ff,"%-*s %02d.%02d.%d\n",iceb_u_kolbait(35,gettext("Дата повышения зарплаты")),gettext("Дата повышения зарплаты"),dppz,mppz,gppz);
 
fprintf(ff,"\
%-*s %02d.%02d.%d%s\n",
iceb_u_kolbait(35,gettext("Дата приема")),gettext("Дата приема"),dn,mn,gn,
gettext("г."));

if(dk != 0)
  fprintf(ff,"\
%-*s %02d.%02d.%d%s\n",
iceb_u_kolbait(35,gettext("Дата увольнения")),gettext("Дата увольнения"),dk,mk,gk,
gettext("г."));

sprintf(strsql,"select zapis from Kartb1 where tabn=%d order by nomz asc",tabn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
 {
  fprintf(ff,"\n%s\n",gettext("Дополнительная информация"));
  fprintf(ff,"---------------------------------------------------------\n");

  while(cur.read_cursor(&row) != 0)
   fprintf(ff,"%s\n",row[0]);
 }

/*Смотрим список картсчетов работника*/
sprintf(strsql,"select kb,nks from Zarkh where tn=%d",tabn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
 {
  fprintf(ff,"\n%s\n",gettext("Список картсчетов работника"));
  fprintf(ff,"---------------------------------------------------------\n");
  class iceb_u_str naim_bank;
  while(cur.read_cursor(&row) != 0)
  {
    /*узнаём наименование банка*/
    naim_bank.new_plus("");
    sprintf(strsql,"select naik from Zarsb where kod=%s",row[0]);    
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) > 0)
     naim_bank.new_plus(row1[0]);
    fprintf(ff,"%2s %-*.*s %s\n",
    row[0],
    iceb_u_kolbait(20,naim_bank.ravno()),
    iceb_u_kolbait(20,naim_bank.ravno()),
    naim_bank.ravno(),
    row[1]);
  }
 }


iceb_podpis(ff,wpredok);


fclose(ff);

class iceb_u_spisok imafs;
class iceb_u_spisok naimf;

imafs.plus(imaf);
naimf.plus(gettext("Карточка работника"));
iceb_ustpeh(imaf,3,wpredok);
iceb_rabfil(&imafs,&naimf,"",0,wpredok);

}
