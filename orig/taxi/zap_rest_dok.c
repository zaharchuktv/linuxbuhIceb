/*$Id: zap_rest_dok.c,v 1.11 2011-01-13 13:50:09 sasa Exp $*/
/*08.10.2007	12.12.2005	Белых А.И.	zap_rest_dok.c
Запись шапки нового документа
*/

#include <libsql.h>
#include <iceb_util.h>
#include <time.h>

void t_msql_error(SQL_baza *bd,const char *zapros);
void  vrshet_nomdok(short god,iceb_u_str *nomdok);

extern SQL_baza bd;

int zap_rest_dok(short d,short m,short g,
iceb_u_str *nomdok, //Возвращаем номер документа
const char *nomstol,
const char *kodkl,
const char *famil,
const char *koment,
uid_t kod_operatora,
int podr,
time_t vremz,
float proc_sk)
{
time_t vrem;
time(&vrem);
if(vremz == 0)
  vremz=vrem; //дата и время заказа
//iceb_lock_tables blok("LOCK TABLES Restdok WRITE");//блокируем таблицу

char strsql[300];
sprintf(strsql,"LOCK TABLES Restdok WRITE");

if(sql_zap(&bd,strsql) != 0)
 t_msql_error(&bd,strsql);

//определяем номер документа
vrshet_nomdok(g,nomdok);

sprintf(strsql,"insert into Restdok values(%d,'%s','%04d-%d-%d',%d,'%s','%s','%s','%s',\
%d,%d,%ld,%d,%ld,%d,%.2f)",
g,
nomdok->ravno(),
g,m,d,
0,
nomstol,
kodkl,
famil,
koment,
0,
kod_operatora,
vrem,
podr,
vremz,
0,
proc_sk);

//if(iceb_sql_zapis(strsql,0,0,wpredok) != 0)
// return(1);

//return(0);
int voz=sql_zap(&bd,strsql);

sprintf(strsql,"UNLOCK TABLES");

if(sql_zap(&bd,strsql) != 0)
 t_msql_error(&bd,strsql);

return(voz);

}


