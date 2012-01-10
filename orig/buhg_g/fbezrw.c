/* $Id: fbezrw.c,v 1.9 2011-02-21 07:35:51 sasa Exp $ */
/*22.12.2010	03.08.2000	Белых А.И.   	fbezrw.c
Расчет отчислений в фонд безработицы
*/
#include        <math.h>
#include        "buhg_g.h"


extern short	metkarnb; //Расчет налогов для бюджета пропорциональный 1-последовательный
extern short    kodbzr;  /*Код отчисления на безработицу*/
extern float    procbez; /*Процент отчисления на безработицу*/
extern double   ogrzp[2]; /*Ограничение на начисление пенсионного отчисления*/
extern double   okrg; /*Округление*/
extern char	*shetb; /*Бюджетные счета*/
extern char	*shpnb;   /*Бюджетный счет подоходного налога*/
extern char	*shetfb; /*Счета фонда безработицы*/
extern short	*kodbzrnv; //Коды не входящие в расчет отчислений на безработицу
extern short    *kodmp;   /*Коды материальной помощи*/

void fbezrw(int tabn,short mp,short gp,
int podr, //Подразделение
double *fbezr, //Общая сумма фонда безработицы с чистой зарплаты
double *fbezrb, //Сумма безработицы по бюджету с чистой зарплаты
int metka_zap, //0-записывать 1-не записывать
int metka_ras, //0-смотреть в массиве 1-в базе
double suma_boln,
const char *uder_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
char		bros[512];
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];
char		shet[32];
double		bbpn;
double		sumanb,sumabu;
double		sumab=0.;
double		bb1,bb2;
short		den;
class ZARP     zp;
double          suma_zarp_o; //Сумма чистой зарплаты общая
double          suma_zarp_b; //Сумма чистой зарплаты бюджетная
double suma_boln_rm=0.;

*fbezr=*fbezrb=0.;
iceb_u_dpm(&den,&mp,&gp,5);

zp.tabnom=tabn;
zp.prn=2;
zp.knu=kodbzr;
zp.dz=den;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

