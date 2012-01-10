/*$Id: zap_vrest_dok.c,v 1.7 2011-01-13 13:50:09 sasa Exp $*/
/*03.02.2006	14.12.2005	Белых А.И.	zap_vrest_dok.c
Запись в документ материалла или услуги
*/
#include <time.h>
#include <libsql.h>

extern SQL_baza bd;

int zap_vrest_dok(short dd,short md,short gd,
const char *nomd,
int skl,
int metkaz,
int kodm,
const char *ei,
double kolih,
double cena,
const char *koment,
uid_t kod_operatora,
int podr,
float ps) //Процент скидки
{
char strsql[300];
time_t vrem;
time(&vrem);

for(int nz=0; ;nz++)
 {
    sprintf(strsql,"insert into Restdok1 values \
 ('%d-%d-%d','%s',%d,%d,%d,'%s',%.10g,%.10g,'%s',%d,%d,%ld,%d,%d,%d,%d,%.2f)",
  gd,md,dd,
  nomd,
  skl,
  metkaz,
  kodm,
  ei,
  kolih,
  cena,
  koment,
  0,
  kod_operatora,
  vrem,
  podr,
  nz,
  0,
  0,
  ps);
 
  //printf("restdok_v_zapis %s\n",strsql);
  if(sql_zap(&bd,strsql) != 0 )
   {
    if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
     continue;
    else
     {
      printf("zap_vrest_dok-Ошибка записи !\n%s\n",strsql);
//      iceb_msql_error(&bd,"Ошибка ввода записи !",strsql,wpredok);
      return(1);
     }
    
   }
  else
   return(0);
 }
}
