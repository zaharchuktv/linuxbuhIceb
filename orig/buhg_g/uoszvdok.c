/*$Id: uoszvdok.c,v 1.5 2011-01-13 13:49:55 sasa Exp $*/
/*21.11.2007	21.11.2007	Белых А.И.	uoszvdok.c
Запись строки в документ 
*/

#include "buhg_g.h"
#include <unistd.h>
int uoszvdok(short dd,short md,short gd,
int tipz,
int podt,
int innom,
const char *nomd,
int podr,
int mat_ot,
int kol,
double bs,
double iz,
float kof_ind,
const char *kod_op,
double bsby,
double izby,
double cena,
const char *shetsp,
GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"insert into Uosdok1 \
values ('%04d-%02d-%02d',%d,%d,%d,'%s',%d,%d,%d,%.2f,%.2f,%.2f,'%s',%d,%ld,%.2f,%.2f,%.2f,'%s')",
gd,md,dd,
tipz,
podt,
innom,
nomd,
podr,
mat_ot,
kol,
bs,
iz,
kof_ind,
kod_op,
iceb_getuid(wpredok),
time(NULL),
bsby,
izby,
cena,
shetsp);

return(iceb_sql_zapis(strsql,1,0,wpredok));

}

