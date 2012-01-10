/* $Id: sumpzpdw.c,v 1.7 2011-02-21 07:35:57 sasa Exp $ */
/*10.02.2008	28.05.1998	Белых А.И.	sumpzpdw.c
Определение общей суммы в подтверждающих записях
к платежным документам
*/
#include <stdlib.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern SQL_baza bd;
extern struct REC rec;

double sumpzpdw(const char *tabl,
short *dp,short *mp,short *gp, //Дата последнего подтверждения
double *sumpp, //Сумма последнего подтверждения
GtkWidget *wpredok)
{
double		sump;
long		kolstr;
SQL_str         row;
char		strsql[512];
short		d,m,g;

*dp=*mp=*gp=0;
*sumpp=0.;
memset(strsql,'\0',sizeof(strsql));
/*Определяем сумму подтверждения документа*/
if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
 sprintf(strsql,"select datp,suma from Pzpd where tp=0 and \
datd='%d-%02d-%02d' and nomd='%s' order by datp asc",rec.gd,rec.md,rec.dd,rec.nomdk.ravno());

if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
 sprintf(strsql,"select datp,suma from Pzpd where tp=1 and \
datd='%d-%02d-%02d' and nomd='%s' order by datp asc",rec.gd,rec.md,rec.dd,rec.nomdk.ravno());

if(strsql[0] == '\0') /*Если читаем типовой документ*/
 return(0.);
 
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

if(kolstr == 0)
  return(0.);


sump=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  *dp=d; *mp=m; *gp=g;
  *sumpp=atof(row[1]);
  sump+=atof(row[1]);
 }
return(sump);
}
