/* $Id: kreditw.c,v 1.11 2011-02-21 07:35:52 sasa Exp $ */
/*22.12.2010    01.12.1992      Белых А.И.      kreditw.c
Подпрограмма расчета удержаний за кредит
Переменные данные лежат в файле кредит
*/
#include <stdlib.h>
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"
#include "zarp1.h"

extern struct ZAR zar;
extern SQL_baza bd;

void kreditw(int tn,short mp,short gp,int podr,const char *uder_only,FILE *ff_prot,GtkWidget *wpredok)
{
char            br[512];
int             ku=0;   /*Код начисления*/
double          nah=0.;
int           tnn=0;
short		d,m,g;
SQL_str         row;
char		strsql[512];
char		shet[32];
class ZARP	zp;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

if(ff_prot != NULL)
 fprintf(ff_prot,"\nРасчёт удержания по кредиту\n----------------------------------------------\n");

sprintf(strsql,"select str from Alx where fil='zarkredit.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки zarkredit.alx\n");
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(row_alx[0][0] == '#')
   continue;
   
  iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
  ku=(int)iceb_u_atof(br);
  if(ku == 0)
     continue;
       
  iceb_u_polen(row_alx[0],br,sizeof(br),1,'|');
  tnn=atoi(br);
  if(tnn == tn)
   {

    if(iceb_u_proverka(uder_only,ku,0,0) != 0)
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Код %d исключён из расчёта\n",ku);
      return;
     }

    iceb_u_polen(row_alx[0],br,sizeof(br),4,'|');
    d=0;
    iceb_u_rsdat(&d,&m,&g,br,1);
    if(d != 0 && mp > m && gp >= g)
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Закончисля срок действия. Расчёт не производим.\n");
      break;
     }
    iceb_u_polen(row_alx[0],br,sizeof(br),3,'|');
    if(strchr(br,'%') != NULL)
     {
      double procent=iceb_u_atof(br);
      if(ff_prot != NULL)
       fprintf(ff_prot,"Процент от суммы начисленной %f.\n",procent);
      //Узнаём всю сумму начиления
      for(int ii=0 ; ii < razm; ii++)
       {
        if(zar.denz[ii] == 0)
         break;

        if(zar.prnu[ii] == 1)
          nah+=zar.sm[ii];
       }
      if(ff_prot != NULL)
       fprintf(ff_prot,"Сумма всех начислений:%.2f\n",nah);
       
      nah=nah*procent/100.;
      nah=iceb_u_okrug(nah,0.01);
     }
    else
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Стабильная сумма.\n");
      nah=(double)iceb_u_atof(br);
      
     }
    break;
   }
 }

if(tn != tnn)
 {
   if(ff_prot != NULL)
    fprintf(ff_prot,"Нет настроек для удержания.\n");
  return;
 }

/*printw("tn=%d tnn=%d nah=%.2f ku=%d\n",tn,tnn,nah,ku);*/
if(nah == 0.)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"Начисленная сумма равна нолю.\n");
   
  return;
 }

if(ff_prot != NULL)
 fprintf(ff_prot,"Код удержания %d. Сумма удержания %.2f\n",ku,nah);
 
SQLCURSOR curr;
/*Проверяем есть ли код начисления в списке*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and \
knah=%d",tn,ku); 
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не введён код удержания %d в список удержаний !\n",ku);
  return;
 }
strncpy(shet,row[0],sizeof(shet)-1);

nah=nah*(-1);

iceb_u_dpm(&d,&mp,&gp,5);

zp.tabnom=tn;
zp.prn=2;
zp.knu=ku;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
strcpy(zp.shet,shet);

zapzarpw(&zp,nah,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//zapzarp(d,mp,gp,tn,2,ku,nah,shet,mp,gp,0,0,br,podr,"",zp); 
}
