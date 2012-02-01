/*$Id: zap_prihodw.c,v 1.8 2011-02-21 07:35:59 sasa Exp $*/
/*03.04.2006	19.02.2005	Белых А.И.	zap_prihodw.c
Запись в приходный документ материального учёта
*/
#include <stdlib.h>
#include "../headers/buhg_g.h"

extern SQL_baza bd;

void zap_prihodw(short dd,short md,short gd,
const char *nomdok,
int sklad,
int kodmat,
double kolih,
double cena,
const char *eiz,
const char *shetu,
double nds,
int vtara,
int mnds,
const char *nomzak,
const char *shet_sp, //Счёт списания
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur1;
double krt=0.;
double fas=0.;
int kodt=0;

int tipz=1;
//printw("zap_prihod %d.%d.%d %s %d\n",dd,md,gd,nomdok,sklad);

  //Пороверяем может такая карточка уже есть
int  nomkar=0;   
if(nomzak[0] == '\0')
 {        
  sprintf(strsql,"select nomk from Kart where kodm=%d and \
sklad=%d and shetu='%s' and cena=%.10g and ei='%s' and nds=%.6g \
and cenap=%.10g and krat=%.6g and mnds=%d",
  kodmat,sklad,shetu,cena,eiz,nds,0.,krt,mnds);
 }
else   //Если карточек несколько то карточка с большим номером соответствует последнему заказу
  sprintf(strsql,"select nomk from Kart where nomz='%s' order by nomk desc",nomzak);

if(iceb_sql_readkey(strsql,&row,&cur1,wpredok) >= 1)
  nomkar=atoi(row[0]);


//Проверяем может запись в документе уже есть и если есть увеличиваем количество
sprintf(strsql,"select kodm from Dokummat1 where sklad=%d and \
nomd='%s' and kodm=%d and nomkar=%d and nomz='%s'", sklad,nomdok,kodmat,nomkar,nomzak);
if(sql_readkey(&bd,strsql,&row,&cur1) >= 1)
 {
  sprintf(strsql,"update Dokummat1 set kolih=kolih+%.10g \
where sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
  kolih,sklad,nomdok,kodmat,nomkar);
  if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
     iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,wpredok);
  return;
 }        

class iceb_lock_tables lock("LOCK TABLES Kart WRITE, Dokummat1 WRITE,icebuser READ");

if(nomkar == 0)
 {
  nomkar=nomkrw(sklad,wpredok);
  if(zapkarw(sklad,nomkar,kodmat,cena,eiz,shetu,krt,fas,kodt,nds,mnds,0.,0,0,0,"","","",0,0,0,0,wpredok) != 0)
    return;
 }

zapvdokw(dd,md,gd,sklad,nomkar,kodmat,nomdok,kolih,cena,eiz,nds,mnds,vtara,tipz,0,shet_sp,nomzak,wpredok);


}
