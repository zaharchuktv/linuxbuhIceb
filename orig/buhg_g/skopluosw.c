/*$Id: skopluosw.c,v 1.3 2011-01-13 13:49:53 sasa Exp $*/
/*13.10.2009	12.10.2009	Белых А.И.	skopluosw.c
Получение суммы к оплате для документа учёта основных средств
*/
#include <stdlib.h>
#include "buhg_g.h"
extern double okrg1;

double skopluosw(short dd,short md,short gd, //Дата документа
const char *nomdok, //номер документа
GtkWidget *wpredok)
{
char strsql[512];
int lnds=0;
int tipz=0;
double suma=0.;
double sumabn=0.;
SQL_str row;
class SQLCURSOR cur;
double pnds=0.;
/*узнаём НДС*/

sprintf(strsql,"select tipz,nds,pn from Uosdok where datd='%04d-%02d-%02d' and nomd='%s'",gd,md,dd,nomdok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 return(0.);
tipz=atoi(row[0]);
lnds=atoi(row[1]);
pnds=atof(row[2]);
 
sumzapuosw(dd,md,gd,nomdok,&suma,&sumabn,lnds,tipz,wpredok);



double nds=suma*pnds/100.;
double suma_dok=suma+sumabn+nds;

suma_dok=iceb_u_okrug(suma_dok,okrg1);

return(suma_dok);
}
