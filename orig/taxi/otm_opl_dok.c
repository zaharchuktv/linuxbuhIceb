/*$Id: otm_opl_dok.c,v 1.15 2011-02-21 07:36:21 sasa Exp $*/
/*06.11.2007	22.12.2004	Белых А.И. 	otm_opl_dok.c
Отметить документ как оплаченный
*/
#include <stdlib.h>
#include "i_rest.h"

extern int  nomer_kas;  //номер кассы
extern SQL_baza bd;
extern uid_t    kod_operatora;

int otm_opl_dok(short god,const char *nomdok,double suma_dok,GtkWidget *wpredok)
{
char strsql[500];

short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);

SQL_str row;
SQLCURSOR cur;
//читаем реквизиты документа
sprintf(strsql,"select nomd,datd,podr,kodkl from Restdok where god=%d and nomd='%s'",god,nomdok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_u_str repl;
  repl.plus("otm_opl_dok Не найден документ !!! ");
  repl.plus(" ");
  repl.plus(god);
  repl.plus(" ");
  repl.plus(nomdok);
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

if(suma_dok == 0.)
  suma_dok=sumapsh(god,row[0],NULL,wpredok);

//Отмечаем документ как оплаченный
sprintf(strsql,"update Restdok set mo=1,vremo=%ld where god=%d and nomd='%s'",time(NULL),god,nomdok);
if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
  return(1);

//Прописываем списание потраченных сумм
if(zap_v_kas(nomer_kas,row[3],suma_dok*-1,2,row[0],ddd,mmm,ggg,
atoi(row[2]),kod_operatora,"") != 0)
 return(1);

return(0);

}





