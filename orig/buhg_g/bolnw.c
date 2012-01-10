/* $Id: bolnw.c,v 1.12 2011-04-14 16:09:35 sasa Exp $ */
/*22.12.2010    13.12.1993      Белых А.И.      bolnw.c
Перерасчет подоходного налога с учетом больничных
*/
#include <stdlib.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "zarp1.h"
double boln_pens(int tn,short mp,short gp,int podr,double suma_boln,FILE *ff_prot,GtkWidget*);

extern struct ZAR zar;
extern float    procpen[2]; /*Процент отчисления в пенсионный фонд*/
extern float    procsoc; /*Процент отчисления в соц-страх*/
extern char	*imabaz;
extern double   okrg;
extern short    kodpn;   /*Код подоходного налога*/
extern short    kodpen;  /*Код пенсионных отчислений*/
extern short   *kodbl; /*Код больничного*/
extern short    kodsocstr;  /*Код отчисления на соц-страх*/
extern short	*kodsocstrnv; //Коды не входящие в расчет соцстраха
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short    *kodmp;   /*Коды материальной помощи*/
extern short	*kodnvpen; /*Коды не входящие в расчет пенсионного отчисления*/

extern short	*kodsocstrnv; //Коды не входящие в расчет соцстраха
extern short	*kodbzrnv; //Коды не входящие в расчет отчислений на безработицу
extern SQL_baza bd;

