/*$Id: zap_v_kas.c,v 1.14 2011-01-13 13:50:09 sasa Exp $*/
/*09.02.2006	24.12.2004	Белых А.И.	zap_v_kas.c
Запись суммы в кассу
*/
#include <unistd.h>
//#include "i_rest.h"
#include <iceb_util.h>
#include <libsql.h>
//#include "i_kontr_vv.h"

void t_msql_error(SQL_baza *bd,const char *zapros);

extern SQL_baza bd;

int zap_v_kas(int nomkas,const char *kodkl,double suma,int kodz,
const char *nomdok,
short dd,short md,short gd,
int podr,
uid_t  kod_operatora,
const char *koment)
{
char strsql[500];

short d,m,g;
iceb_u_poltekdat(&d,&m,&g);

sprintf(strsql,"insert into Restkas  \
values('%s',%d,'%04d-%02d-%02d','%s',%d,%.2f,'%s','%04d-%02d-%02d',%d,%d,'%s')",
kodkl,nomkas,g,m,d,iceb_u_vrem_tek(),kodz,suma,
nomdok,gd,md,dd,podr, 
kod_operatora,koment);

int voz=0;
if((voz=sql_zap(&bd,strsql)) != 0)
  t_msql_error(&bd,strsql);
return(voz);
}

