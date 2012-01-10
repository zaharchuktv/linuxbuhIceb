/*$Id: zapzkorw.c,v 1.9 2011-02-21 07:35:59 sasa Exp $*/
/*23.06.2009	15.02.2003	Белых А.И.	zapzkorw.c
Подпрограмма записи в кассовый ордер
Если вернули 0 - запись сделана
             1 - такая запись уже есть
             2 - другие ошибки записи
*/
#include        <time.h>
#include	"buhg_g.h"
#include <unistd.h>

extern SQL_baza bd;

int	zapzkorw(const char *kassa,int tipz,
short dd,short md,short gd,
const char *nomd,const char *kodkont,double suma,
const char *kontrz,
const char *koment,
GtkWidget *wpredok)
{
time_t vrem=0;
char strsql[512];

time(&vrem);

//Уникальный ключ не позволит в одном документе иметь более одной записи на контрагента

if(kontrz[0] == '\0')
 sprintf(strsql,"insert into Kasord1 \
values(%s,%d,'%d-%d-%d','0000-00-00','%s','%s',%.2f,%d,0,%d,%ld,'%s')",
 kassa,tipz,gd,md,dd,nomd,kodkont,suma,gd,
 iceb_getuid(wpredok),vrem,koment);

if(kontrz[0] != '\0')
  sprintf(strsql,"update Kasord1 set \
kontr='%s',\
suma=%.2f,\
ktoi=%d,\
vrem=%ld,\
koment='%s' \
where kassa=%s and god=%d and tp=%d and nomd='%s' and kontr='%s'",
   kodkont,suma,iceb_getuid(wpredok),vrem,koment,
   kassa,gd,tipz,nomd,kontrz);

//printf("\nzapzkorw-%s\n",strsql);

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
   return(1);
  else
   {
    iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
    return(2);
   } 
 }
return(0);
}