double bolnw(int tn, //Табельный номер
short mp,short gp,
int podr,
const char *uder_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
int             i,i1;
double          zap;
double          nah,nah1;
short           den;
double          nalog1,nalog2;
double          bol;
double          sn;
double          bbb;
char		shet[32];
char		kom[512];
SQL_str         row;
SQLCURSOR cur;
char		strsql[512];
double		npens,pens1;
double		nsoc,soc,soc1;
long		kolstr;
short		kolzap=0;
double		neoblmp=0;
class ZARP     zp;
double		matpomb=0.;
//double		kolnmin=0.;
//int		metkalgot=0;

/*
printw("boln-%d %d.%d %d\n",tn,mp,gp,sovm);
refresh();
*/
if(kodbl == NULL)
  return(0);

iceb_u_dpm(&den,&mp,&gp,5);

memset(&zp,'\0',sizeof(zp));
zp.tabnom=tn;
zp.prn=2;
//zp.knu=knzz; //Назначаеься ниже
zp.dz=den;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

class iceb_u_int MES;
class iceb_u_int GOD;
class iceb_u_double SUM;
class iceb_u_int KNAH;

for(i=0;i<razm;i++)
 {
  if(zar.prnu[i] == 2 )
   break;

  if(provkodw(kodbl,zar.knu[i]) >= 0 && zar.mes[i] != mp)
   {
    if((i1=MES.find(zar.mes[i])) == -1)
     {
      MES.plus(zar.mes[i],-1);
      GOD.plus(zar.god[i],-1);
      SUM.plus(zar.sm[i],-1);
      KNAH.plus(zar.knu[i],-1);
      kolzap++;
     }      
    else
     {
      SUM.plus(zar.sm[i],i1);
     }
   }
 }
if(kolzap == 0)
  return(0.);
  
bbb=sn=bol=nalog1=nalog2=nah=zap=0.;

sn=0.;
short	mes=0,god=0;
double sum=0.;
for(i=0;i<kolzap;i++)
 {
  mes=MES.ravno(i);
  god=GOD.ravno(i);
  sum=SUM.ravno(i);
//  printw("mes=%d sum=%.2f\n",mes,sum);
//  OSTANOV();
      
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"\nПерерасчёт подоходного налога для больничного. (boln.c)\n\
---------------------------------------------------\n");
    fprintf(ff_prot,"Дата перерасчета:%d.%d\n",mes,god);
   }      

  bol=sum;
  nsoc=nah1=nah=findnahw(tn,mes,god,wpredok);

  npens=nah; /*В наcтоящий момент совпадает*/
  double sblpom,sblpom_b;
  neoblmp=matpom1w(tn,mes,god,kodmp,ff_prot,1,&matpomb,0,&sblpom,&sblpom_b,wpredok);
  nah-=neoblmp;
  nah1-=neoblmp;
  nah=iceb_u_okrug(nah,okrg);
  nah1=iceb_u_okrug(nah1,okrg);

  //Больничный должен быть в списке кодов на которые не распространяется социальная
  //льгота, а значит с него всегда берётся налог со всей суммы больничного и смотреть
  //предыдущий месяц просто не нужно.
  
  if(provkodw(kn_bsl,KNAH.ravno(i)) >= 0)
   {
    if(ff_prot != NULL)    
     fprintf(ff_prot,"На больничный не распространяется социальная льгота\n");
      
    bbb=podohrs1w(tn,mp,gp,0,0.,sum,0.,ff_prot,wpredok);
   }
  else
   {
    if(ff_prot != NULL)    
     fprintf(ff_prot,"На больничный распространяется социальная льгота\n");
    bbb=boln_pens(tn,mes,god,podr,sum,ff_prot,wpredok);
   }
    
    

  sprintf(strsql,"select shet from Uder where kod=%d",kodpn);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s %d","bolnw-Не найден код удержания",kodpn);
    iceb_menu_soob(strsql,wpredok);
   }
  else
    strncpy(shet,row[0],sizeof(shet)-1);

  sprintf(kom,gettext("Перерасчет больничного"));
  zp.mesn=mes; zp.godn=god;
  strcpy(zp.shet,shet);
  zp.knu=kodpn;

  if(iceb_u_proverka(uder_only,kodpn,0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код %d исключён из расёта\n",kodpn);  
   }
  else
    zapzarpw(&zp,bbb,den,mp,gp,0,shet,"",0,podr,"",wpredok);

/****************************************************/

  if(ff_prot != NULL)
    fprintf(ff_prot,"\nПерерасчет пенcионного отчисления для больничного\n\
----------------------------------------------------------------\n\
Сумма с которой считалос:%f %d.%d\n",npens,mes,god);    
  double pens_pr=0.;
  if(fabs(procpen[0]-procpen[1]) < 0.001)
   {
    pens_pr=pensw(tn,mp,gp,bol,ff_prot,wpredok)*-1;
   }
  else
   {
    /*Читаем сумму отчисления*/
    sprintf(strsql,"select knah,suma from Zarp where datz >= '%04d-%d-01' and \
  datz <= '%04d-%d-31' and tabn=%d and prn='2' and knah=%d and godn=%d and \
  mesn=%d",god,mes,god,mes,tn,kodpen,god,mes);
  /*
     printw("%s\n",strsql);
     OSTANOV();
  */     
     SQLCURSOR cur;
     if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
       iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

     pens1=0.;
     if(kolstr > 0)
     while(cur.read_cursor(&row) != 0)
      {
       if(provkodw(kodnvpen,row[0]) >= 0)
        continue;
       if(ff_prot != NULL)
        fprintf(ff_prot,"%s %s\n",row[0],row[1]);
        
       pens1+=atof(row[1]);
      }

    if(ff_prot != NULL)
      fprintf(ff_prot,"Сумма пенсионного отчисления: %f %d.%d\n\
  Сумма вместе с больничным:%f+%f=%f\n",pens1,mes,god,npens,bol,npens+bol);
     npens+=bol;

    pens_pr=pensw(tn,mp,gp,npens,ff_prot,wpredok)*-1;
    if(ff_prot != NULL)
      fprintf(ff_prot,"Разница:%f-%f=%f\n",pens_pr,pens1,pens_pr-pens1);  

    pens_pr-=pens1;
   }

  sprintf(strsql,"select shet from Uder where kod=%d",kodpen);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s %d","bolnw-Не найден код удержания",kodpen);
    iceb_menu_soob(strsql,wpredok);
   }
  else
    strncpy(shet,row[0],sizeof(shet)-1);

   strcpy(zp.shet,shet);
   zp.mesn=mes; zp.godn=god;
   zp.knu=kodpen;
  if(iceb_u_proverka(uder_only,kodpen,0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код %d исключён из расёта\n",kodpen);  
   }
  else
   zapzarpw(&zp,pens_pr,den,mp,gp,0,shet,"",0,podr,"",wpredok);
//   zapzarp(den,mp,gp,tn,2,kodpen,pens_pr,shet,mes,god,0,0,kom,podr,"",zp);

/****************************************************/
/**************************/
  if(kodsocstr == 0)
    continue;
  //Если код больничного не входит в расчет соцстаха то и перерасчет делать не надо

  if(kodsocstrnv != NULL)
  for(int i1=1; i1 <= kodsocstrnv[0] ; i1++)
    if(provkodw(kodbl,kodsocstrnv[i1]) >= 0)
      goto vper; //чтобы выйти из цикла
      
//  nsoc-=neoblmp;
      
  if(ff_prot != NULL)
    fprintf(ff_prot,"\nПерерасчет соцстраха для больничного\n\
-----------------------------------------------------\n\
Сумма с которой считалос:%f %d.%d\n",nsoc,mes,god);    

  /*Читаем сумму отчисления*/
  sprintf(strsql,"select suma from Zarp where datz >= '%04d-%d-01' and \
datz <= '%04d-%d-31' and tabn=%d and prn='2' and knah=%d and godn=%d and \
mesn=%d",god,mes,god,mes,tn,kodsocstr,god,mes);

//   printw("%s\n",strsql);
//   OSTANOV();
     
   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
     iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
   soc1=0.;
   if(kolstr > 0)
   while(cur.read_cursor(&row) != 0)
     soc1+=atof(row[0]);

  if(ff_prot != NULL)
    fprintf(ff_prot,"Сумма отчисления на соц-страх: %f %d.%d\n\
Сумма вместе с боьничным:%f+%f=%f\n",soc1,mes,god,nsoc,bol,nsoc+bol);
   nsoc+=bol;

  soc=0.;

  if(procsoc != 0.)
      soc=nsoc*procsoc/100.*(-1);
  if(ff_prot != NULL)
     fprintf(ff_prot,"%f*%f/100.=%f\n",nsoc,procsoc,soc);
     
  if(ff_prot != NULL)
    fprintf(ff_prot,"Разница:%f-%f=%f\n",soc,soc1,soc-soc1);  
  soc-=soc1;
  if(soc != 0.)
   {
    sprintf(strsql,"select shet from Uder where kod=%d",kodsocstr);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
     {
      sprintf(strsql,"%s %d","bolnw-Не найден код удержания",kodsocstr);
      iceb_menu_soob(strsql,wpredok);
      continue;
     }
    else
      strncpy(shet,row[0],sizeof(shet)-1);

    strcpy(zp.shet,shet);
    zp.mesn=mes; zp.godn=god;
    zp.knu=kodsocstr;
    if(iceb_u_proverka(uder_only,kodsocstr,0,0) != 0)
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Код %d исключён из расёта\n",kodsocstr);  
     }
    else
     zapzarpw(&zp,soc,den,mp,gp,0,shet,"",0,podr,"",wpredok);
   }
