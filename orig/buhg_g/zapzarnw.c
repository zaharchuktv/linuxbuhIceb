/* $Id: zapzarnw.c,v 1.9 2011-02-21 07:35:59 sasa Exp $ */
/*03.07.2006    05.04.1994      Белых А.И.      zapzarnw.c
Программа записи текущей настройки на человека
*/
#include        <time.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void zapzarnw(short mz,short gz, //Дата записи
int podr, //Подразделение
int tabn, //Табельный номер
short kateg, //Категория
short sovm,  //0-не совместитель 1-совместитель
short zvan,  //Звание
const char shet[], //Счет учета
const char lgot[], //Коды льгот
short dn,short mn,short gn, //Дата начала работы
short dk,short mk,short gk, //Дата конца работы
short prov,   //0-не выполнены 1-выполнены
const char dolg[], //Должность
GtkWidget *wpredok)
{
struct  tm      *bf;
time_t          tmm;
char		strsql[512];
class iceb_u_str dolgz(dolg);

sprintf(strsql,"replace into Zarn \
values (%d,%d,%d,%d,%d,%d,%d,'%s','%s',%d,'%04d-%d-%d','%04d-%d-%d','%s',%d)",
gz,mz,podr,tabn,kateg,sovm,zvan,shet,lgot,prov,gn,mn,dn,gk,mk,dk,dolgz.ravno_filtr(),0);

//printf("zapzapnw-%s\n",strsql);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
  return;
 else
  {
   char bros[512];
   sprintf(bros,"%s-%s",__FUNCTION__,gettext("Ошибка записи !"));
   iceb_msql_error(&bd,bros,strsql,wpredok);
  }
 }
time(&tmm);
bf=localtime(&tmm);

if(gz > bf->tm_year+1900 || (gz = bf->tm_year+1900 && mz >= bf->tm_mon+1))
 {

  sprintf(strsql,"update Zarn set \
podr=%d,\
kateg=%d,\
tabn=%d,\
sovm=%d,\
zvan=%d,\
shet='%s',\
lgot='%s',\
prov=%d,\
datn='%04d-%d-%d',\
datk='%04d-%d-%d',\
dolg='%s',\
blok=%d \
  where (god > %d or (god = %d and mes > %d)) and tabn=%d",
  podr,kateg,tabn,sovm,zvan,shet,lgot,prov,gn,mn,dn,gk,mk,dk,dolgz.ravno_filtr(),0,
  bf->tm_year+1900,bf->tm_year+1900,bf->tm_mon+1,tabn);

//  printf("zapzapnw-%s\n",strsql);
  if(sql_zap(&bd,strsql) != 0)
   {
    char bros[512];
    sprintf(bros,"%s-%s",__FUNCTION__,gettext("Ошибка записи !"));
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }
 }
}
