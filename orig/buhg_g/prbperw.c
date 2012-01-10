/* $Id: prbperw.c,v 1.4 2011-01-13 13:49:52 sasa Exp $ */
/*28.12.2006    27.12.1996	Белых А.И.	prbperw.c
Ищем все удержания расчетного месяца исключая расчетный месяц
*/
#include <stdlib.h>
#include        "buhg_g.h"

extern char	*shetbu; /*Бюджетные счета удержаний*/
extern SQL_baza bd;
extern short	*obud; /*Обязательные удержания*/

double prbperw(short mn,short gn,short mk,short gk,int tabb,
double *sumab, //Сумма бюджетных удержаний
int metka, //0-смотреть все удержания до даты и после даты 1-только после даты
GtkWidget *wpredok)
{
double          sum=0.;
long		kolstr;
SQL_str         row;
char		strsql[512];

*sumab=0.;

if(metka == 0)
  sprintf(strsql,"select knah,suma,shet from Zarp where tabn=%d and \
godn=%d and mesn=%d and (datz < '%d-%d-01' or datz > '%d-%02d-31') \
and datz <= '%d-%d-31' and prn='2' and suma <> 0.",
  tabb,gn,mn,gn,mn,gn,mn,gk,mk);
if(metka == 1)
  sprintf(strsql,"select knah,suma,shet from Zarp where tabn=%d and \
godn=%d and mesn=%d and  datz > '%d-%02d-31' \
and datz <= '%d-%d-31' and prn='2' and suma <> 0.",
  tabb,gn,mn,gn,mn,gk,mk);

class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"prbper",strsql,wpredok);
  return(0.);
 }
if(kolstr == 0)
  return(0.);

while(cur.read_cursor(&row) != 0)
 {
  //Если это обязательное удержание то это не выплата зарплаты
  if(provkodw(obud,atoi(row[0])) >= 0) 
   continue;
 
  sum+=atof(row[1]);

//  if(shetbu != NULL)
//   if(iceb_u_pole1(shetbu,row[1],',',0,&i1) == 0 || iceb_u_SRAV(shetbu,row[1],0) == 0)
  if(iceb_u_proverka(shetbu,row[2],0,0) == 0)
     *sumab=*sumab+atof(row[1]);;

 }

if(sum != 0.)
 sum=iceb_u_okrug(sum,0.01);
 
return(sum);


}
