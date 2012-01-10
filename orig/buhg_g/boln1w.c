/* $Id: boln1w.c,v 1.4 2011-09-05 08:18:27 sasa Exp $ */
/*02.09.2011    01.02.2011      Белых А.И.      boln1w.c
Перерасчет подоходного налога с учетом больничных
*/
#include <stdlib.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "zarp1.h"

void boln1_pa(int tn,int podr,short mp,short gp,short mes,short god,double sum,int knah,const char *uder_only,FILE *ff_prot,GtkWidget *wpredok);

extern short    kodpen;  /*Код пенсионных отчислений*/
extern struct ZAR zar;
extern float    procpen[2]; /*Процент отчисления в пенсионный фонд*/
extern float    procsoc; /*Процент отчисления в соц-страх*/
extern char	*imabaz;
extern double   okrg;
extern short    kodpn;   /*Код подоходного налога*/
extern short kodpn_sbol;
extern class iceb_u_str kodpn_all;
extern short   *kodbl; /*Код больничного*/
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short    kodsocstr;  /*Код отчисления на соц-страх*/
extern short	*kodsocstrnv; //Коды не входящие в расчет соцстраха
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short    *kodmp;   /*Коды материальной помощи*/
extern short	*kodnvpen; /*Коды не входящие в расчет пенсионного отчисления*/

extern short	*kodsocstrnv; //Коды не входящие в расчет соцстраха
extern short	*kodbzrnv; //Коды не входящие в расчет отчислений на безработицу
extern SQL_baza bd;

double boln1w(int tn, //Табельный номер
short mp,short gp,
int podr,
const char *uder_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
int             i,i1;
short           den;
double          sn;
char		shet[64];
char		kom[512];
SQL_str         row;
class SQLCURSOR cur;
char		strsql[512];
short		kolzap=0;
struct ZARP     zp;
double bbb=0.;

/*
printw("boln-%d %d.%d %d\n",tn,mp,gp,sovm);
refresh();
*/
if(kodbl == NULL)
  return(0);

iceb_u_dpm(&den,&mp,&gp,5);

memset(&zp,'\0',sizeof(zp));
/***********
zp.dz=den;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
*************/
zp.tabnom=tn;
zp.prn=1;
zp.knu=kodpn;
if(kodpn_sbol != 0)
 zp.knu=kodpn_sbol;
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
  if(fabs(zar.sm[i]) < 0.01)
   continue;
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
  

sn=0.;
short	mes=0,god=0;
double sumbol=0.;
for(i=0;i<kolzap;i++)
 {
  mes=MES.ravno(i);
  god=GOD.ravno(i);
  sumbol=SUM.ravno(i);
//  printw("mes=%d sumbol=%.2f\n",mes,sumbol);
//  OSTANOV();
      
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"\nПерерасчёт подоходного налога для больничного. (%s)\n\
---------------------------------------------------\n",__FUNCTION__);
    fprintf(ff_prot,"Дата перерасчета:%d.%d Сумма больничного:%.2f\n",mes,god,sumbol);

   }      

  /*чтение величины минимальной зарплаты и прожиточного минимума*/
  zar_read_tnw(1,mes,god,ff_prot,wpredok); 

  if(iceb_u_sravmydat(1,mes,god,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) < 0)
   {
    boln1_pa(tn,podr,mp,gp,mes,god,sumbol,KNAH.ravno(i),uder_only,ff_prot,wpredok);
    continue;
   }

  double suma_podoh=0.;
  /*Смотрим какая там сумма подоходного уже посчитана*/
  sprintf(strsql,"select knah,suma from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31' and tabn=%d and prn='2' and suma <> 0.",god,mes,god,mes,tn);

  if(cur.make_cursor(&bd,strsql) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return(0.);
   }
  while(cur.read_cursor(&row) != 0)
   {
    if(iceb_u_proverka(kodpn_all.ravno(),row[0],0,1) == 0)
      suma_podoh+=atof(row[1]);
       
   }  
  if(ff_prot != NULL)
   fprintf(ff_prot,"Сумма подоходного %.2f\n",suma_podoh);
   
  double suma_esv=0.;
  double suma_esvb=0.;
  zaresvw(tn,mes,god,podr,&suma_esv,&suma_esvb,1,sumbol,uder_only,ff_prot,wpredok);

  //расчёт подоходного налога
  bbb=ras_podoh1w(tn,mes,god,podr,suma_esv,suma_esvb,1,1,sumbol,uder_only,ff_prot,wpredok);

  if(ff_prot != NULL)
   fprintf(ff_prot,"Сумма подоходного пересчитанная %.2f\n\
Разница %.2f-%.2f=%.2f\n",bbb,bbb,suma_podoh,bbb-suma_podoh);
  bbb-=suma_podoh;      

  sprintf(strsql,"select shet from Uder where kod=%d",zp.knu);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s-%s %d",__FUNCTION__,gettext("Не найден код удержания"),zp.knu);
    iceb_menu_soob(strsql,wpredok);
   }
  else
    strncpy(shet,row[0],sizeof(shet)-1);

  sprintf(kom,gettext("Перерасчет больничного"));
  zp.mesn=mes; zp.godn=god;
  strcpy(zp.shet,shet);
  if(iceb_u_proverka(uder_only,zp.knu,0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код %d исключён из расчёта\n",zp.knu);
   }
  else
    zapzarpw(&zp,bbb,den,mp,gp,0,shet,kom,0,podr,"",wpredok);
//   zapzarp(den,mp,gp,tn,2,kodpn,bbb,shet,mes,god,0,0,kom,podr,"",zp); 

   
 }
