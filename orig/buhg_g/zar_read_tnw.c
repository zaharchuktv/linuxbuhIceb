/*$Id: zar_read_tnw.c,v 1.9 2011-04-14 16:09:36 sasa Exp $*/
/*09.12.2010	05.02.2007	Белых А.И	zar_read_tnw.c
Чтение настроек минимальной зарплаты и прожиточного минимума 
по дате
*/
#include "buhg_g.h"

//double minzar=0.; /*Минимальная зарплата*/		
//double minzar_ng=0.; /*Минимальная зарплата на начало года*/		
//float  prog_min_ng=0.; //Прожиточный минимум на начало года
//float  prog_min_tek=0.; //Прожиточный минимум текущий
double ogrzp[2]; /*Разграниечение зарплаты для расчета пенсионных*/
//float  max_sum_for_soc=0.; //Максимальная сумма для расчёта соц.отчислений
extern SQL_baza bd;

void zar_read_tnw(short dn,short mn,short gn, //дата на которую нам нужны данные
FILE *ff_prot,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
//max_sum_for_soc=minzar=minzar_ng=prog_min_ng=prog_min_tek=0.;
ogrzp[0]=ogrzp[1]=0.;

sprintf(strsql,"select str from Alx where fil='zar_mz_pm.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки zar_mz_pm.alx\n");
  return;
 }


char data[512];
char stroka_zap[1024];
memset(stroka_zap,'\0',sizeof(stroka_zap));
char stroka_zap_ngod[1024];
memset(stroka_zap_ngod,'\0',sizeof(stroka_zap_ngod));
short d,m,g;
double prog_min_zap=0.;
double prog_min_ngod_zap=0.;
double min_zarp_ngod_zap=0.;
double min_zarp_tek_zap=0.;
double ogr_sum_zap=0.;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
   
  if(iceb_u_polen(row_alx[0],data,sizeof(data),1,'|') != 0)
   continue; 
  if(iceb_u_rsdat(&d,&m,&g,data,1) != 0)
   continue;
   
  if(iceb_u_sravmydat(d,m,g,1,1,gn) <= 0)
   {
    iceb_u_polen(row_alx[0],&min_zarp_ngod_zap,2,'|');
    strncpy(stroka_zap_ngod,row_alx[0],sizeof(stroka_zap_ngod)-1);
    iceb_u_polen(row_alx[0],&prog_min_ngod_zap,3,'|');
   }

  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) <= 0)
   {
    iceb_u_polen(row_alx[0],&min_zarp_tek_zap,2,'|');
    iceb_u_polen(row_alx[0],&prog_min_zap,3,'|');
    iceb_u_polen(row_alx[0],&ogr_sum_zap,4,'|');
    strncpy(stroka_zap,row_alx[0],sizeof(stroka_zap)-1);
            
   }  

 }
/*********
minzar_ng=min_zarp_ngod_zap;
minzar=min_zarp_tek_zap;
prog_min_ng=prog_min_ngod_zap;
prog_min_tek=prog_min_zap;
***********/
if(gn <= 2006)
 ogrzp[0]=150;
else
  ogrzp[0]=prog_min_zap;

//max_sum_for_soc=ogr_sum_zap;
ogrzp[1]=ogr_sum_zap;


}
