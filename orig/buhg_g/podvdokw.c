/* $Id: podvdokw.c,v 1.9 2011-02-21 07:35:55 sasa Exp $ */
/*29.09.2009    23.03.1994      Белых А.И.      podvdokw.c
Просматриваем соотвецтвует ли количесво в карточках
количеству материалла по записи в накладных
Если нет то делаем запись в список не закрытых накладных
Если да то снимаем запись из списка не закрытых накладных
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <math.h>
#include        <string.h>
#include	"buhg_g.h"

extern SQL_baza	bd;

void podvdokw(short d,short m,short g,const char *nn,int skll,GtkWidget *wpredok)
{
double          kol1,kol2;
short           mz; /*mz=0 подтверждена 1-нет*/
short		dd,mm,gg;
SQL_str         row;
char		strsql[512];
int		kolstr;
int		pod,nk;



mz=0;

sprintf(strsql,"select datd,nomkar,kolih from Dokummat1 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
g,m,d,skll,nn);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

  
kol1=0.;
while(cur.read_cursor(&row) != 0)
 {

  kol1=atof(row[2]);
  nk=atoi(row[1]);
  /*Читаем в карточке количество реально выданное-полученное*/
  iceb_u_rsdat(&dd,&mm,&gg,row[0],2);
  kol2=readkolkw(skll,nk,dd,mm,gg,nn,wpredok);

  if(fabs(kol1-kol2) > 0.00001)
   {
    mz=1;
    break;
   }

 }

if(kol1 == 0.)
 mz=1;


pod=0; //Не подтвержден
if(mz == 0) /*Подтверждена*/
  pod=1; //Подтвержден

//Если дата заблокирована то не устанавливаем метку не подтвержденного документа
if(pod == 0)
 {
  if(iceb_pblok(m,g,ICEB_PS_MU,wpredok) != 0) //Дата заблокирована
    return;
 }

sprintf(strsql,"update Dokummat \
set \
pod=%d \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
pod,g,m,d,skll,nn);

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  iceb_msql_error(&bd,"podvdokw-Ошибка корректировки записи !",strsql,wpredok);

}
