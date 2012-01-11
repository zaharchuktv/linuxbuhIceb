/* $Id: zapvdokw.c,v 1.8 2011-02-21 07:35:59 sasa Exp $ */
/*11.07.2004    26.09.1997      Белых А.И.      zapvdokw.c
Запись в документ
*/
#include        <time.h>
#include	"../headers/buhg_g.h"
#include        <unistd.h>

extern SQL_baza  bd;

int  zapvdokw(short dd,short md,short gd,int skll,int nkk,
int km,const char *nnn,double kol,double cen,
const char *ei,double nds,int mnds,int voztar,int tipz,
int nomkarp,//номер карточки парного документа
const char *shet, //Счет списания для расходных документов
const char *nomz, //Номер заказа
GtkWidget *wpredok)
{
extern double	okrcn;  /*Округление цены*/
time_t 		vrem;
char		strsql[512];

cen=iceb_u_okrug(cen,okrcn);
nds=iceb_u_okrug(nds,0.01);

time(&vrem);

sprintf(strsql,"insert into Dokummat1 \
values ('%04d-%02d-%02d',%d,'%s',%d,%d,%.10g,%.10g,'%s',%d,%.10g,%d,%d,%ld,\
%d,%d,'%s','%s','%s')",
gd,md,dd,skll,nnn,nkk,km,kol,cen,ei,voztar,nds,mnds,iceb_getuid(wpredok),vrem,
tipz,nomkarp,shet,"",nomz);

if(sql_zap(&bd,strsql) != 0)
 {
  char bros[512];
  sprintf(bros,"zapvdokw-%s",gettext("Ошибка записи !"));
  iceb_msql_error(&bd,bros,strsql,wpredok);
  return(1);
 }
return(0);
}
