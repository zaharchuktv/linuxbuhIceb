/*$Id: sprtabotpw.c,v 1.11 2011-02-21 07:35:57 sasa Exp $*/
/*30.06.2010	02.01.2003	Белых А.И.	sprtabotpw.c
Расчет отпускных
*/
#include <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>
#include "zar_sprav.h"

double 	itogvert(int,int,int,double*);
void	rasrez(int,short,short,short,short,short,char*,char*,char*,
        char*,char*,double*,double*,int,int,class iceb_u_int*,
        class iceb_u_spisok*,class iceb_u_double*,FILE*,int,int,double*,char*);
void readki(class iceb_u_spisok*,class iceb_u_double*,GtkWidget*);
void sprtabotp_rnvvr(int tabn,short mn,short gn,short dk,short mk,short gk,const char *imaf,class iceb_u_int *skodnvr,class iceb_u_double *sum_vr,GtkWidget *wpredok);


extern char     *organ;/*Наименование организации*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern SQL_baza bd;

void sprtabotpw(class zar_sprav_rek *data,
GtkWidget *bar,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
FILE            *ff;
SQL_str		row;
char		strsql[512];
int		kolstr=0;
float kolstr1=0;
char		imaf[32],imafbu[32],imafxr[32];
SQLCURSOR cur;
short		dk=31;
char		kodnvr[1024]; //Коды не входящие в расчет отпускных
char		kodnvrind[1024]; //Коды не входящие в расчет индексации отпускных
int		kolkodnvr=0;
int		kolkodnvrind=0;
short		d,m,g;
int		nommes=0;
int		nomkod=0;
double		suma=0.;
char		fio[512];
char		kodpodr[32];
char		naipod[512];
char		dolg[512];
short		dnr,mnr,gnr;
SQLCURSOR curr;

short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->datak.ravno());

sprintf(strsql,"select fio,dolg,podr,datn from Kartb where tabn=%d",data->tabnom.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),data->tabnom.ravno_atoi());
  iceb_menu_soob(strsql,wpredok);
  return;
 }
strncpy(fio,row[0],sizeof(fio)-1);
strncpy(dolg,row[1],sizeof(dolg)-1);
strncpy(kodpodr,row[2],sizeof(kodpodr)-1);
iceb_u_rsdat(&dnr,&mnr,&gnr,row[3],2);

iceb_u_dpm(&dk,&mk,&gk,5); //получить последний день месяца

sprintf(strsql,"%d %s\n",data->tabnom.ravno_atoi(),fio);
iceb_printw(iceb_u_toutf(strsql),buffer,view);
sprintf(strsql,"%d.%d.%d => %d.%d.%d\n",1,mn,gn,dk,mk,gk);
iceb_printw(iceb_u_toutf(strsql),buffer,view);


memset(kodnvr,'\0',sizeof(kodnvr));
memset(kodnvrind,'\0',sizeof(kodnvrind));
iceb_poldan("Коды не входящие в расчет отпускных",kodnvr,"zarotp.alx",wpredok);
iceb_poldan("Коды не входящие в расчет индексации отпускных",kodnvrind,"zarotp.alx",wpredok);

kolkodnvr=iceb_u_pole2(kodnvr,',');
if(kolkodnvr == 0 && kodnvr[0] != '\0')
  kolkodnvr=1;

kolkodnvrind=iceb_u_pole2(kodnvrind,',');
if(kolkodnvrind == 0 && kodnvrind[0] != '\0')
  kolkodnvrind=1;
  
int kolmes=iceb_u_period(1,mn,gn,dk,mk,gk,1);

class iceb_u_spisok DATAI; //Список дат м.год индексации
class iceb_u_double KOF; //Коффициенты индексации по доатам
//Читаем коэффициенты индексации
readki(&DATAI,&KOF,wpredok);

double sumanmes[kolmes]; //Сумма начисленная
memset(&sumanmes,'\0',sizeof(sumanmes));
double sumanvr[kolmes*kolkodnvr]; //Суммы не входящие в расчет
memset(&sumanvr,'\0',sizeof(sumanvr));

double sumanmesxr[kolmes]; //Сумма начисленная хозрасчет
memset(&sumanmesxr,'\0',sizeof(sumanmesxr));
double sumanvrxr[kolmes*kolkodnvr]; //Суммы не входящие в расчет хозрасчет
memset(&sumanvrxr,'\0',sizeof(sumanvrxr));

double sumanmesbu[kolmes]; //Сумма начисленная бюджет
memset(&sumanmesbu,'\0',sizeof(sumanmesbu));
double sumanvrbu[kolmes*kolkodnvr]; //Суммы не входящие в расчет бюджет
memset(&sumanvrbu,'\0',sizeof(sumanvrbu));


double sumanvi[kolmes*kolkodnvrind]; //Сумма не входящая в расчет индексации отпускных
memset(sumanvi,'\0',sizeof(sumanvi));

double sumanvixr[kolmes*kolkodnvrind]; //Сумма не входящая в расчет индексации отпускных хозрасчета
memset(sumanvixr,'\0',sizeof(sumanvixr));

double sumanvibu[kolmes*kolkodnvrind]; //Сумма не входящая в расчет индексации отпускных бюджета
memset(sumanvibu,'\0',sizeof(sumanvibu));

class iceb_u_int skodnvr; /*список кодов начислений вошедшх в расчёт*/