/*Проверяем есть ли код удержания отчисления в фонд безработицы*/

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and \
knah=%d",tabn,kodbzr);
double suma_ndfnz,suma_ndfnz_b;
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) >= 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"\nРасчёт удержания в фонд безработицы\n\
-----------------------------------------------------\n");
  if(iceb_u_proverka(uder_only,kodbzr,0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код %d исключён из расчёта\n",kodbzr);
    return;   
   }

  strncpy(shet,row[0],sizeof(shet)-1);

/*Пока кодов нет*/
/*Пересчитываем чтобы учесть больничный если он есть берется с общей
  суммы так как нет диапазона процентов*/
  if(metka_ras == 0)
    bbpn=findnahmw(mp,4,kodbzrnv,ff_prot,&sumanb,&sumabu,&suma_ndfnz,&suma_ndfnz_b,&suma_zarp_o,&suma_zarp_b,&suma_boln_rm);
  else
   {
    bbpn=findnahmw(mp,gp,tabn,4,kodbzrnv,ff_prot,&sumanb,&sumabu,&suma_ndfnz,&suma_ndfnz_b,&suma_zarp_o,&suma_zarp_b,&suma_boln_rm,wpredok);
    //если есть сyмма больничного значит делаем перерасчёт подоходного для больничных на которые 
    //распространяется социальная льгота 
    if(suma_boln != 0.)
     if(ff_prot != NULL)
      fprintf(ff_prot,"Увеличиваем на сумму больничного\n");
    bbpn+=suma_boln;
    sumanb+=suma_boln;
    suma_zarp_o+=suma_boln;
   }
//  rudnfvw(&bbpn,suma_ndfnz,ff_prot);

//  rudnfvw(&suma_zarp_o,suma_ndfnz,ff_prot);
//  rudnfvw(&suma_zarp_b,suma_ndfnz_b,ff_prot);

  if(ff_prot != NULL)
    fprintf(ff_prot,"С общей суммы\n");
  bb1=fbezr_sumnalw(bbpn);

  if(ff_prot != NULL)
    fprintf(ff_prot,"Сумма=%.2f Безробіття: %.2f\n",bbpn,bb1);

  if(ff_prot != NULL)
    fprintf(ff_prot,"Только с зарплаты\n");

  *fbezr=fbezr_sumnalw(suma_zarp_o);

  if(ff_prot != NULL)
    fprintf(ff_prot,"Сумма=%.2f Безробіття: %.2f\n",suma_zarp_o,*fbezr);


  
  memset(bros,'\0',sizeof(bros));
  if(shetb == NULL || shetfb == NULL)
   {
    strcpy(zp.shet,shet);
    if(ff_prot != NULL)
      fprintf(ff_prot,"Не бюджетная организация. %f\n",bb1);   
    if(metka_zap == 0)
     zapzarpw(&zp,bb1,den,mp,gp,0,shet,"",0,podr,"",wpredok);
//     zapzarp(den,mp,gp,tabn,2,kodbzr,bb1,shet,mp,gp,0,0,bros,podr,"",zp);
   }
  else
   {
    if(ff_prot != NULL)
      fprintf(ff_prot,"Бюджетная организация.\n");   
    bb2=0.;
    if(metkarnb == 1) //Последовательный расчет бюджет/хозрасчет
     { 
      //Считаем налог для бюджета
           
      sumab=sumabu;
//      rudnfvw(&sumab,suma_ndfnz_b,ff_prot);            
      if(ff_prot != NULL)
          fprintf(ff_prot,"С общей суммы\n");

      bb2=fbezr_sumnalw(sumab);
      if(ff_prot != NULL)
        fprintf(ff_prot,"Бюджет последовательный %.2f (%f)\n",bb2,sumabu);            

      if(ff_prot != NULL)
       fprintf(ff_prot,"Только с зарплаты\n");

      *fbezrb=fbezr_sumnalw(suma_zarp_b);
      if(ff_prot != NULL)
        fprintf(ff_prot,"Бюджет последовательный %.2f (%f)\n",*fbezrb,suma_zarp_b);            
     }

/*****************************************/

    if(metkarnb == 0) //Пропорциональный расчет налога
     { 
      
      if(ff_prot != NULL)
       fprintf(ff_prot,"Разбивка суммы: не бюджет=%.2f бюджет:%.2f\n",sumanb,sumabu);

      if(sumabu != 0.)
       {
//        double zarpl=0.;
        //Узнаем долю бюджета в оставшейся сумме к расчету
        //Если хозрасчет не равен нолю то долю определять надо
        if(sumanb != 0.)
          bb2=bb1*sumabu/bbpn;
        else
          bb2=bb1;

        if(suma_zarp_b != 0.)
          *fbezrb=*fbezr*suma_zarp_b/suma_zarp_o;
        else
          *fbezrb=0.;

//        *fbezrb=bb2;
       }
     }

    bb2=iceb_u_okrug(bb2,okrg);
    iceb_u_polen(shetfb,shet,sizeof(shet),2,',');
    if(ff_prot != NULL)
       fprintf(ff_prot,"Бюджет %.2f\n",bb2);            
        
    if(shet[0] != '\0')
     {
      strcpy(bros,gettext("Бюджет"));
      if(ff_prot != NULL)
        fprintf(ff_prot,"%s %.2f\n",bros,bb2);
      strcpy(zp.shet,shet);
     
      if(metka_zap == 0)
        zapzarpw(&zp,bb2,den,mp,gp,0,shet,"",0,podr,"",wpredok);
//        zapzarp(den,mp,gp,tabn,2,kodbzr,bb2,shet,mp,gp,0,0,bros,podr,"",zp);
     }


     iceb_u_polen(shetfb,shet,sizeof(shet),1,',');
     bb2=bb1-bb2;
        
     bb2=iceb_u_okrug(bb2,okrg);
     if(shet[0] != '\0')
      {
       strcpy(bros,gettext("Хозрасчет"));
        if(ff_prot != NULL)
           fprintf(ff_prot,"%s %.2f\n",bros,bb2);
        strcpy(zp.shet,shet);
       if(metka_zap == 0)
        zapzarpw(&zp,bb2,den,mp,gp,0,shet,"",0,podr,"",wpredok);
//        zapzarp(den,mp,gp,tabn,2,kodbzr,bb2,shet,mp,gp,0,0,bros,podr,"",zp);
      }
   }
 }
else
 if(ff_prot != NULL)
  fprintf(ff_prot,"Не введён код отчисления в фонд безработицы !\n");
}
