/*$Id: zarkmdw.c,v 1.8 2011-02-21 07:36:00 sasa Exp $*/
/*22.12.2010	05.05.2004	Белых А.И.	zarkmdw.c
Расчёт командировочных 
*/
#include <stdlib.h>
#include        "buhg_g.h"

extern short    *kodkomand; //Коды командировочных расходов
extern short   *knvr_komand; //Коды начислений не входящие в расчёт командировочных
extern short   kodt_b_komand; //Код бюджетного табеля командировочных
extern short   *ktnvr_komand; //Коды табеля не входящие в расчёт командировочных
extern short kod_komp_z_kom; /*Код компенсации за командировку*/
extern double  okrg; /*Округление*/
extern char    *shetb; /*Бюджетные счета начислений*/
extern SQL_baza bd;

void zarkmdw(int tn, //Табельный номер*/
short mp,short gp,
int podr,
const char *nah_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
double itogo_nah=0.; /*Общая сумма всех командировочных начислений*/
int itogo_dnei=0; /*Общая сумма дней в командировке*/
double itogo_nah_zpred2mes=0.; /*Итого начислено за передыдущих два месяца*/
int itogo_dnei_zpred2mes=0; /*Итого отработанных дней за предыдущих два месяца*/
class iceb_u_str shet("");
int kolstr;
short   kodt_komand=0; //Код табеля командировочных
short metka_vk=0; //0-хозрасчёт 1-бюджет
char strsql[512];
SQL_str row;
SQLCURSOR cur;   

short dn,mn,gn;
short dk=1,mk=mp,gk=gp;

if(ff_prot != NULL)
  fprintf(ff_prot,"\n%s-Начисление командировочных\n\
----------------------------------------------\n",__FUNCTION__);  
  
if(kodkomand == NULL)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не введено код командировочных начислений!\n");  
  return;
 }

iceb_u_dpm(&dk,&mk,&gk,4); //уменьшить дату на месяц

dn=dk; mn=mk; gn=gk;

iceb_u_dpm(&dk,&mk,&gk,5); //получить последний день месяца

iceb_u_dpm(&dn,&mn,&gn,4); //уменьшить дату на месяц

//Смотрим отработанное время

sprintf(strsql,"select kodt,dnei from Ztab where tabn=%d and \
((god=%d and mes=%d) or (god=%d and mes=%d))",tn,gn,mn,gk,mk);

if(ff_prot != NULL)
   fprintf(ff_prot,"%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);


while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(ktnvr_komand,atoi(row[0])) >= 0)
   continue;
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s\n",row[0],row[1]);
  
  itogo_dnei_zpred2mes+=atoi(row[1]);

 }

if(itogo_dnei_zpred2mes == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Количество отработанных дней за предыдущих два месяца равно нолю!\n");
  return;
 }

int kodkomand_sp=0;
short d=0;

iceb_u_dpm(&d,&mp,&gp,5);

class ZARP     zp;
zp.tabnom=tn;
zp.prn=1;
zp.knu=kodkomand_sp;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;



