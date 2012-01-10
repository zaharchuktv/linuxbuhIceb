/*$Id: otm_opl_dok_t.c,v 1.7 2011-02-21 07:36:21 sasa Exp $*/
/*14.11.2008	22.12.2004	Белых А.И. 	otm_opl_dok_t.c
Отметить документ как оплаченный (Терминальная версия)
*/
#include <stdlib.h>
#include "i_kontr_vv.h"

extern int  nomer_kas;  //номер кассы
extern SQL_baza bd;
extern uid_t    kod_operatora;

int otm_opl_dok_t(short god,const char *nomdok,double suma_dok)
{
char strsql[500];

short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);

SQL_str row;
SQLCURSOR cur;
//читаем реквизиты документа
sprintf(strsql,"select nomd,datd,podr,kodkl from Restdok where god=%d and nomd='%s'",
god,nomdok);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  printf("otm_opl_dok_t-Не найден документ !!! Год %d Номер документа %s\n",god,nomdok);
  return(1);
 }
if(suma_dok == 0.)
  suma_dok=sumapsh_t(god,row[0]);

//Отмечаем документ как оплаченный
sprintf(strsql,"update Restdok set mo=1,vremo=%ld where god=%d and nomd='%s'",time(NULL),god,nomdok);
if(sql_zap(&bd,strsql) != 0)
 {
  printf("otm_opl_dok_t-Ошибка записи !\n%s\n",strsql);
  return(1);
 }

//Прописываем списание потраченных сумм
if(zap_v_kas(nomer_kas,row[3],suma_dok*-1,2,row[0],ddd,mmm,ggg,
atoi(row[2]),kod_operatora,"") != 0)
 return(1);

return(0);

}