vper:;
   
 }
//OSTANOV();
return(sn);
}
/**********************************/
/*Перерасчёт больничного за предыдущий месяц для пенсионного фонда*/
/*********************************************/

double boln_pens(int tn, //Табельный номер
short mp,short gp,
int podr,
double suma_boln,
FILE *ff_prot,
GtkWidget *wpredok)
{
char strsql[512];
double prom=0.;

//расчёт отчисления в фонд безработицы
double		fbezrab=0.,fbezrabb=0.;
prom=suma_boln;
for(int i=0; i <= kodbl[0]; i++)
 if(provkodw(kodbzrnv,kodbl[i+1]) >= 0)
  {
   prom=0.;
   break;
  }

fbezrw(tn,mp,gp,podr,&fbezrab,&fbezrabb,1,1,prom,"",ff_prot,wpredok);

//расчёт пенсионного отчисления
double sumapo=0; //Общая сумма пенсионного отчисления только с зарплаты 
double sumapob=0.; //Сумма пенсинного отчисления с бюджетных начислений только с зарплаты

prom=suma_boln;
for(int i=0; i <= kodbl[0]; i++)
 if(provkodw(kodnvpen,kodbl[i+1]) >= 0)
  {
   prom=0.;
   break;
  }
ras_pensw(tn,mp,gp,podr,&sumapo,&sumapob,1,1,prom,"",ff_prot,wpredok);

//расчёт отчисления на соц-страх
double sumaos=0.; //Сумма отчисления на соц-страх только с зарплаты
double sumaosb=0.; //Сумма отчисления на соц-страх бюджет только с зарплаты
prom=suma_boln;
for(int i=0; i <= kodbl[0]; i++)
 if(provkodw(kodsocstrnv,kodbl[i+1]) >= 0)
  {
   prom=0.;
   break;
  }

ras_socw(tn,mp,gp,podr,&sumaos,&sumaosb,1,1,prom,"",ff_prot,wpredok);

//расчёт подоходного налога
double podoh=ras_podohw(tn,mp,gp,podr,fbezrab,fbezrabb,sumapo,sumapob,sumaos,sumaosb,1,1,suma_boln,"",ff_prot,wpredok);
if(ff_prot != NULL)
 fprintf(ff_prot,"Пересчитанный подоходный налог за %d.%d=%.2f\n",mp,gp,podoh);

double podoh_pm=0.;
//Определяем сумму налогообложения за предыдущий месяц
sprintf(strsql,"select SUM(suma) from Zarp where datz >= '%04d-%02d-%02d' \
and datz <= '%04d-%02d-%02d' and tabn=%d and prn='2' and knah=%d and godn=%d and mesn=%d",
gp,mp,1,gp,mp,31,tn,kodpn,gp,mp);

SQL_str row;
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
 if(row[0] != NULL)
  podoh_pm=atof(row[0]);

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"Подоходный за %d.%d = %.2f\n",mp,gp,podoh_pm);
  fprintf(ff_prot,"Разница %.2f - %.2f = %.2f\n",podoh,podoh_pm,podoh-podoh_pm);
 }  

return(podoh-podoh_pm);

}
