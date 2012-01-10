/*$Id: skopluslw.c,v 1.4 2011-01-13 13:49:53 sasa Exp $*/
/*13.10.2009	12.10.2009	Белых А.И.	skopluslw.c
Сумма к оплате по документа учёта услуг
*/
#include <stdlib.h>
#include "buhg_g.h"
extern double okrg1;

double skopluslw(short dd,short md,short gd, //Дата документа
const char *nomdok, //номер документа
int podr,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
double suma=0.;
double sumabn=0.;
int lnds=0;
int tipz=0;
float pnds=0.;

sprintf(strsql,"select tp,nds,pn from Usldokum where datd='%04d-%02d-%02d' and podr=%d and nomd='%s'",
gd,md,dd,podr,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 return(0.);

tipz=atoi(row[0]);
lnds=atoi(row[1]);
pnds=atof(row[2]);
 
sumzap1uw(dd,md,gd,nomdok,podr,&suma,&sumabn,lnds,tipz,wpredok);

double nds=suma*pnds/100.;
double suma_dok=suma+sumabn+nds;
suma_dok=iceb_u_okrug(suma_dok,okrg1);

return(suma_dok);


}
