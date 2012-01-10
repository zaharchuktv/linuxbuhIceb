/* $Id: zvaniew.c,v 1.12 2011-02-21 07:36:00 sasa Exp $ */
/*22.12.2010    02.12.1992      Белых А.И.      zvaniew.c
Подпрограмма начисления за звание
*/
#include <stdlib.h>
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void zvaniew(int tn,//Табельный номер
short mp,short gp, //Дата расчета
int podr, //Код подразделения
const char *nah_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
char            br[112];
int             kat,kat1;
int             knzz; /*Код начисления за звание*/
double          nah;
short           d;
char		shet[32];
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];
int		zvan;
class ZARP     zp;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
nah=0.;

if(ff_prot != NULL)
 fprintf(ff_prot,"\nРасчёт начисления за завание\n\
----------------------------------------------------------\n");
 
sprintf(strsql,"select str from Alx where fil='zarzvanie.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки zarzvanie.alx\n");
  return;
 }

iceb_poldan("Код начисления за звание",strsql,"zarzvanie.alx",wpredok);
knzz=atoi(strsql);
if(iceb_u_proverka(nah_only,knzz,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код %d исключён из расчёта\n",knzz);
  return;
 }
 
/*Проверяем есть ли код начисления в списке*/
memset(shet,'\0',sizeof(shet));
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tn,knzz); 
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не нашли код начисления за звание %d\n",knzz);
  return;
 }
strncpy(shet,row[0],sizeof(shet));
//Читаем код звания
sprintf(strsql,"select zvan from Kartb where tabn=%d",tn); 
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден код звания для %d\n",tn);
  return;
 }
zvan=atoi(row[0]);


kat1=0;
nah=0.;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
  kat=(int)iceb_u_atof(br);
  if(kat == 0)
   continue;
  if(kat == zvan)
   {
    iceb_u_polen(row_alx[0],br,sizeof(br),3,'|');
    nah=iceb_u_atof(br);
    kat1=kat;
    break;
   }

 }
if(kat1 == 0)
 return;

iceb_u_dpm(&d,&mp,&gp,5);

zp.tabnom=tn;
zp.prn=1;
zp.knu=knzz;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
strncpy(zp.shet,shet,sizeof(zp.shet)-1);

zapzarpw(&zp,nah,d,mp,gp,0,shet,"",0,podr,"",wpredok);
}