for(int shetkk=1; shetkk <= kodkomand[0]; shetkk++)
 {
  kodkomand_sp=kodkomand[shetkk];
  if(iceb_u_proverka(nah_only,kodkomand_sp,0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код %d исключён из расчёта\n",kodkomand_sp);
    continue;
   }
  if(ff_prot != NULL)
   fprintf(ff_prot,"Смотрим код табеля для начисления %d\n",kodkomand_sp);
   
  //Читаем код табеля командировочных
  sprintf(strsql,"select kodt from Nash where kod=%d",kodkomand_sp);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не найден код начисления %d для расчёта командировочных !\n",
     kodkomand_sp);
    continue;
   }   
  kodt_komand=atoi(row[0]);
  if(kodt_komand == 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не введён код табеля для кода командировочных %d !\n",
     kodkomand_sp);
    continue;
   }
  short dnei=0;
  if(kodt_b_komand == kodt_komand)
   {
    //Проверяем введен ли табель
    sprintf(strsql,"select dnei from Ztab where tabn=%d and god=%d \
and mes=%d and kodt=%d",tn,gp,mp,kodt_b_komand);

    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

    while(cur.read_cursor(&row) != 0)
     dnei+=atoi(row[0]);
    if(dnei != 0)
     metka_vk=1; 
   }

  if(metka_vk == 0)
   {
    //Проверяем введен ли табель
    sprintf(strsql,"select dnei from Ztab where tabn=%d and god=%d \
and mes=%d and kodt=%d",tn,gp,mp,kodt_komand);

    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

    while(cur.read_cursor(&row) != 0)
     dnei+=atoi(row[0]);
   }
   
  if(ff_prot != NULL)
    fprintf(ff_prot,"dnei=%d\n",dnei);
    
  if(dnei == 0)
   continue;   
  itogo_dnei+=dnei;

  shet.new_plus("");
  //Проверяем есть ли начисление в карточке
  sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
  knah=%d",tn,kodkomand_sp);

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
    continue;

  shet.new_plus(row[0]);

   


  //Смотрим начисления за 2 последних месяца
  sprintf(strsql,"select knah,suma,shet from Zarp where datz >= '%d-%d-%d' \
and datz <= '%d-%d-%d' and tabn=%d and prn='1' and suma != 0.",
  gn,mn,dn,gk,mk,dk,tn);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

  double suma=0.;

  if(ff_prot != NULL)
     fprintf(ff_prot,"Начисления\n");
  itogo_nah_zpred2mes=0.;
  while(cur.read_cursor(&row) != 0)
   {
    if(provkodw(knvr_komand,atoi(row[0])) >= 0)
     continue;

    itogo_nah_zpred2mes+=atof(row[1]);

    if(kodt_b_komand != 0)
     {
      if(metka_vk == 0)
       if(iceb_u_proverka(shetb,row[2],0,1) != 0)
        {
         suma+=atof(row[1]);
         if(ff_prot != NULL)
          fprintf(ff_prot,"%s %s %s\n",row[0],row[1],row[2]);
        }
      if(metka_vk == 1)
       if(iceb_u_proverka(shetb,row[2],0,1) == 0)
        {
         suma+=atof(row[1]);
         if(ff_prot != NULL)
          fprintf(ff_prot,"%s %s %s\n",row[0],row[1],row[2]);
        }
       
     }
    else
     {
      suma+=atof(row[1]);
      if(ff_prot != NULL)
          fprintf(ff_prot,"%s %s %s\n",row[0],row[1],row[2]);
     }
   }

  if(ff_prot != NULL)
     fprintf(ff_prot,"Табель\n");
   


  double sum_kr=0.;
  if(itogo_dnei_zpred2mes != 0)
   {
    sum_kr=suma/itogo_dnei_zpred2mes*dnei;
    sum_kr=iceb_u_okrug(sum_kr,okrg);
   }

  if(ff_prot != NULL)
   fprintf(ff_prot,"Командировочные %.2f/%d*%d=%.2f\n",
   suma,itogo_dnei_zpred2mes,dnei,sum_kr);



  strcpy(zp.shet,shet.ravno());
  zapzarpw(&zp,sum_kr,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
//  zapzarp(d,mp,gp,tn,1,kodkomand_sp,sum_kr,shet,mp,gp,0,0,"",podr,"",zp);
  itogo_nah+=sum_kr;
 }

if(ff_prot != NULL)
 fprintf(ff_prot,"Код компенсации за командировку=%d\n",kod_komp_z_kom);

if(kod_komp_z_kom == 0)
 return;
if(ff_prot != NULL)
 {
  fprintf(ff_prot,"%s-Расчёт компенсации за командировку\n\
---------------------------------------------------------\n",__FUNCTION__); 
 }
sprintf(strsql,"select kodt from Nash where kod=%d",kod_komp_z_kom);
if(ff_prot != NULL)
   fprintf(ff_prot,"%s\n",strsql);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден код начисления %d для расчёта командировочных !\n",
   kod_komp_z_kom);
  return;
 }   

shet.new_plus("");

//Проверяем есть ли начисление в карточке
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and knah=%d",tn,kod_komp_z_kom);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено код компенсации в списке начислений в карточке работкика!");
 
  return;

 }  

shet.new_plus(row[0]);

if(itogo_nah == 0.)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Сумма командировочных равна нолю!");
  return; 
 }

if(itogo_dnei == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Количество дней в командировке равно нолю!\n");
  return;
 }


double snah_kom=itogo_nah/itogo_dnei;
double snah_zp2mes=itogo_nah_zpred2mes/itogo_dnei_zpred2mes;


if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\
Общая сумма команидировочных=%.2f\n\
Общее количество дней в командировке=%d\n\
Среднее начисление за один день=%.2f/%d=%f\n\
Начислено за предыдущих два месяца=%.2f\n\
Количество отработанных дней за предыдущих два месяца=%d\n\
Среднее начисление за 1 день за предыдущих два месяца=%f\n",
  itogo_nah,
  itogo_dnei,
  itogo_nah,
  itogo_dnei,
  snah_kom,
  itogo_nah_zpred2mes,
  itogo_dnei_zpred2mes,
  snah_zp2mes);
 }

if(snah_zp2mes < snah_kom)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Средняя зарплата за предыдущих 2 месяца меньше среднего начисления за командировку!");
  return;
 }
double raznica=snah_zp2mes-snah_kom;
double suma_komp_zkom=raznica*itogo_dnei;

if(ff_prot != NULL)
 fprintf(ff_prot,"\
Разница между средними начислениями=%f-%f=%f\n\
Умножаем на количество дней в командировке=%f*%d=%f\n",
 snah_zp2mes,
 snah_kom,
 raznica,
 raznica,
 itogo_dnei,
 suma_komp_zkom);  

strcpy(zp.shet,shet.ravno());
zp.knu=kod_komp_z_kom;
zapzarpw(&zp,suma_komp_zkom,d,mp,gp,0,shet.ravno(),"",0,podr,"",wpredok);
//zapzarp(d,mp,gp,tn,1,kod_komp_z_kom,suma_komp_zkom,shet.ravno(),mp,gp,0,0,"",podr,"",zp);

}
