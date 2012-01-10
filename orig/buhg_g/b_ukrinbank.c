/*$Id: b_ukrinbank.c,v 1.9 2011-03-28 06:55:46 sasa Exp $*/
/*09.11.2009	21.08.2002	Белых А.И.	b_ukrinbank.c

Структура файла экспорта информации Укринбанка

МФО Дебет             N6
Бал. счет             N14
Счет 2 порядка        S3   три пробела
Код ЕДРПОУ            S10
Найменування          S38
МФО Кредит            N6
Бал. счет             N14
Счет 2 порядка        S3   три пробела
Код ЕДРПОУ            S10
Найменування          S38
Сумма                 N18.2
ICO платежа           S3     UAH,USD...
Дата проведено банком ггггммдд
Дата пост. в банк     ггггммдд  дата передачи в банк
Код операции          N3    210-внешний платеж 110-внутри банка
Тип документа         N2    1-платежка
N документа           STRING(10)
Дата документа        ггггммдд
Назначение платежа    S160
Дебет/Кредит          S1   (0-дебет 1-кредит) у нас всегда 1

Телефон 52-06-07

*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <errno.h>
#include        <time.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern class REC rec;


int b_ukrinbank(const char *tabl,GtkWidget *wpredok)
{
char		imaf[32];
FILE		*ff,*ff1;
class iceb_u_str koment("");
char		stt[1024];
char		nomer[32];
char		strsql[512];
short		d,m,g;
char		kodop[32];
struct  tm      *bf;
time_t          tmm;

sprintf(imaf,"platp.txt");
if((ff1 = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }


/*
printw(gettext("Запись документа N%s в файл для передачи.\n"),rec.nomdk);
refresh();
*/
time(&tmm);
bf=localtime(&tmm);
short dt=bf->tm_mday;
short mt=bf->tm_mon+1;
short gt=bf->tm_year+1900;

sprintf(imaf,"plat.txt");
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }

while(fgets(stt,sizeof(stt),ff1) != NULL)
 {
  iceb_u_polen(stt,strsql,sizeof(strsql),1,'|');
  iceb_u_rsdat(&d,&m,&g,strsql,1);
  iceb_u_polen(stt,nomer,sizeof(nomer),2,'|');

  if(readpdokw(tabl,g,nomer,wpredok) != 0)
     continue;

  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);
  if(iceb_u_SRAV(rec.mfo.ravno(),rec.mfo1.ravno(),0) == 0)
    strcpy(kodop,"110");
  else
    strcpy(kodop,"210");

  fprintf(ff,"%-6s%-14s%-3s%-10s%-*s%-6s%-14s%-3s%-10s%-*s%18.2f\
%-3s%04d%02d%02d%04d%02d%02d\
%-3s%2s%-10s%04d%02d%02d%-*.*s%1s\n",
  rec.mfo.ravno(),
  rec.nsh.ravno(),
  " ",
  rec.kodor.ravno(),
  iceb_u_kolbait(38,rec.naior.ravno()),rec.naior.ravno(),
  rec.mfo1.ravno(),
  rec.nsh1.ravno(),
  " ",
  rec.kodor1.ravno(),
  iceb_u_kolbait(38,rec.naior1.ravno()),rec.naior1.ravno(),
  rec.sumd,
  "UAH",
  0,0,0,
  gt,mt,dt,
  kodop,
  "1",
  nomer,
  g,m,d,
  iceb_u_kolbait(160,koment.ravno()),iceb_u_kolbait(160,koment.ravno()),koment.ravno(),
  "1");
  

 }

fclose(ff);
fclose(ff1);
return(0);
}
