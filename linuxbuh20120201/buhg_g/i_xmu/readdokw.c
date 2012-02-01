/* $Id: readdokw.c,v 1.14 2011-02-21 07:35:57 sasa Exp $ */
/*22.10.2009    16.04.1997      Белых А.И.      readdokw.c
Поиск реквизитов документа в материальном учете по номеру
Если вернули 0-нашли
	     1-нет
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include	"../headers/buhg_g.h"
#include        "l_dokmat.h"

extern char	*kodopsrvc; //Код операции расчета средневзвешенной цены
extern SQL_baza  bd;

int readdokw(class dokmat_r_data *data,GtkWidget *wpredok)
{
char		strsql[512];
SQL_str         row;
char		bros[512];
int		i;
short		d,m,g;
SQLCURSOR       cur;

//чистим все руквизиты так как может быть повторное чтение
data->clear();

if(data->dd != 0)
 sprintf(strsql,"select * from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
 data->gd,data->md,data->dd,data->skk,data->nomdok.ravno());
else /*передали только год*/
 sprintf(strsql,"select * from Dokummat \
where datd >= '%d-01-01' and datd <= '%d-12-31' and sklad=%d and \
nomd='%s'",
 data->gd,data->gd,data->skk,data->nomdok.ravno());

if(sql_readkey(&bd,strsql,&row,&cur) < 1)
 {
  iceb_u_str repl;
  repl.plus(strsql);
  repl.ps_plus(gettext("Не найден документ"));
  repl.plus(" !!!");

  sprintf(strsql,"%s:%d.%d.%d %s:%s %s:%d",
  gettext("Дата"),
  data->dd,data->md,data->gd,
  gettext("Номер документа"),
  data->nomdok.ravno(),
  gettext("Склад"),
  data->skk);
  
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);  
  
  return(1);
 }

if(data->dd == 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[1],2);
  data->dd=d;
  data->md=m;
  data->gd=g;
 } 

data->kpos.new_plus(row[3]);
data->kprr.new_plus(row[6]);
data->tipz=atoi(row[0]);
data->pro=atoi(row[8]);
data->ktoi=atoi(row[9]);
data->nomnn.new_plus(row[5]);
data->vrem_zap=atol(row[10]);
data->nomon.new_plus(row[11]);
iceb_u_rsdat(&data->dpnn,&data->mpnn,&data->gpnn,row[12],2);
data->pnds=atof(row[13]);

data->mdd=0;
if(kodopsrvc != NULL)
 if(iceb_u_SRAV(data->kprr.ravno(),kodopsrvc,0) == 0)
  data->mdd=1;

/*Для внутреннего перемещения*/
if((data->skl1=iceb_00_skl(data->kpos.ravno())) != 0)
  data->mdd=1;


/*
printw("mdd=%d data->kpos.ravno()=%s\n",data->mdd,data->kpos.ravno());
OSTANOV();
*/

/*Читаем наименование организации*/
iceb_u_str kontr;
iceb_00_kontr(data->kpos.ravno(),&kontr);

sprintf(strsql,"select naikon from Kontragent \
where kodkon='%s'",kontr.ravno_filtr());

data->naimo.new_plus("");

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не нашли код контрагента"));
  repl.plus(bros);
  repl.plus(" !!!");
  iceb_menu_soob(&repl,wpredok);  
  
 }
else
 data->naimo.new_plus(row[0]);


/*Читаем наименование операции*/
/*
printw("Читаем наименование операции.\n");
refresh();
*/
if(data->tipz == 1)
 strcpy(bros,"Prihod");
if(data->tipz == 2)
 strcpy(bros,"Rashod");
 
sprintf(strsql,"select * from %s where kod='%s'",bros,data->kprr.ravno());

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код операции"));
  repl.plus(data->kprr.ravno());
  repl.plus(" !!!");
  iceb_menu_soob(&repl,wpredok);  
 }
else
 {
  data->naimpr.new_plus(row[1]);
  if(data->tipz == 2)
   {
    data->sheta.new_plus(row[5]);
    data->metkaprov=atoi(row[6]);
   }
  else
    data->metkaprov=atoi(row[5]);
  data->mvnp=atoi(row[4]);
 }


/*Читаем наименование склада*/
/*
printw("Читаем наименование склада.\n");
refresh();
*/
sprintf(strsql,"select naik from Sklad \
where kod=%d",
data->skk);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код склада"));
  repl.plus(data->skk);
  repl.plus(" !!!");
  iceb_menu_soob(&repl,wpredok);  
 }
else
 data->naiskl.plus(row[0]);

/*Читаем основание документа*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=3",
data->gd,data->nomdok.ravno(),data->skk);

if((i=sql_readkey(&bd,strsql,&row,&cur)) == 1)
 data->osnovanie.new_plus(row[0]);

/*Читаем логин распечатавшего чек на кассе*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=5",
data->gd,data->nomdok.ravno(),data->skk);

if((i=sql_readkey(&bd,strsql,&row,&cur)) == 1)
 data->loginrash=atoi(row[0]);

if(data->tipz == 1)
 {
  /*Читаем сумму НДС введенную вручную*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=6",
  data->gd,data->nomdok.ravno(),data->skk);

  if((i=sql_readkey(&bd,strsql,&row,&cur)) == 1)
   data->sumandspr=atof(row[0]);
 }
/*Читаем метку НДС*/
/*
printw("Читаем метку НДС.\n");
refresh();
*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=11",
data->gd,data->nomdok.ravno(),data->skk);

if((i=sql_readkey(&bd,strsql,&row,&cur)) == 1)
 data->lnds=atoi(row[0]);

/*Читаем кто заблокировал*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=12",
data->gd,data->nomdok.ravno(),data->skk);

if((i=sql_readkey(&bd,strsql,&row,&cur)) == 1)
 data->blokpid=atoi(row[0]);

//Читаем cумму корректировки
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
data->gd,data->nomdok.ravno(),data->skk);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 data->sumkor=atof(row[0]);

return(0);

}
