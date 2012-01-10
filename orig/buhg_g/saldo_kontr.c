/* $Id: saldo_kontr.c,v 1.7 2011-02-21 07:35:57 sasa Exp $ */
/*16.11.2009    07.11.2005      Белых А.И.      saldo_kontr.c
Получение сальдо по оpганизации и счету
*/
#include        <stdlib.h>
#include        "buhg_g.h"


extern char	*organ;
extern short	startgodb; /*Стартовый год*/
extern SQL_baza bd;

void saldo_kontr(char *sh, //Счет
char *kor,  //Код контрагента
double *db,  //Дебет
double *kr,  //Дебет
short dn,short mn,short gn, //Дата начала
short dk,short mk,short gk, //Дата конца
GtkWidget *wpredok)
{
short           d,m,g;
char		shh[32];
double          ddn,kkn;
double		dd,kk;
short           msv; /*0-без сверки 1-со сверкой*/
short           mvp; /*Метка второго прохода*/
char		strsql[512];
SQL_str         row;
long		kolstr;
short		godn;
short		shett=0;
double		deb=0.,kre=0.;

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

*db=0.;
*kr=0.;
mvp=msv=0;

strcpy(shh,sh);

ddn=kkn=0.;
dd=kk=0.;
sprintf(strsql,"select deb,kre from Saldo where kkk='1' and gs=%d and \
ns='%s' and kodkon='%s'",godn,shh,kor);
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  ddn=atof(row[0]);
  kkn=atof(row[1]);
 }

g=godn;
m=1;
d=1;
while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
 {

   sprintf(strsql,"select * from Prov \
where val=0 and datp='%d-%02d-%02d' and sh='%s' and kodkon='%s'",
   g,m,d,shh,kor);

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
      iceb_u_dpm(&d,&m,&g,1);
      continue;
     }
   shett=0;
   while(cur.read_cursor(&row) != 0)
    {

     deb=atof(row[9]);
     kre=atof(row[10]);

     if(iceb_u_sravmydat(g,m,d,gn,mn,dn) >= 0)
     if(iceb_u_sravmydat(gk,mk,dk,g,m,d) >= 0)
      {
       dd+=deb;
       kk+=kre;

      }
     if(iceb_u_sravmydat(g,m,d,gn,mn,dn) < 0)
      {
       ddn+=deb;
       kkn+=kre;
      }

    }
  iceb_u_dpm(&d,&m,&g,1);
 }

*db+=dd+ddn;
*kr+=kk+kkn;

}
