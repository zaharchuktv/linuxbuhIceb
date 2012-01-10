/*$Id: zapvreew.c,v 1.9 2011-03-28 06:55:46 sasa Exp $*/
/*22.06.2010	30.08.2005	Белых А.И. 	zapvreew.c
Запись в реестр выданных налоговых накладных
*/
#include <stdlib.h>
#include "buhg_g.h"
#include <unistd.h>


int zapvreew(short dd,short md,short gd,
const char *nnn,
double sumd,
double snds,
int mt,
const char *inn,
const char *nkontr,
int mi,
const char *datdi,
const char *nomdi,
const char *koment,
const char *kodgr,
const char *kodop,
const char *viddok,
const char *datv,
GtkWidget *wpredok)
{
char strsql[1024];

class iceb_u_str nnn_f(nnn);

class iceb_u_str nomdi_f(nomdi);

class iceb_u_str nkontr_f(nkontr);

class iceb_u_str koment_f(koment);

class iceb_u_str kodgr_f(kodgr);
class iceb_u_str kodop_f(kodop);

int dnz=0; 
for(dnz=1; dnz < 99999999; dnz++)
 {
  sprintf(strsql,"select dnz from Reenn where datd='%04d-%02d-%02d' and dnz=%d",gd,md,dd,dnz);
  if(iceb_sql_readkey(strsql,wpredok) <= 0)
   break;
 }

sprintf(strsql,"insert into Reenn (datd,nnn,sumd,snds,mt,inn,nkontr,mi,datdi,nomdi,\
koment,ktoi,vrem,gr,ko,vd,datv,dnz,nnni) values ('%04d-%02d-%02d','%s',%.2f,%.2f,%d,'%s','%s',%d,'%s','%s',\
'%s',%d,%ld,'%s','%s','%s','%s',%d,%d)",
gd,md,dd,
nnn_f.ravno_filtr(),
sumd,
snds,
mt,
inn,
nkontr_f.ravno_filtr(),
mi,
datdi,
nomdi_f.ravno_filtr(),
koment_f.ravno_filtr(),
iceb_getuid(wpredok),
time(NULL),
kodgr_f.ravno_filtr(),
kodop_f.ravno_filtr(),
viddok,
datv,
dnz,
atoi(nnn));

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return(1);

return(0);

}
