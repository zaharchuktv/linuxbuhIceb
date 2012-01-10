/*$Id: zarpvw.c,v 1.5 2011-02-21 07:36:00 sasa Exp $*/
/*29.03.2010	29.03.2010	Белых А.И.	zarpv.c
Поиск всех выплат сделанных в счёт месяца расчёта и предыдущих месяцев
Применяется для автоматической розноски выплаченной зарплаты с учётом сальдо
*/


#include <stdlib.h>
#include        "buhg_g.h"

extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short	*obud; /*Обязательные удержания*/
extern SQL_baza bd;


double zarpv(short mr,short gr,int tabnom,
double *sumab, //Сумма бюджетных удержаний
GtkWidget *wpredok)
{
double          sum=0.;
long		kolstr;
SQL_str         row;
char		strsql[512];

*sumab=0.;

sprintf(strsql,"select knah,suma,shet from Zarp where tabn=%d and \
datz > '%d-%02d-31' and (godn < %d or (godn = %d and mesn <= %d)) and prn='2' and suma <> 0.)",
tabnom,gr,mr,gr,gr,mr);

class SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
//printw("kolstr=%d\n",kolstr);

if(kolstr == 0)
  return(0.);

while(cur.read_cursor(&row) != 0)
 {
  //Если это обязательные удержания то это не выплата зарплаты
  if(provkodw(obud,atoi(row[0])) >= 0) 
   continue;
   
  sum+=atof(row[1]);

  
  if(iceb_u_proverka(shetbu,row[2],0,0) == 0)  
     *sumab=*sumab+atof(row[1]);;

 }

if(sum != 0.)
 sum=iceb_u_okrug(sum,0.01);
 
return(sum);

}
