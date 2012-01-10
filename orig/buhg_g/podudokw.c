/* $Id: podudokw.c,v 1.10 2011-02-21 07:35:55 sasa Exp $ */
/*29.09.2009	15.05.2000	Белых А.И.	podudokw.c
Проверка подтверждения документа для услуг
*/
#include <stdlib.h>
#include        <math.h>
#include	"buhg_g.h"

extern SQL_baza bd;

void podudokw(short d,short m,short g,const char *nn,int podr,
int tipz,GtkWidget *wpredok)
{
double          kol1,kol2;
short           mz; /*mz=0 подтверждена 1-нет*/
SQL_str         row;
char		strsql[512];
int		kolstr;
int		pod;
int		metka;
int		kodzap;

mz=0;

sprintf(strsql,"select metka,kodzap,kolih,shetu,nz from Usldokum1 where \
datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
g,m,d,podr,nn,tipz);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

  
kol1=0.;
while(cur.read_cursor(&row) != 0)
 {
  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kol1=atof(row[2]);

  /*Читаем в карточке количество реально выданное-полученное*/
  kol2=readkoluw(d,m,g,podr,nn,tipz,metka,kodzap,atoi(row[4]),wpredok);

  if(fabs(kol1-kol2) > 0.00001)
   {
    mz=1;
    break;
   }

 }

if(kol1 == 0.)
 mz=1;

/*
printw("\nmz=%d kol1=%.2f kol2=%.2f\n",mz,kol1,kol2);
OSTANOV();
  */

pod=0;  //Не подтвержден
if(mz == 0) /*Подтверждена*/
  pod=1; //Подтвержден

//Если дата заблокирована то не устанавливаем метку не подтвержденного документа
if(pod == 0)
 {
  if(iceb_pblok(m,g,ICEB_PS_UU,wpredok) != 0) //Дата заблокирована
    return;
 }

sprintf(strsql,"update Usldokum \
set \
pod=%d \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
pod,g,m,d,podr,nn,tipz);
/*
printw("\n%s\n",strsql);
OSTANOV();
*/
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  {
   char bros[512];
   sprintf(bros,"podudok-%s",gettext("Ошибка корректировки записи !"));
   iceb_msql_error(&bd,bros,strsql,wpredok);
  }

}
