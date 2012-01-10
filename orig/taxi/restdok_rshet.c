/*$Id: restdok_rshet.c,v 1.13 2011-02-21 07:36:21 sasa Exp $*/
/*18.08.2007	20.12.2004	Белых А.И.	resdok_rshet.c
Распечатка счета
*/
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "i_rest.h"

extern SQL_baza bd;
extern char *organ;
extern uid_t kod_operatora;

void  restdok_rshet(short dd,short md,short gd,
const char *nomdok,
GtkWidget *wpredok)
{
SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str   row;
SQL_str   row1;
int       kolstr;
char      strsql[400];
char      nomst[20];
char      kodkl[20];
char      fio[50];
char      koment[100];
char      podr[20];
iceb_u_str naimpodr;
float procent_sk=0.;
memset(nomst,'\0',sizeof(nomst));
memset(kodkl,'\0',sizeof(kodkl));
memset(fio,'\0',sizeof(fio));
memset(koment,'\0',sizeof(koment));
memset(podr,'\0',sizeof(podr));


//читаем шапку

sprintf(strsql,"select nomst,kodkl,fio,koment,podr,ps from Restdok where datd='%d-%d-%d' and \
nomd='%s'",gd,md,dd,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  strncpy(nomst,row[0],sizeof(nomst)-1);  
  strncpy(kodkl,row[1],sizeof(kodkl)-1);  
  strncpy(fio,row[2],sizeof(fio)-1);  
  strncpy(koment,row[3],sizeof(koment)-1);  
  strncpy(podr,row[4],sizeof(podr)-1);  
//  procent_sk=atof(row[5]);
 }

naimpodr.new_plus("");

sprintf(strsql,"select naik from Restpod where kod=%s",podr);
if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
 naimpodr.new_plus(row[0]);
 
sprintf(strsql,"select skl,mz,kodz,ei,kolih,cena,koment,ps from Restdok1 where datd='%d-%d-%d' and \
nomd='%s' order by skl asc",gd,md,dd,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

FILE *ff;
char  imafil[40];
FILE *ffuz;
char  imafiluz[40];
struct tm *bf;
time_t     vrem;

time(&vrem);
bf=localtime(&vrem);

sprintf(imafil,"shet%d.lst",getpid());

if((ff = fopen(imafil,"w")) == NULL)
 {
  iceb_er_op_fil(imafil,"",errno,wpredok);
  return;

 }
iceb_u_startfil(ff);

fprintf(ff,"\x1b\x6C%c",10); /*Установка левого поля*/
fprintf(ff,"\x1B\x4D"); /*12-знаков*/

fprintf(ff,"%s\n\n",organ);
fprintf(ff,"%s:%s ",gettext("Счет"),nomdok);
fprintf(ff,"%s %d.%d.%d%s\n",gettext("от"),dd,md,gd,gettext("г."));

fprintf(ff,"%s: %s %s\n",gettext("Подразделение"),podr,naimpodr.ravno());

if(nomst[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Номер столика"),nomst);
if(kodkl[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код клиента"),kodkl);
if(fio[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Фамилия клиента"),fio);
if(koment[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Коментарий"),koment);
//if(procent_sk != 0.)
// fprintf(ff,"%s:%.6g\n",gettext("Процент скидки"),procent_sk);


fprintf(ff,"%s %d.%d.%d%s  %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff,"\
-------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Склад|Код |       Наименование           |Е/и|Количество|   Цена   |  Сумма   |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------\n");

sprintf(imafiluz,"shetu%d.lst",getpid());

if((ffuz = fopen(imafiluz,"w")) == NULL)
 {
  iceb_er_op_fil(imafiluz,"",errno,wpredok);
  return;

 }
iceb_u_startfil(ffuz);


fprintf(ffuz,"%s\n\n",organ);
fprintf(ffuz,"%s:%s",gettext("Счет"),nomdok);
fprintf(ffuz," %s %d.%d.%d%s\n",gettext("от"),dd,md,gd,gettext("г."));

fprintf(ffuz,"%s:%s %s\n",gettext("Подразделение"),podr,naimpodr.ravno());

if(nomst[0] != '\0')
 fprintf(ffuz,"%s:%s\n",gettext("Номер столика"),nomst);
if(kodkl[0] != '\0')
 fprintf(ffuz,"%s:%s\n",gettext("Код клиента"),kodkl);
if(fio[0] != '\0')
 fprintf(ffuz,"%s:%s\n",gettext("Клиент"),fio);
if(koment[0] != '\0')
 fprintf(ffuz,"%s:%s\n",gettext("Коментарий"),koment);
//if(procent_sk != 0.)
// fprintf(ffuz,"%s:%.6g\n",gettext("Процент скидки"),procent_sk);


fprintf(ffuz,"%s %d.%d.%d%s %s:%02d:%02d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ffuz,"\
---------------------------------------------\n");

fprintf(ffuz,"%s\n",gettext("\
 Код |Наименование     |Кол.| Цена | Сумма  |"));
/*
12345 12345678901234567 1234 123456 12345678
*/
fprintf(ffuz,"\
---------------------------------------------\n");



double kolih,cena,suma;
double cena_sk;
char   naim[50];
//double itogo=0.;
double suma_dok=0.;
while(cur.read_cursor(&row) != 0)
 {
  memset(naim,'\0',sizeof(naim));
  //узнаём наименование
  if(atoi(row[1]) == 0) //материал
   {
    
    sprintf(strsql,"select naimat from Material where kodm=%s",
    row[2]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      strncpy(naim,row1[0],sizeof(naim)-1);            
   }
  else //услуга
   {
    
    sprintf(strsql,"select naius from Uslugi where kodus=%s",
    row[2]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      strncpy(naim,row1[0],sizeof(naim)-1);            
            
   }
  
  kolih=atof(row[4]);
  cena=atof(row[5]);
  procent_sk=atof(row[7]);
  cena_sk=cena-cena*procent_sk/100.;
  cena_sk=iceb_u_okrug(cena_sk,0.01);
  
  suma=kolih*cena_sk;
  suma_dok+=suma;

  fprintf(ffuz,"%-5s %-17.17s %4.4g %6.2f %8.2f\n",row[2],naim,kolih,cena_sk,suma);  

  if(strlen(naim) > 17)
   fprintf(ffuz,"%5s %s\n","",&naim[17]);

     
  fprintf(ff,"%-5s %-4s %-30.30s %-3s %10.10g %10.2f %10.2f\n",
  row[0],row[2],naim,row[3],kolih,cena_sk,suma);  

  if(strlen(naim) > 30)
    fprintf(ff,"%-5s %-4s %s\n"," "," ",&naim[30]);
  

  if(row[6][0] != '\0')
    fprintf(ff,"%-5s %-4s %s\n"," "," ",row[6]);
  
 }
fprintf(ff,"\
-------------------------------------------------------------------------------\n");

fprintf(ffuz,"\
---------------------------------------------\n");


fprintf(ffuz,"%35s:%8.2f\n",gettext("К оплате"),suma_dok);

fprintf(ff,"%67s:%10.2f\n",gettext("К оплате"),suma_dok);

fprintf(ff,"\x12");  /*Нормальный режим печати*/
fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/
//iceb_podpis(kod_operatora,ff);
iceb_podpis(kod_operatora,ff,wpredok);
iceb_podpis(ffuz,wpredok);
 
fclose(ffuz);
fclose(ff);

iceb_u_spisok fil;
iceb_u_spisok nazv;

fil.plus(imafiluz);
nazv.plus(gettext("Счет на оплату (Узкая форма)"));
fil.plus(imafil);
nazv.plus(gettext("Счет на оплату"));

//printf("rasklient-fine\n");

iceb_rabfil(&fil,&nazv,"",0,wpredok);
//iceb_print(imafil);
//unlink(imafil);

}
