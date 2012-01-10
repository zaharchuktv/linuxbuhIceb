/*$Id: ras_pensw.c,v 1.11 2011-02-21 07:35:56 sasa Exp $*/
/*22.12.2010	17.02.2005	Белых А.И.	ras_pensw.c
Расчёт пенсионного отчисления
*/
#include        "buhg_g.h"

extern short    kodpen;  /*Код пенсионных отчислений*/
extern char	*shetb; /*Бюджетные счета*/
extern char	*shetpf; /*Счета пенсионного фонда*/
extern short	*kodnvpen; /*Коды не входящие в расчет пенсионного отчисления*/
extern double   okrg; /*Округление*/
extern short	metkarnb; //0-Расчет налогов для бюджета пропорциональный 1-последовательный
extern  short   *kodbl; /*Код больничного*/
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу

void ras_pensw(int tn, //Табельный номер*/
short mp,short gp,
int podr,
double *sumapo, //Общая сумма пенсионного отчисления только с зарплаты 
double *sumapob, //Сумма пенсинного отчисления с бюджетных начислений только с зарплаты
int metka_zap, //0-делать запись 1-нет
int metka_ras, //0-смотреть начиления в массиве 1- в базе
double suma_boln, //0 или сумма больничного для перерасчёта
const char *uder_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR curr;
char shet[32];
class ZARP     zp;
short		d;
double suma_boln_rm=0.;
iceb_u_dpm(&d,&mp,&gp,5);

zp.tabnom=tn;
zp.prn=2;
zp.knu=kodpen;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

*sumapo=*sumapob=0.;

/*Проверяем есть ли код удержания отчисления в пенсионный фонд*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and \
knah=%d",tn,kodpen); 
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) <= 0)
 {

  if(ff_prot != NULL)
   fprintf(ff_prot,"\nНе введён код отчисления в пенсионный фонд ! %d\n",kodpen);
  return;
 }

double sumanb=0.;
double sumabu=0.;
double suma_ndfnz=0.;
double suma_ndfnz_b=0.;
double suma_zarp_o=0.;
double suma_zarp_b=0.;

if(ff_prot != NULL)
 fprintf(ff_prot,"\nРасчет пенсионного отчисления.\n\
---------------------------------------------------\n");
strncpy(shet,row[0],sizeof(shet)-1);

if(iceb_u_proverka(uder_only,kodpen,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код %d исключён из расчёта\n",kodpen);
  return;
 }

double bbpn=0.;
if(metka_ras == 0)
  bbpn=findnahmw(mp,2,kodnvpen,ff_prot,&sumanb,&sumabu,&suma_ndfnz,&suma_ndfnz_b,&suma_zarp_o,&suma_zarp_b,&suma_boln_rm);
else
 {
  bbpn=findnahmw(mp,gp,tn,2,kodnvpen,ff_prot,&sumanb,&sumabu,&suma_ndfnz,&suma_ndfnz_b,&suma_zarp_o,&suma_zarp_b,&suma_boln_rm,wpredok);
  //если есть сyмма больничного значит делаем перерасчёт подоходного для больничных на которые 
  //распространяется социальная льгота 
  if(suma_boln != 0.)
   if(ff_prot != NULL)
    fprintf(ff_prot,"Увеличиваем на сумму больничного\n");
  sumanb+=suma_boln;
  suma_zarp_o+=suma_boln;
 }

if(ff_prot != NULL)
 fprintf(ff_prot,"findnahm - nah=%f Хозрасчет=:%.2f Бюджет=%.2f Недежная форма=%.2f/%.2f\n",
 bbpn,sumanb,sumabu,suma_ndfnz,suma_ndfnz_b);

//rudnfvw(&bbpn,suma_ndfnz,ff_prot);  
bbpn=iceb_u_okrug(bbpn,okrg);

double bbpnb=sumabu;


if(ff_prot != NULL)
 fprintf(ff_prot,"Со всей суммы--------------------\n");
double bb1=pensw(tn,mp,gp,bbpn,ff_prot,wpredok);
bb1=iceb_u_okrug(bb1,okrg)*(-1);
if(ff_prot != NULL)
 fprintf(ff_prot,"\nТолько с зарплаты (для расчёта подоходного налога)-----------\n");

/*Если на больничный не распространяестя социальная льгота
значит он не вошёл в сумму для расчёта пенсионного только с зарплаты
Однако с него нужно брать пенсионный для расчёта подоходного налога 
Для других соц фондов он не входит в расчёт подоходного налога*/
double suma_plus=0.;
if(suma_boln_rm != 0. && kn_bsl != NULL && kodbl != NULL)
 {
  
  for(int nom=1; nom < kn_bsl[0]; nom++)
   for(int nom1=1; nom1 < kodbl[0]; nom1++)
     if(kn_bsl[nom] == kodbl[nom1])
      {
       if(ff_prot != NULL)
        fprintf(ff_prot,"Прибавляем больничный %.2f к сумме на которую не распространяется социальная льгота\n",suma_boln_rm);
       suma_plus=suma_boln_rm;
       break;
      }
 }

*sumapo=pensw(tn,mp,gp,suma_zarp_o+suma_plus,ff_prot,wpredok);
*sumapo=iceb_u_okrug(*sumapo,okrg)*(-1);

//  printw("bb1=%f\n",bb1);
char bros[512];
memset(bros,'\0',sizeof(bros));
if(shetb == NULL || shetpf == NULL)
 {
  
  if(ff_prot != NULL)
    fprintf(ff_prot,"bb1=%f kodpen=%d\n",bb1,kodpen);
  
  strcpy(zp.shet,shet);
  if(metka_zap == 0)
   zapzarpw(&zp,bb1,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//   zapzarp(d,mp,gp,tn,2,kodpen,bb1,shet,mp,gp,0,0,bros,podr,"",zp);
 }
else
 {
  double bb2=0.;
  if(metkarnb == 1) //Последовательный метод расчет налогов
   {
    if(ff_prot != NULL)
      fprintf(ff_prot,"Расчёт пенсионного отчисления для бюджетных отчислений.\n");
      
//    rudnfvw(&bbpnb,suma_ndfnz_b,ff_prot);
    if(ff_prot != NULL)
      fprintf(ff_prot,"Со всей суммы-----------\n");
    bb2=pensw(tn,mp,gp,bbpnb,ff_prot,wpredok)*-1;
    if(ff_prot != NULL)
      fprintf(ff_prot,"\nТолько с зарплаты (для расчёта подоходного)--------\n");
    *sumapob=pensw(tn,mp,gp,suma_zarp_b,ff_prot,wpredok)*-1;
   } 
  if(metkarnb == 0) //Пропорциональный метод расчет налогов
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Пропорциональный:Разбивка суммы:%.2f хозрасчет=%.2f бюджет:%.2f\n",
     bbpn,sumanb,sumabu);

    if(sumabu != 0.)
     {
      if(bbpn != 0.)
       {
        //Если хозрасчет не равен нолю то долю определять надо
        if(sumanb != 0.)
         bb2=bb1*sumabu/bbpn;
        else
         bb2=bb1;

        if(suma_zarp_b != 0.)
         *sumapob=*sumapo*suma_zarp_b/suma_zarp_o;
        else
         *sumapob=0.;
       }
     }
   }

  *sumapob=iceb_u_okrug(*sumapob,okrg);
  bb2=iceb_u_okrug(bb2,okrg);
  iceb_u_polen(shetpf,shet,sizeof(shet),2,',');
  if(ff_prot != NULL)
    fprintf(ff_prot,"Налог бюджет=%.2f\n",bb2);
 
  if(shet[0] != '\0')
   {
    strcpy(bros,gettext("Бюджет"));
    strcpy(zp.shet,shet);
    if(metka_zap == 0)
      zapzarpw(&zp,bb2,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//      zapzarp(d,mp,gp,tn,2,kodpen,bb2,shet,mp,gp,0,0,bros,podr,"",zp);
   }

  iceb_u_polen(shetpf,shet,sizeof(shet),1,',');
  bb2=bb1-bb2;

  bb2=iceb_u_okrug(bb2,okrg);
  if(shet[0] != '\0')
   {
    strcpy(bros,gettext("Хозрасчет"));
    strcpy(zp.shet,shet);
    if(metka_zap == 0)
      zapzarpw(&zp,bb2,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//      zapzarp(d,mp,gp,tn,2,kodpen,bb2,shet,mp,gp,0,0,bros,podr,"",zp);
   }
   
 }

}