class iceb_u_int kolkd; //Количество календарных дней в месяце
kolkd.make_class(kolmes);

sprintf(strsql,"select datz,knah,suma,shet from Zarp where tabn=%d \
and datz >= '%04d-%d-%d' and datz <='%04d-%d-%d' and prn='1'",
data->tabnom.ravno_atoi(),gn,mn,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(dnr != 0 && dnr != 1) //Если первый месяц отработан не полностью в расчет его не берем
    if(mnr == m && gnr == g)
      continue;

  suma=atof(row[2]);

  nommes=iceb_u_period(1,mn,gn,d,m,g,1)-1;    

  sumanmes[nommes]+=suma;

  if(iceb_u_proverka(shetb,row[3],0,1) == 0)
    sumanmesbu[nommes]+=suma;
  else 
    sumanmesxr[nommes]+=suma;

  if(iceb_u_proverka(kodnvr,row[1],0,1) == 0)
   {
    iceb_u_pole1(kodnvr,row[1],',',0,&nomkod);
    sumanvr[nommes*kolkodnvr+nomkod]+=suma;
    if(iceb_u_proverka(shetb,row[3],0,1) == 0)
      sumanvrbu[nommes*kolkodnvr+nomkod]+=suma;
    else 
      sumanvrxr[nommes*kolkodnvr+nomkod]+=suma;

   }
  else
   {
    if(skodnvr.find(row[1]) < 0)
     skodnvr.plus(row[1]);
   }

  if(iceb_u_proverka(kodnvrind,row[1],0,1) == 0)
   {
    iceb_u_pole1(kodnvrind,row[1],',',0,&nomkod);
    sumanvi[nommes*kolkodnvrind+nomkod]+=suma;
    if(iceb_u_proverka(shetb,row[3],0,1) == 0)
      sumanvibu[nommes*kolkodnvrind+nomkod]+=suma;
    else 
      sumanvixr[nommes*kolkodnvrind+nomkod]+=suma;

   }
 }

/*Создаём масив для сумм начислений вошедших в расчёт*/
class iceb_u_double sum_vr;
int kolih_kod=skodnvr.kolih();
sum_vr.make_class(kolih_kod*kolmes);
kolstr1=0;
cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(dnr != 0 && dnr != 1) //Если первый месяц отработан не полностью в расчет его не берем
    if(mnr == m && gnr == g)
      continue;

  suma=atof(row[2]);

  nommes=iceb_u_period(1,mn,gn,d,m,g,1)-1;    
  if((nomkod=skodnvr.find(row[1])) < 0)
   continue;
  sum_vr.plus(suma,nommes*kolih_kod+nomkod);
 }

//Узнаем количество календарных дней
if(gn != gk)
  sprintf(strsql,"select god,mes,kdnei from Ztab where tabn=%d and \
((god > %d and god < %d) \
or (god=%d and mes >= %d) \
or (god=%d and mes <= %d))",
  data->tabnom.ravno_atoi(),gn,gk,gn,mn,gk,mk);
else
  sprintf(strsql,"select god,mes,kdnei from Ztab where tabn=%d and \
god = %d and mes >= %d and mes <= %d",
  data->tabnom.ravno_atoi(),gn,mn,mk);

