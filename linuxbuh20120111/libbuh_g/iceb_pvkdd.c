/*$Id: iceb_pvkdd.c,v 1.8 2011-02-21 07:36:07 sasa Exp $*/
/*17.11.2008	22.04.2008	Белых А.И.	iceb_pvkdd.c
Проверка возможности корректировки даты документа
Проверяется есть ли к документу проводки по старой дате документа
Если есть - проверяется заблокированы ли проводки по обеим датам
Если хоть одна дата заблокирована корректировать дату документа
нельзя
Возвращаем 0- можно корректировать
           1-нельзя
*/           
#include "iceb_libbuh.h"

extern SQL_baza bd;

int iceb_pvkdd(int metka_ps,
short dd,short md,short gd, /*Старая дата документа*/
short mk,short gk, /*Новая дата документа*/
int pod,
const char *nomdok,
int tipz,
GtkWidget *wpredok)
{
char		strsql[512];
char kto[20];
memset(kto,'\0',sizeof(kto));

iceb_mpods(metka_ps,kto); /*получение символьной метки подсистемы*/


SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

sprintf(strsql,"select distinct sh,shk from Prov where kto='%s' and pod=%d \
and nomd='%s' and datd = '%04d-%02d-%02d' and tz=%d and datp='%04d-%02d-%02d' and deb <> 0. limit 1",
kto,pod,nomdok,gd,md,dd,tipz,gd,md,dd);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"",strsql,wpredok);
  return(1);
 }
if(kolstr == 0)
 return(0);
  
if(iceb_pvglkni(md,gd,wpredok) != 0)
 return(1);
if(iceb_pvglkni(mk,gk,wpredok) != 0)
 return(1);


while(cur.read_cursor(&row) != 0)
 {
  
  if(iceb_pbsh(md,gd,row[0],row[1],"",wpredok) != 0)
   return(1);
  if(iceb_pbsh(mk,gk,row[0],row[1],"",wpredok) != 0)
   return(1);
 }

return(0);

}
