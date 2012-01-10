/*$Id: usl_spismat.c,v 1.10 2011-02-21 07:35:58 sasa Exp $*/
/*23.11.2005	23.11.2005	Белых А.И.	usl_spismat.c
Списание материаллов введеных в услугах в подсистеме материальный учёт
*/
#include "buhg_g.h"
#include "usl_spismat.h"

int   usl_spismat_m(class usl_spismat_rek *rek_ras,GtkWidget *wpredok);
void usl_spismat_r(class usl_spismat_rek *rek_ras,GtkWidget *wpredok);


void usl_spismat(short dd,short md,short gd,
const char *nomdok,
int podr,
int tipz,
const char *kontr,
int lnds,
float pnds,
GtkWidget *wpredok)
{
char strsql[512];

//Проверяем документ на наличие материалов
sprintf(strsql,"select tp from Usldokum1 where datd='%04d-%02d-%02d' \
and podr=%d and nomd='%s' and tp=%d and metka=0 limit 1",gd,md,dd,podr,nomdok,tipz);
if(iceb_sql_readkey(strsql,wpredok) <= 0)
 {
  iceb_menu_soob(gettext("В документ не введено ни одного материалла !"),wpredok);
  return;
 }

SQL_str row;
SQLCURSOR cur;
char osnov[80];
sprintf(osnov,"USL%02d.%02d.%d N%s",dd,md,gd,nomdok);

//Проверяем может уже материалы списаны
sprintf(strsql,"select sklad,nomd from Dokummat2 where god=%d and nomerz=2 and sodz='%s'",
gd,osnov);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Материалы уже списаны."));

  sprintf(strsql,"%s %s",gettext("Номер документа"),row[1]);
  repl.ps_plus(strsql);
  
  sprintf(strsql,"%s %s",gettext("Склад"),row[0]);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return;

 }


class usl_spismat_rek data;
data.dd=dd;
data.md=md;
data.gd=gd;
data.nomdoku.new_plus(nomdok);
data.podr=podr;
data.tipz=tipz;
data.kontr.new_plus(kontr);
data.lnds=lnds;
data.pnds=pnds;

if(usl_spismat_m(&data,wpredok) != 0)
 return;

usl_spismat_r(&data,wpredok);


}
