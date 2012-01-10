/*$Id: b_ukrpromw.c,v 1.8 2011-03-28 06:55:46 sasa Exp $*/
/*09.11.2009	27.10.2005	Белых А.И.	b_ukrpromw.c
Формирование файла для импорта платёжек в подсистему Укрпромбанка
Имя файла
INxnnnnn.kmm
 где
  IN    - тип файла
  x     - номер дня недели
  nnnnn - номер клиента
  k     - символ "k"
  mm    - номер сеанса

Структура записи

char MfoD[9]         МФО плательщика
char AccD[14]        Лицевой счёт плательшика
char OkpoD[10]       ОКПО плательщика
char MfoK[9]         МФО получателя
char AccK[14]        Лицевой счёт получателя
char OkpoK[10]       ОКПО получателя
char DocN[10]        Номер документа
char Amt[19]         Сумма (в коп.) платежа
char Opr[2]          01-платёжное поручение
char NamD[38]        Наименование плательщика
char NamK[38]        Наименование получателя
char NameP[160]      Назначение платежа
char day[2]          Ден
char day[2]          Месяц
char year[2]         Год
char sym[1]
char rezerv[40]
char eol[2]=0x0a0d   Признак конца строки




*/


#include <stdlib.h>
#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern class REC rec;

int b_ukrpromw(const char *tabl,GtkWidget *wpredok)
{
char		imaf[54],imaf1[54];
FILE		*ff,*ff1;
char		stt[1024];
class iceb_u_str koment("");
char		strsql[512];
short		d,m,g;
char		nomerdok[32];

sprintf(imaf,"platp.txt");
if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }

sprintf(imaf1,"plat.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return(1);
 }
int suma_int=0;
double suma_double=0.;
double bb=0.;
while(fgets(stt,sizeof(stt),ff) != NULL)
 {
  iceb_u_polen(stt,strsql,sizeof(strsql),1,'|');
  iceb_u_rsdat(&d,&m,&g,strsql,1);
  iceb_u_polen(stt,nomerdok,sizeof(nomerdok),2,'|');

  if(readpdokw(tabl,g,nomerdok,wpredok) != 0)
     continue;

  //Округляем, чтобы не пропадали копейки
  rec.sumd=iceb_u_okrug(rec.sumd,0.01);

  bb=rec.sumd*100.;
 
  modf(bb,&suma_double);
  suma_int=(int)suma_double;


  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);
  
  fprintf(ff1,"%9.9s%14.14s%10.10s%9.9s%14.14s%10.10s%-10.10s%19d\
%2.2s%-*.*s%-*.*s%-*.*s%02d%02d%02d%1.1s%-40.40s\n",
  rec.mfo.ravno(),
  rec.nsh.ravno(),
  rec.kod.ravno(),
  rec.mfo1.ravno(),
  rec.nsh1.ravno(),
  rec.kod1.ravno(),
  nomerdok, 
  suma_int,
  "01",
  iceb_u_kolbait(38,rec.naior.ravno()),iceb_u_kolbait(38,rec.naior.ravno()),rec.naior.ravno(),
  iceb_u_kolbait(38,rec.naior1.ravno()),iceb_u_kolbait(38,rec.naior1.ravno()),rec.naior1.ravno(),
  iceb_u_kolbait(160,koment.ravno()),iceb_u_kolbait(160,koment.ravno()),koment.ravno(),
  d,m,g-2000,
  " ",
  " ");
  
  
 }
memset(stt,'\0',sizeof(stt));
memset(stt,'0',380);
fprintf(ff1,"%s\n",stt);

fclose(ff1);
fclose(ff);
return(0);
}