//OSTANOV();
return(sn);
}
/**********************************/
/*Перерасчёт больничного за предыдущий месяц для пенсионного фонда*/
/*********************************************/

double boln_pens1(long tn, //Табельный номер
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
//double i_bezr=0.,i_bezrb=0.;
//fbezrw(tn,mp,gp,podr,&fbezrab,&fbezrabb,1,1,prom,&i_bezr,&i_bezrb,"",ff_prot,wpredok);
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
//double i_pens=0.,i_pensb=0.;
//ras_pensw(tn,mp,gp,podr,&sumapo,&sumapob,1,1,prom,&i_pens,&i_pensb,"",ff_prot,wpredok);
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
//double i_soc=0.,i_socb=0.;
//ras_socw(tn,mp,gp,podr,&sumaos,&sumaosb,1,1,prom,&i_soc,&i_socb,"",ff_prot,wpredok);
ras_socw(tn,mp,gp,podr,&sumaos,&sumaosb,1,1,prom,"",ff_prot,wpredok);

//расчёт подоходного налога
double podoh=ras_podohw(tn,mp,gp,podr,fbezrab,fbezrabb,sumapo,sumapob,sumaos,sumaosb,1,1,suma_boln,"",ff_prot,wpredok);
if(ff_prot != NULL)
 fprintf(ff_prot,"Пересчитанный подоходный налог за %d.%d=%.2f\n",mp,gp,podoh);

double podoh_pm=0.;
//Определяем сумму налогообложения за предыдущий месяц
sprintf(strsql,"select SUM(suma) from Zarp where datz >= '%04d-%02d-%02d' \
and datz <= '%04d-%02d-%02d' and tabn=%ld and prn='2' and knah=%d and godn=%d and mesn=%d",
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

/************************/
/*перерасчёт за предыдущему алгоритму*/
/************************************/

void boln1_pa(int tn,
int podr,
short mp,short gp,short mes,short god,double sum,int knah,
const char *uder_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
struct ZARP     zp;
SQL_str         row;
class SQLCURSOR cur;
char		strsql[512];
double  bol=sum;
double nsoc,nah1,nah,npens;
char shet[64];
char kom[512];
short den=1;
int kolstr=0;
iceb_u_dpm(&den,&mp,&gp,5);

memset(&zp,'\0',sizeof(zp));
/**********
zp.dz=den;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
**************/
zp.tabnom=tn;
zp.prn=1;
zp.knu=kodpn;
if(kodpn_sbol != 0)
 zp.knu=kodpn_sbol;
zp.dz=den;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
//strncpy(zp.shet,shet,sizeof(zp.shet)-1);


  nsoc=nah1=nah=findnahw(tn,mes,god,wpredok);

  npens=nah; /*В наcтоящий момент совпадает*/
  double sblpom,sblpom_b;
  double matpomb=0.;
  double neoblmp=matpom1w(tn,mes,god,kodmp,ff_prot,1,&matpomb,0,&sblpom,&sblpom_b,wpredok);
  nah-=neoblmp;
  nah1-=neoblmp;
  nah=iceb_u_okrug(nah,okrg);
  nah1=iceb_u_okrug(nah1,okrg);

  //Больничный должен быть в списке кодов на которые не распространяется социальная
  //льгота, а значит с него всегда берётся налог со всей суммы больничного и смотреть
  //предыдущий месяц просто не нужно.
  double bbb=0.;  
  if(provkodw(kn_bsl,knah) >= 0)
   {
    if(ff_prot != NULL)    
     fprintf(ff_prot,"На больничный не распространяется социальная льгота\n");
      
    bbb=podohrs1w(tn,mp,gp,0,0.,sum,0.,ff_prot,wpredok);
   }
  else
   {
    if(ff_prot != NULL)    
     fprintf(ff_prot,"На больничный распространяется социальная льгота\n");
    bbb=boln_pens1(tn,mes,god,podr,sum,ff_prot,wpredok);
   }
    
    

  sprintf(strsql,"select shet from Uder where kod=%d",zp.knu);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s-%s %d",__FUNCTION__,gettext("Не найден код удержания"),zp.knu);
    iceb_menu_soob(strsql,wpredok);
   }
  else
    strncpy(shet,row[0],sizeof(shet)-1);

  sprintf(kom,gettext("Перерасчет больничного"));
  zp.mesn=mes; zp.godn=god;
  strcpy(zp.shet,shet);
  if(iceb_u_proverka(uder_only,zp.knu,0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код %d исключён из расчёта\n",zp.knu);
   }
  else
    zapzarpw(&zp,bbb,den,mp,gp,0,shet,"",0,podr,"",wpredok);
//   zapzarp(den,mp,gp,tn,2,kodpn,bbb,shet,mes,god,0,0,kom,podr,"",zp); 


/****************************************************/

  if(ff_prot != NULL)
    fprintf(ff_prot,"\nПерерасчет пенcионного отчисления для больничного\n\
----------------------------------------------------------------\n\
Сумма с которой считалос:%f %d.%d\n",npens,mes,god);    
  double pens_pr=0.;

  if(fabs(procpen[0]-procpen[1]) < 0.001) /*одинаковый процент-перерасчёт не нужен*/
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Одинаковый процент по диапазонам\n");
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

     double pens1=0.;
     if(kolstr > 0)
     while(cur.read_cursor(&row) != 0)
      {
       if(provkodw(kodbzrnv,row[0]) >= 0)
        continue;

       if(ff_prot != NULL)
         fprintf(ff_prot,"%s %s\n",row[0],row[1]);
       pens1+=atof(row[1]);
      }

    if(ff_prot != NULL)
      fprintf(ff_prot,"Сумма пенсионного отчисления: %f %d.%d\n\
Сумма вместе с больничным:%f+%f=%f\n",pens1,mes,god,npens,bol,npens+bol);
     npens+=bol;

    double pens_pr=pensw(tn,mp,gp,npens,ff_prot,wpredok)*-1;

    if(ff_prot != NULL)
      fprintf(ff_prot,"Разница:%f-%f=%f\n",pens_pr,pens1,pens_pr-pens1);  

    pens_pr-=pens1;
   }


  sprintf(strsql,"select shet from Uder where kod=%d",kodpen);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s-%s %d",__FUNCTION__,gettext("Не найден код удержания"),kodpen);
    iceb_menu_soob(strsql,wpredok);
   }
  else
    strncpy(shet,row[0],sizeof(shet)-1);

  strcpy(zp.shet,shet);
  zp.mesn=mes; zp.godn=god;
  if(iceb_u_proverka(uder_only,kodpen,0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код %d исключён из расчёта\n",kodpen);
   }
//  else
//    zapzarp(den,mp,gp,tn,2,kodpen,pens_pr,shet,mes,god,0,0,kom,podr,"",zp);

/****************************************************/
/**************************/
  if(kodsocstr == 0)
    return;

  //Если код больничного не входит в расчет соцстаха то и перерасчет делать не надо

  if(kodsocstrnv != NULL)
  for(int i1=1; i1 <= kodsocstrnv[0] ; i1++)
    if(provkodw(kodbl,kodsocstrnv[i1]) >= 0)
      return; //чтобы выйти из цикла
      

  if(iceb_u_proverka(uder_only,kodsocstr,0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код %d исключён из расчёта\n",kodsocstr);
    return;
   }
      
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
   double soc1=0.;
   if(kolstr > 0)
   while(cur.read_cursor(&row) != 0)
     soc1+=atof(row[0]);

  if(ff_prot != NULL)
    fprintf(ff_prot,"Сумма отчисления на соц-страх: %f %d.%d\n\
Сумма вместе с боьничным:%f+%f=%f\n",soc1,mes,god,nsoc,bol,nsoc+bol);
   nsoc+=bol;

  double soc=0.;

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
    if(sql_readkey(&bd,strsql,&row,&cur) == 0)
     {
      sprintf(strsql,"%s-%s %d",__FUNCTION__,gettext("Не найден код удержания"),kodsocstr);
      iceb_menu_soob(strsql,wpredok);
      return;
     }
    else
      strncpy(shet,row[0],sizeof(shet)-1);

     strcpy(zp.shet,shet);
     zp.mesn=mes; zp.godn=god;
//     zapzarp(den,mp,gp,tn,2,kodsocstr,soc,shet,mes,god,0,0,kom,podr,"",zp);
   }
   
}