//printf("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

d=1;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s\n",row[0],row[1],row[2]);
  
  g=atoi(row[0]);
  m=atoi(row[1]);
  nommes=iceb_u_period(1,mn,gn,d,m,g,1)-1;    
  kolkd.plus(atoi(row[2]),nommes);
//  printw("kolkd[%d]=%d\n",nommes,kolkd[nommes]);

 }

//Читаем наименование подразделения
memset(naipod,'\0',sizeof(naipod));
sprintf(strsql,"select naik from Podr where kod=%s",kodpodr);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 strncpy(naipod,row[0],sizeof(naipod)-1);
 
//Распечатываем общий итог

sprintf(imaf,"otp%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }


fprintf(ff,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

rasrez(data->tabnom.ravno_atoi(),mn,gn,dk,mk,gk,kodnvr,fio,dolg,kodpodr,naipod,sumanmes,
sumanvr,kolmes,kolkodnvr,&kolkd,&DATAI,&KOF,ff,0,kolkodnvrind,sumanvi,kodnvrind);

iceb_podpis(ff,wpredok);
fclose(ff);

char imaf_sn[56];
sprintf(imaf_sn,"otpn%d.lst",getpid());

sprtabotp_rnvvr(data->tabnom.ravno_atoi(),mn,gn,dk,mk,gk,imaf_sn,&skodnvr,&sum_vr,wpredok);

data->imaf.plus(imaf);
data->naimf.plus(gettext("Расчетный лист на оплату отпускных"));

data->imaf.plus(imaf_sn);
data->naimf.plus(gettext("Начисления взятые в расчёт отпускных"));

if(shetb != NULL)
 {
  sprintf(imafbu,"otpbu%d.lst",getpid());

  if((ff = fopen(imafbu,"w")) == NULL)
   {
    iceb_er_op_fil(imafbu,"",errno,wpredok);
    return;
   }


  rasrez(data->tabnom.ravno_atoi(),mn,gn,dk,mk,gk,kodnvr,fio,dolg,kodpodr,naipod,sumanmesbu,
  sumanvrbu,kolmes,kolkodnvr,&kolkd,&DATAI,&KOF,ff,1,kolkodnvrind,sumanvibu,kodnvrind);

  iceb_podpis(ff,wpredok);
  fclose(ff);


  sprintf(imafxr,"otpxr%d.lst",getpid());

  if((ff = fopen(imafxr,"w")) == NULL)
   {
    iceb_er_op_fil(imafxr,"",errno,wpredok);
    return;
   }


  rasrez(data->tabnom.ravno_atoi(),mn,gn,dk,mk,gk,kodnvr,fio,dolg,kodpodr,naipod,
  sumanmesxr,sumanvrxr,kolmes,kolkodnvr,&kolkd,&DATAI,&KOF,ff,2,kolkodnvrind,sumanvixr,kodnvrind);

  iceb_podpis(ff,wpredok);

  fclose(ff);

  data->imaf.plus(imafbu);
  data->naimf.plus(gettext("Расчетный лист на оплату отпускных (бюджет)"));

  data->imaf.plus(imafxr);
  data->naimf.plus(gettext("Расчетный лист на оплату отпускных (хозрасчёт)"));

 }

for(int nom=0; nom < data->imaf.kolih(); nom++)
 iceb_ustpeh(data->imaf.ravno(nom),3,wpredok);
}

/********************************/
/* Распечатка результата        */
/********************************/
void	rasrez(int tabn,short mn,short gn,
short dk,short mk,short gk,
char *kodnvr,
char *fio,
char *dolg,
char *kodpodr,
char *naipod,
double *sumanmes,
double *sumanvr,
int kolmes,int kolkodnvr,
class iceb_u_int *kolkd,
class iceb_u_spisok *DATAI,
class iceb_u_double *KOF,
FILE *ff,
int metka, //0-общий 1-бюджет 2-хозрасчет
int kolkodnvrind, //Количество кодов не входящих в расчет индексации
double *sumanvi, //Массив сумм не входящих в расчет индексации
char *kodnvrind) //Коды не входящие в расчет индексации
{
struct  tm      *bf;
int nomkod=0;
int nommes=0;
char strsql[512];
short d,m,g;
SQL_str row;
int kolkdi=0;
char bros[512];
double kof=0.;
int  nomer=0;
double  sumaindekc=0.;
SQLCURSOR curr;
//Вычисляем количество колонок
int kolkol=0;
for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
  if(itogvert(kolkodnvr,kolmes,nomkod,sumanvr) != 0.)
   kolkol++;

for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
  if(itogvert(kolkodnvrind,kolmes,nomkod,sumanvi) != 0.)
   kolkol++;

char stroka[41+(kolkol+1)*11+1];
memset(stroka,'\0',sizeof(stroka));
memset(stroka,'-',sizeof(stroka)-1);


time_t  tmm;
time(&tmm);
bf=localtime(&tmm);
iceb_u_zagolov(gettext("Расчетный лист на оплату отпускных"),1,mn,gn,dk,mk,gk,organ,ff);

fprintf(ff,"\n%s:%d\n",gettext("Табельный номер"),tabn);
fprintf(ff,"%s:%s\n",gettext("Фамилия Имя Отчество"),fio);
fprintf(ff,"%s:%s\n",gettext("Должность"),dolg);
fprintf(ff,"%s:%s %s\n",gettext("Подразделение"),kodpodr,naipod);
fprintf(ff,"%-*s____________________\n",iceb_u_kolbait(15,gettext("Вид отпуска")),gettext("Вид отпуска"));
fprintf(ff,"%-*s____________________\n",iceb_u_kolbait(15,gettext("Приказ")),gettext("Приказ"));
fprintf(ff,"%-*s____________________\n",iceb_u_kolbait(15,gettext("Период з")),gettext("Период з"));
fprintf(ff,"\n");

if(metka == 1)
  fprintf(ff,"%s\n",gettext("Бюджет"));

if(metka == 2)
  fprintf(ff,"%s\n",gettext("Хозрасчет"));

if(kolkodnvr != 0)
 {
  char kod[32];
  char naikod[80];
  fprintf(ff,"%s:\n",gettext("Коды, которые не вошли в расчет"));
  short shethik=0;
  char str_prom[500];
  char str2[100];
  
  for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
   {
    if(iceb_u_polen(kodnvr,kod,sizeof(kod),nomkod+1,',') != 0)
      strcpy(kod,kodnvr);
    sprintf(strsql,"select naik from Nash where kod=%s",kod);
    memset(naikod,'\0',sizeof(naikod));
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     strncpy(naikod,row[0],sizeof(naikod)-1);    
  
    if(shethik == 0)
     {
      memset(str_prom,'\0',sizeof(str_prom));
      sprintf(str_prom,"%2s %-*.*s",kod,iceb_u_kolbait(45,naikod),iceb_u_kolbait(45,naikod),naikod);
     }

    if(shethik == 1)
     {
      sprintf(str2,"  %2s %-*.*s",kod,iceb_u_kolbait(45,naikod),iceb_u_kolbait(45,naikod),naikod);
      strcat(str_prom,str2);
     }


    
    shethik++;
    if(shethik == 2)
     {
      fprintf(ff,"%s\n",str_prom);
      shethik=0;
     }
  
  
   }
  if(shethik != 0)
      fprintf(ff,"%s\n",str_prom);

 }

if(kolkodnvrind != 0)
 {
  char kod[32];
  char naikod[80];
  fprintf(ff,"%s:\n",gettext("Коды, которые не вошли в расчет индексации"));
  for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
   {
    if(iceb_u_polen(kodnvrind,kod,sizeof(kod),nomkod+1,',') != 0)
      strcpy(kod,kodnvrind);
    sprintf(strsql,"select naik from Nash where kod=%s",kod);
    memset(naikod,'\0',sizeof(naikod));
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     strncpy(naikod,row[0],sizeof(naikod)-1);    
    fprintf(ff,"%s %s\n",kod,naikod);
   }

 }

fprintf(ff,"\n");

fprintf(ff,"%s\n",stroka);

fprintf(ff,gettext(" Дата  |Дни|Начислено |"));
//Печатаем коды не входящие в расчет
for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
 {
  if(itogvert(kolkodnvr,kolmes,nomkod,sumanvr) == 0.)
    continue;
    
  if(iceb_u_polen(kodnvr,strsql,sizeof(strsql),nomkod+1,',') != 0)
    strcpy(strsql,kodnvr);
  fprintf(ff,"%10s|",strsql);
 }

fprintf(ff,"%*.*s|",
iceb_u_kolbait(10,gettext("В расчет")),iceb_u_kolbait(10,gettext("В расчет")),gettext("В расчет"));

//Печатаем коды не входящие в расчет индексации
for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
 {
  if(itogvert(kolkodnvrind,kolmes,nomkod,sumanvi) == 0.)
    continue;
    
  if(iceb_u_polen(kodnvrind,strsql,sizeof(strsql),nomkod+1,',') != 0)
    strcpy(strsql,kodnvrind);
  fprintf(ff,"%10s|",strsql);
 }


fprintf(ff,"%-*.*s|%-*.*s|\n",
iceb_u_kolbait(6,gettext("Коэф.")),iceb_u_kolbait(6,gettext("Коэф.")),gettext("Коэф."),
iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",stroka);

d=1; m=mn; g=gn;
double itog=0.;
double itog1=0.;
double itogind=0.;

for(nommes=0; nommes < kolmes; nommes++)
 {
  fprintf(ff,"%02d.%04d|%3d|%10.2f",m,g,kolkd->ravno(nommes),sumanmes[nommes]);
  kolkdi+=kolkd->ravno(nommes);
  itog1+=sumanmes[nommes];
  itog=0.;
  for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
   {
    if(itogvert(kolkodnvr,kolmes,nomkod,sumanvr) == 0.)
      continue;
    fprintf(ff," %10.2f",sumanvr[nommes*kolkodnvr+nomkod]);
    itog+=sumanvr[nommes*kolkodnvr+nomkod];
   }

  fprintf(ff," %10.2f",sumanmes[nommes]-itog);

  itogind=0.;
  for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
   {
    if(itogvert(kolkodnvrind,kolmes,nomkod,sumanvi) == 0.)
      continue;
    fprintf(ff," %10.2f",sumanvi[nommes*kolkodnvrind+nomkod]);
    itogind+=sumanvi[nommes*kolkodnvrind+nomkod];
   }

  sprintf(bros,"%d.%d",m,g);
  if((nomer=DATAI->find(bros)) >= 0)
   {
    kof=KOF->ravno(nomer);
    fprintf(ff," %6.6g %10.2f",kof,kof*(sumanmes[nommes]-itog-itogind));
    sumaindekc+=kof*(sumanmes[nommes]-itog-itogind);
   }  
  fprintf(ff,"\n");
  
  iceb_u_dpm(&d,&m,&g,3);
 }
fprintf(ff,"%s\n",stroka);

//Печатаем итоговую строку
fprintf(ff,"%*s:%3d %10.2f",iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),kolkdi,itog1);
for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
 {
  if(itogvert(kolkodnvr,kolmes,nomkod,sumanvr) == 0.)
    continue;
  itog=0.;
  for(nommes=0; nommes < kolmes; nommes++)
    itog+=sumanvr[nommes*kolkodnvr+nomkod];
  fprintf(ff," %10.2f",itog);
  itog1-=itog;
 }
fprintf(ff," %10.2f",itog1);
itogind=0.;
for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
 {
  if(itogvert(kolkodnvrind,kolmes,nomkod,sumanvi) == 0.)
    continue;
  itog=0.;
  for(nommes=0; nommes < kolmes; nommes++)
    itog+=sumanvi[nommes*kolkodnvrind+nomkod];
  fprintf(ff," %10.2f",itog);
  itogind+=itog;
 }

if(sumaindekc != 0.)
 fprintf(ff," %6s %10.2f"," ",sumaindekc);
 
fprintf(ff,"\n");
if(itogind != 0.)
 fprintf(ff,"\n%.2f+%.2f=%.2f\n",itogind,sumaindekc,itogind+sumaindekc);
 
fprintf(ff,"\n-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");

}

/**********************************/
/*Вычисление итога по вертикали*/
/********************************/

double 	itogvert(int kolkodnvr,int kolmes,int nomkod,double *sumanvr)
{

double itog=0.;
for(int nommes=0; nommes < kolmes; nommes++)
  itog+=sumanvr[nommes*kolkodnvr+nomkod];
return(itog);

}
/*********************************/
/*Чтение коєффициентов индексации*/
/*********************************/
void readki(class iceb_u_spisok *DATAI,class iceb_u_double *KOF,GtkWidget *wpredok)
{
char  bros[512];
char  bros1[512];
short m,g;
int   nomer;
double kof=0.;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='zarotp.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarotp.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') != 0)
    continue;
  if(iceb_u_polen(bros,bros1,sizeof(bros1),1,'.') != 0)
    continue;
  m=atoi(bros);

  if(iceb_u_polen(bros,bros1,sizeof(bros1),2,'.') != 0)
    continue;
  g=atoi(bros1);  

  if(iceb_u_polen(row_alx[0],bros1,sizeof(bros1),2,'|') != 0)
    continue;
  kof=atof(bros1);  

  sprintf(bros,"%d.%d",m,g);
  if((nomer=DATAI->find(bros)) < 0)
    DATAI->plus(bros); 

  KOF->plus(kof,nomer);

 }

}
/******************************************/
/*Распечатка начислений вошедших в расчёт*/
/******************************************/
void sprtabotp_rnvvr(int tabn,short mn,short gn,
short dk,short mk,short gk,
const char *imaf,
class iceb_u_int *skodnvr,
class iceb_u_double *sum_vr,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naik;
char strsql[512];
FILE *ff;
double itogo_gor=0.;
double suma=0.;
int kolmes=iceb_u_period(1,mn,gn,dk,mk,gk,1);

short d=1,m=mn,g=gn;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(strsql,"",errno,wpredok);
  return;
 }

