/* $Id: zarsaldw.c,v 1.5 2011-01-13 13:49:56 sasa Exp $ */
/*03.07.2006	15.09.1998	Белых А.И.	zarsaldw.c
Получение сальдо для нужного табельного номера на 
нужную дату
*/
#include <stdlib.h>
#include        "buhg_g.h"

extern short	startgodz; /*Стаpтовый год*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern double   okrg; /*Округление*/
extern SQL_baza bd;

double zarsaldw(short dd,short mm,short gg,int tbn,
double *saldob, //Бюджетное сальдо
GtkWidget *wpredok)
{
double		saldo,saldobb;
long		kolstr;
SQL_str         row;
SQLCURSOR cur;
char		strsql[512];
double		sum;
int		i1;

//printw("Вычисляем сальдо startgodz=%d gg=%d.\n",startgodz,gg);
//refresh();		
//OSTANOV();

saldobb=saldo=0.;
*saldob=0.;

if(startgodz == 0 || startgodz > gg )
 {

  sprintf(strsql,"select saldo,saldob from Zsal where god=%d and tabn=%d",
  gg,tbn);
  if(dd == 1 && mm == 1)
   {
    sprintf(strsql,"select saldo,saldob from Zsal where god=%d and tabn=%d",
    gg-1,tbn);
   }  
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    saldo=atof(row[0]);
    saldobb=atof(row[1]);
   }
  sprintf(strsql,"select suma,shet,prn from Zarp where datz >= '%d-01-01' \
and datz < '%04d-%d-%d' and tabn=%d and suma <> 0.",gg,gg,mm,dd,tbn);

  if(dd == 1 && mm == 1)
   sprintf(strsql,"select suma,shet,prn from Zarp where datz >= '%d-01-01' \
and datz < '%04d-%d-%d' and tabn=%d and suma <> 0.",gg-1,gg,mm,dd,tbn);

 }
else
 {
  sprintf(strsql,"select saldo,saldob from Zsal where god=%d and tabn=%d",
  startgodz,tbn);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    saldo=atof(row[0]);
    saldobb=atof(row[1]);
   }  
  sprintf(strsql,"select suma,shet,prn from Zarp where datz >= '%d-01-01' \
and datz < '%d-%02d-%d' and tabn=%d and suma <> 0.",
  startgodz,gg,mm,dd,tbn);
 }

//printw("%f %f\n%s\n",saldo,saldobb,strsql);
//OSTANOV();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  if(sql_nerror(&bd) != ER_TABLEACCESS_DENIED_ERROR) //Запрет команды select  
   iceb_msql_error(&bd,"zarsald-Ошибка создания курсора!",strsql,wpredok);
  return(0.);
 }
while(cur.read_cursor(&row) != 0)
 {
  sum=atof(row[0]);
  saldo+=sum;
  if(shetbu != NULL && shetb != NULL)
   {
    if(row[2][0] == '2')
     if(iceb_u_pole1(shetbu,row[1],',',0,&i1) == 0 || iceb_u_SRAV(shetbu,row[1],0) == 0)
      saldobb+=sum;
    if(row[2][0] == '1')
     {
      if(iceb_u_pole1(shetb,row[1],',',0,&i1) == 0 || iceb_u_SRAV(shetb,row[1],0) == 0)
       saldobb+=sum;
     }
   }
 }
//printw("saldo=%f saldob=%f\n",saldo,saldobb);
//OSTANOV();

saldo=iceb_u_okrug(saldo,okrg);
*saldob=iceb_u_okrug(saldobb,okrg);
return(saldo);
}
