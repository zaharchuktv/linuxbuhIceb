/* $Id: zapkarw.c,v 1.7 2011-01-13 13:49:55 sasa Exp $ */
/*03.04.2006    26.09.1997      Белых А.И.      zapkarw.c
Запись новой карточки в базу
Если вернули 0 - записали
	     1 - такая запись уже есть - не записали
*/

#include        <pwd.h>
#include        <time.h>
#include	"../headers/buhg_g.h"
#include        <unistd.h>

extern SQL_baza  bd;

short zapkarw(int skll,int  nkk,int km,double ccn,const char *eii,
const char *shu,double krt,double fas,int kodt,double nds,int mnds,
double cenav,
short denv,short mesv,short godv, //Дата ввода в эксплуатацию (для малоценки)
const char *innom, //Инвентарный номер (для малоценки)
const char *rnd, //Регистрационный номер для мед. препарата
const char *nomz, //Номер заказа
short deng,short mesg,short godg, //Дата конечного использования
int mv, //0-ввести новую 1-исправить старую
GtkWidget *wpredok)
{
char		strsql[512];
extern double	okrcn;  /*Округление цены*/
time_t		vrem;

ccn=iceb_u_okrug(ccn,okrcn);
nds=iceb_u_okrug(nds,0.01);
krt=iceb_u_okrug(krt,0.000001);
fas=iceb_u_okrug(fas,0.000001);
cenav=iceb_u_okrug(cenav,okrcn);
time(&vrem);

if(mv == 0)
  sprintf(strsql,"insert into Kart \
values (%d,%d,%d,%d,'%s','%s',%.10g,%.10g,%.10g,%.10g,%.10g,%d,%d,%ld,\
'%04d-%d-%d','%s','%s','%s','%04d-%02d-%02d')",
skll,nkk,km,mnds,eii,shu,ccn,cenav,krt,nds,fas,kodt,iceb_getuid(wpredok),vrem,
godv,mesv,denv,innom,rnd,nomz,godg,mesg,deng);

if(mv == 1)
  sprintf(strsql,"update Kart \
set \
sklad=%d,\
nomk=%d,\
kodm=%d,\
mnds=%d,\
ei='%s',\
shetu='%s',\
cena=%.10g,\
cenap=%.10g,\
krat=%.10g,\
nds=%.10g,\
fas=%.10g,\
kodt=%d,\
ktoi=%d,\
vrem= %ld,\
datv='%04d-%d-%d',\
innom='%s',\
rnd='%s',\
nomz='%s',\
dat_god='%04d-%02d-%02d' \
where sklad=%d and nomk=%d",
skll,nkk,km,mnds,eii,shu,ccn,cenav,krt,nds,fas,kodt,iceb_getuid(wpredok),vrem,
godv,mesv,denv,innom,rnd,nomz,godg,mesg,deng,
skll,nkk);

//printf("\n%s\n",strsql);

if(sql_zap(&bd,strsql) != 0)
 {
  iceb_msql_error(&bd,"zapkarw",strsql,wpredok);
  return(1);
 }

return(0);
}