//fprintf(ff,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/
iceb_u_zagolov(gettext("Начисления взятые в расчёт отпускных"),1,mn,gn,dk,mk,gk,organ,ff);

int kolih_kod=skodnvr->kolih();
class iceb_u_double itogo_kol;

itogo_kol.make_class(kolmes);
sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
naik.new_plus("");
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naik.new_plus(row[0]);

fprintf(ff,"%d %s\n",tabn,naik.ravno());


class iceb_u_str liniq;
liniq.plus("-------------------------------------");
for(int nom=0; nom <= kolmes; nom++)
 liniq.plus("-----------");

fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff," Код |   Наименование начисления    |");
/*          12345 123456789012345678901234567890*/

for(int nom_mes=0; nom_mes < kolmes; nom_mes++)
 {
  fprintf(ff," %02d.%04d  |",m,g);
  iceb_u_dpm(&d,&m,&g,3);
 }

fprintf(ff,"%-*.*s|\n",
iceb_u_kolbait(10,gettext("Итого")),
iceb_u_kolbait(10,gettext("Итого")),
gettext("Итого"));
fprintf(ff,"%s\n",liniq.ravno());

for(int nom_kod=0; nom_kod < kolih_kod; nom_kod++)
 {
  naik.new_plus("");
  
  sprintf(strsql,"select naik from Nash where kod=%d",skodnvr->ravno(nom_kod));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naik.new_plus(row[0]);

  fprintf(ff,"%5d %-*.*s|",skodnvr->ravno(nom_kod),
  iceb_u_kolbait(30,naik.ravno()),
  iceb_u_kolbait(30,naik.ravno()),
  naik.ravno());
  itogo_gor=0.;
  for(int nom_mes=0; nom_mes < kolmes; nom_mes++)
   {
    suma=sum_vr->ravno(nom_mes*kolih_kod+nom_kod);
    if(suma != 0.)
     {
      fprintf(ff,"%10.2f|",suma);
      itogo_gor+=suma;    
      itogo_kol.plus(suma,nom_mes);
     }
    else
      fprintf(ff,"%10s|","");
     
   }
  fprintf(ff,"%10.2f|\n",itogo_gor);
  iceb_u_dpm(&d,&m,&g,3);

 }
fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*.*s|",
iceb_u_kolbait(36,gettext("Итого")),
iceb_u_kolbait(36,gettext("Итого")),
gettext("Итого"));

for(int nom_mes=0; nom_mes < kolmes; nom_mes++)
 fprintf(ff,"%10.2f|",itogo_kol.ravno(nom_mes));
fprintf(ff,"%10.2f|\n",itogo_kol.suma());


iceb_podpis(ff,wpredok);
fclose(ff);
}
