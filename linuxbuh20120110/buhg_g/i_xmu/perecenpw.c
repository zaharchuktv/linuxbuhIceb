/*$Id: perecenpw.c,v 1.8 2011-02-21 07:35:55 sasa Exp $*/
/*11.11.2008	21.10.2004	Белых А.И.	perecenpw.c
Переоценка приходного документа на заданный процент
*/
#include <stdlib.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern SQL_baza bd;
extern char *name_system;
extern double	okrcn;

int perecenpw(short dd,short mm,short gg, //Дата
int tipz,
const char *nn, //Номер накладной
int skl, //Код склада
GtkWidget *wpredok)
{
iceb_u_str repl;
char strsql[512];
SQL_str row;
SQLCURSOR cur;
SQL_str row1;
SQLCURSOR cur1;
//Все записи в документе не должны иметь привязки к карточкам

sprintf(strsql,"select nomkar from Dokummat1 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and tipz=%d and nomkar <> 0",
gg,mm,dd,skl,nn,tipz);
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
 {
  repl.new_plus(gettext("В документе есть записи привязанные к карточкам !"));
  repl.ps_plus(gettext("Переоценка не возможна !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }


iceb_u_str prnc1;
prnc1.plus("");

iceb_u_spisok menu;
menu.plus(gettext("Переоценка документа"));

menu.plus(gettext("Для увеличения цены, процент нужно вводить со знаком \"+\", или без знака."));
menu.plus(gettext("Для уменьшения цены, процент нужно вводить со знаком \"-\"."));
     
menu.plus(gettext("Введите процент."));

naz:;

if(iceb_menu_vvod1(&menu,&prnc1,20,wpredok) != 0)
  return(1);

  
if(prnc1.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Процент не введено !"),wpredok);
  goto naz;
 }

float prnc=iceb_u_atof(prnc1.ravno());
if(prnc == 0.)
 {
  iceb_menu_soob(gettext("Процент не может быть нулевым !"),wpredok);
  goto naz;
 }


printf(gettext("Процент %s\n"),prnc1.ravno());

sprintf(strsql,"select kodm,cena from Dokummat1 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and tipz=%d order by kodm asc",
gg,mm,dd,skl,nn,tipz);
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
  return(1);
double cenan;
int kodm;
double cena;
char naimat[512];
while(cur.read_cursor(&row) != 0)
 {

  cenan=0.;
  cena=atof(row[1]);
  kodm=atoi(row[0]);
  
  /*Узнаем наименование материалла*/
  memset(naimat,'\0',sizeof(naimat));
  
  sprintf(strsql,"select naimat from Material where kodm=%d",kodm);
  
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strcpy(naimat,row1[0]);

  printf("%-3s %-20.20s %10.10g > ",
  row[0],naimat,cena);

  if(prnc >= 0.)
    cenan=cena+cena*prnc/100.;
  else
    cenan=cena+cena*prnc/(100.-prnc);

  cenan=iceb_u_okrug(cenan,okrcn);

  /*Меняем цену в документе*/
  sprintf(strsql,"update Dokummat1 \
set \
cena=%.10g \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
cenan,gg,mm,dd,skl,nn,kodm,0);

   if(sql_zap(&bd,strsql) != 0)
    iceb_msql_error(&bd,gettext("Ошибка корректировки !"),strsql,wpredok);


  printf("%10.20g  %.2f%% \n",cenan,prnc);

  
 }

return(0);
}
