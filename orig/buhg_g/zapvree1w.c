/*$Id: zapvree1w.c,v 1.6 2011-01-13 13:49:55 sasa Exp $*/
/*22.12.2009	30.08.2005	Белых А.И. 	zapvree1w.c
Запись в реестр полученных налоговых накладных
*/
#include "buhg_g.h"
#include <unistd.h>


int zapvree1w(short dd,short md,short gd,
const char *nnn,
double sumd1,
double snds1,
double sumd2,
double snds2,
double sumd3,
double snds3,
double sumd4,
double snds4,
const char *inn,
const char *nkontr,
int mi,
const char *datdi,
const char *nomdi,
const char *koment,
const char *kodgr,
const char *kodop,
short dv,short mv,short gv,
const char *viddok,
GtkWidget *wpredok)
{
char strsql[1024];

class iceb_u_str nnn_f(nnn);

class iceb_u_str nomdi_f(nomdi);

class iceb_u_str nkontr_f(nkontr);

class iceb_u_str koment_f(koment);

class iceb_u_str kodgr_f(kodgr);
class iceb_u_str kodop_f(kodop);

sprintf(strsql,"insert into Reenn1 (datd,nnn,\
sumd1,snds1,\
sumd2,snds2,\
sumd3,snds3,\
sumd4,snds4,\
inn,nkontr,mi,datdi,nomdi,\
koment,gr,ktoi,vrem,ko,dvd,vd) values ('%04d-%02d-%02d','%s',%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,\
'%s','%s',%d,'%s','%s','%s','%s',%d,%ld,'%s','%04d-%02d-%02d','%s')",
gd,md,dd,
nnn_f.ravno_filtr(),
sumd1,
snds1,
sumd2,
snds2,
sumd3,
snds3,
sumd4,
snds4,
inn,
nkontr_f.ravno_filtr(),
mi,
datdi,
nomdi_f.ravno_filtr(),
koment_f.ravno_filtr(),
kodgr_f.ravno_filtr(),
iceb_getuid(wpredok),
time(NULL),
kodop_f.ravno_filtr(),
gv,mv,dv,
viddok);

//printw("zapvree1-%s\n",strsql);
//OSTANOV();

if(iceb_sql_zapis(strsql,0,0,wpredok) != 0)
 return(1);

return(0);

}
