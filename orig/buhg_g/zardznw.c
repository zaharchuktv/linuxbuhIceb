/*$Id: zardznw.c,v 1.6 2011-02-21 07:36:00 sasa Exp $*/
/*22.12.2010	02.12.2009	Белых А.И.	zardrnw.c
Расчёт доплаты до средней зарплаты в командировке с учётом надбавки за награду
*/
#include <stdlib.h>
#include        "buhg_g.h"

extern short    *kodkomand; //Коды командировочных расходов
extern short *kvvrdzn; /*Коды входящие в расчёт доплаты до средней зарплаты в командировке с учётом надбавки за награду*/
extern short koddzn; /*Код доплаты до средней зарплаты в командировке с учётом надбавки за награду*/
extern short   *ktnvr_komand; //Коды табеля не входящие в расчёт командировочных
extern double  okrg; /*Округление*/
extern SQL_baza bd;

void zardznw(int tn, //Табельный номер*/
short mp,short gp,
int podr,
const char *nah_only,
FILE *ff_prot,GtkWidget *wpredok)
{
char strsql[512];
char shet[32];
SQL_str row;
SQLCURSOR cur;   

if(ff_prot != NULL)
 fprintf(ff_prot,"\nРасчёт доплаты до средней зарплаты в командировке с учётом награды\n\
------------------------------------------------\n");

if(kodkomand == NULL)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не введены коды командировочных расходов !\n");  
  return;

 }

  
if(kvvrdzn == NULL)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не введены коды входящие в расчёт для расчёта доплаты до средней зарплаты в командировке !\n");  
  return;

 }

if(koddzn == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не введён код доплаты до средней заплаты в командировке с учётом надбавки за награду !\n");  
  return;

 }

if(ff_prot != NULL)
 fprintf(ff_prot,"Код доплаты %d\n",koddzn);

if(iceb_u_proverka(nah_only,koddzn,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код %d исключён из расчёта\n",koddzn);
  return;  
 }


//Проверяем есть ли начисление в карточке
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tn,koddzn);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не введён код начисления %d !\n",koddzn);
  return;
 }    
strncpy(shet,row[0],sizeof(shet)-1);

class iceb_u_str kod_tabel;

for(int shetkk=1; shetkk <= kodkomand[0]; shetkk++)
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"Ищем код табеля для начисления %d\n",kodkomand[shetkk]);
  //Читаем код табеля командировочных
  sprintf(strsql,"select kodt from Nash where kod=%d",kodkomand[shetkk]);
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s\n",strsql);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не найден код начисления %d для расчёта командировочных !\n",kodkomand[shetkk]);
    continue;
   }   
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код табеля %s\n",row[0]);   
  kod_tabel.z_plus(row[0]);
 } 

short dnei=0;
int kolstr;

    //Проверяем введен ли табель
sprintf(strsql,"select kodt,dnei from Ztab where tabn=%d and god=%d  and mes=%d",tn,gp,mp);

if(ff_prot != NULL)
   fprintf(ff_prot,"%s\n",strsql); 

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
 
if(kolstr > 0)
 while(cur.read_cursor(&row) != 0)
  {
   if(iceb_u_proverka(kod_tabel.ravno(),row[0],0,1) != 0)
      continue;

     if(ff_prot != NULL)
        fprintf(ff_prot,"%s %s\n",row[0],row[1]);
      dnei+=atoi(row[1]);
  }
   
if(ff_prot != NULL)
    fprintf(ff_prot,"Количество дней командировочных=%d\n",dnei);
    
if(dnei == 0)
 return;



short dn,mn,gn;
short dk=1,mk=mp,gk=gp;
iceb_u_dpm(&dk,&mk,&gk,4); //уменьшить дату на месяц

dn=dk; mn=mk; gn=gk;

iceb_u_dpm(&dk,&mk,&gk,5); //получить последний день месяца

iceb_u_dpm(&dn,&mn,&gn,4); //уменьшить дату на месяц

//Смотрим начисления за 2 последних месяца
sprintf(strsql,"select datz,knah,suma,shet from Zarp where datz >= '%d-%d-%d' \
and datz <= '%d-%d-%d' and tabn=%d and prn='1' and suma != 0.",
gn,mn,dn,gk,mk,dk,tn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
double suma=0.;

if(ff_prot != NULL)
   fprintf(ff_prot,"Начисления\n");

while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(kvvrdzn,row[1]) < 0)
   continue;

  suma+=atof(row[2]);
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %s %s\n",row[0],row[1],row[2],row[3]);

 }

if(ff_prot != NULL)
   fprintf(ff_prot,"Табель\n");
 
//Смотрим отработанное время

sprintf(strsql,"select kodt,dnei from Ztab where tabn=%d and \
((god=%d and mes=%d) or (god=%d and mes=%d))",tn,gn,mn,gk,mk);

if(ff_prot != NULL)
   fprintf(ff_prot,"%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

int den=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(ktnvr_komand,row[0]) ==  0)
   continue;
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s\n",row[0],row[1]);
  
  den+=atoi(row[1]);

 }


double sum_kr=0.;
if(den != 0)
 {
  sum_kr=suma/den*dnei;
  sum_kr=iceb_u_okrug(sum_kr,okrg);
 }

if(ff_prot != NULL)
 fprintf(ff_prot,"Сумма доплаты %.2f/%d*%d=%.2f\n",
 suma,den,dnei,sum_kr);


short d=0;

iceb_u_dpm(&d,&mp,&gp,5);
/**********
struct ZARP     zp;
zp.dz=d;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

strcpy(zp.shet,shet);
zapzarp(d,mp,gp,tn,1,koddzn,sum_kr,shet,mp,gp,0,0,"",podr,"",zp);
***************/
class ZARP     zp;
zp.tabnom=tn;
zp.prn=1;
zp.knu=koddzn;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

strcpy(zp.shet,shet);
zapzarpw(&zp,sum_kr,d,mp,gp,0,shet,"",0,podr,"",wpredok);

}




