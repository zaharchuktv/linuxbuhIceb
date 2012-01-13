/*$Id: zaphkorw.c,v 1.15 2011-02-21 07:35:59 sasa Exp $*/
/*29.06.2009	15.01.2003 	Белых А.И.	zaphkorw.c
Подпрограмма записи шапки кассового ордера
Если вернули 0 - запись сделана
             1 - такая запись уже есть
             2 - другие ошибки записи
*/
#include <stdlib.h>
#include        <time.h>
#include        <pwd.h>
#include        <ctype.h>
#include	"../headers/buhg_g.h"
#include <unistd.h>

extern SQL_baza bd;

int   zaphkorw(int mkor, //0-новая запись 1-корректировка
const char *kassa,int tipz,
short d,short m,short g,const char *nomd,const char *shetk,const char *kodop,
const char *osnov,const char *dopol,const char *fio,const char *dokum,
const char *kassaz,const char *nomdz,short dz,short mz,short gz,
const char *nomer_bl, /*Номер бланка*/
const char *nomv,
GtkWidget *wpredok)
{
time_t vrem=0;
char strsql[1024];

time(&vrem);

//При вводе новой записи и корректировки старой уникальный ключ не даст ввести две одинаковых записи

if(mkor == 0)
 sprintf(strsql,"insert into Kasord values(%s,%d,'%d-%d-%d','%s','%s',\
'%s',%d,%d,'%s','%s','%s','%s',%d,%d,%ld,'%s','%s')",
 kassa,tipz,g,m,d,nomd,
 shetk,kodop,0,0,
 osnov,dopol,fio,
 dokum,g,iceb_getuid(wpredok),
 vrem,
 nomer_bl,
 nomv);


if(mkor == 1)
   sprintf(strsql,"update Kasord set \
kassa='%s',\
datd='%d-%d-%d',\
nomd='%s',\
shetk='%s',\
kodop='%s',\
osnov='%s',\
dopol='%s',\
fio='%s',\
dokum='%s',\
god=%d,\
ktoi=%d,\
vrem=%ld,\
nb='%s',\
nomv='%s' \
where nomd='%s' and kassa=%s and tp=%d and god=%d",
   kassa,
   g,m,d,nomd,shetk,kodop,
   osnov,dopol,fio,
   dokum,
   g,iceb_getuid(wpredok),vrem,
   nomer_bl,
   nomv,
   nomdz,kassaz,tipz,gz);


//printf("\n%s\n",strsql);

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
   {
    return(1);
   }
  else
   {
    iceb_msql_error(&bd,"zaphkor-Ошибка записи !",strsql,wpredok);
    return(2);
   } 
 }

if(mkor == 0) //если новый документ то записей к документа еще нет
  return(0);

/*Исправляем записи в документе*/

if(d != dz || m != mz || g != gz)
 {


  //корректирум записи в которых дата документа и дата подтверждения совпадают
  sprintf(strsql,"update Kasord1 set \
datd='%d-%d-%d',\
datp='%d-%d-%d',\
god=%d,\
ktoi=%d,\
vrem=%ld \
where datd='%d-%d-%d' and datp='%d-%d-%d' and nomd='%s' and kassa=%s \
and tp=%d",
  g,m,d,g,m,d,g,iceb_getuid(wpredok),vrem,
  gz,mz,dz,gz,mz,dz,nomdz,kassaz,tipz);

//printw("\n%s\n",strsql);
//refresh();

  if(sql_zap(&bd,strsql) != 0)
    iceb_msql_error(&bd,"zaphkor-Ошибка записи !",strsql,wpredok);

  sprintf(strsql,"update Kasord1 set \
datd='%d-%d-%d',\
god=%d,\
ktoi=%d,\
vrem=%ld \
where datd='%d-%d-%d' and nomd='%s' and kassa=%s \
and tp=%d",
  g,m,d,g,iceb_getuid(wpredok),vrem,
  gz,mz,dz,nomdz,kassaz,tipz);

//printw("\n%s\n",strsql);
//refresh();

  if(sql_zap(&bd,strsql) != 0)
    iceb_msql_error(&bd,"zaphkor-Ошибка записи !",strsql,wpredok);
  


 }

if(iceb_u_SRAV(nomdz,nomd,0) != 0 || iceb_u_SRAV(kassa,kassaz,0) != 0)
 {
  sprintf(strsql,"update Kasord1 set \
nomd='%s',\
kassa=%s,\
ktoi=%d,\
vrem=%ld \
where god=%d and nomd='%s' and kassa=%s \
and tp=%d",nomd,kassa,iceb_getuid(wpredok),vrem,
  g,nomdz,kassaz,tipz);

//printw("\n%s\n",strsql);
//refresh();

  if(sql_zap(&bd,strsql) != 0)
    iceb_msql_error(&bd,"zaphkor-Ошибка записи !",strsql,wpredok);

 }

  /*Исправляем проводки в которых дата проводки совпадает с
  датой документа*/
//  printw(gettext("Проверяем проводки...\n"));
//  refresh();
  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datp='%d-%02d-%02d',\
datd='%d-%02d-%02d',\
pod=%d,\
ktoi=%d,\
vrem=%ld \
where datd='%d-%d-%d' and datp='%d-%d-%d' and nomd='%s' and \
pod=%d and kto='%s'",
  kodop,nomd,g,m,d,g,m,d,atoi(kassa),iceb_getuid(wpredok),vrem,
  gz,mz,dz,gz,mz,dz,nomdz,atoi(kassaz),gettext("КО"));
  if(sql_zap(&bd,strsql) != 0)
    iceb_msql_error(&bd,"zaphkor-Ошибка записи !",strsql,wpredok);
  /*Исправляем остальные проводки*/

  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datd='%04d-%02d-%02d',\
pod=%d,\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and nomd='%s' and \
pod=%d and kto='%s'",
kodop,nomd,g,m,d,atoi(kassa),iceb_getuid(wpredok),vrem,
gz,mz,dz,nomdz,atoi(kassaz),gettext("КО"));
  if(sql_zap(&bd,strsql) != 0)
    iceb_msql_error(&bd,"zaphkor-Ошибка записи !",strsql,wpredok);

return(0);
}
