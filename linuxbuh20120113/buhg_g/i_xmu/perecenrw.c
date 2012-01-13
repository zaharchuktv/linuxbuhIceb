/*$Id: perecenrw.c,v 1.9 2011-02-21 07:35:55 sasa Exp $*/
/*01.09.2005	22.10.2004	Белых А.И.	perecenrw.c
Переоценка расходного документа
*/
#include <stdlib.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern SQL_baza bd;
extern char *name_system;
extern double	okrcn;

int perecenrw(short dd,short mm,short gg, //Дата
int tipz,
const char *nn, //Номер накладной
int skl, //Код склада
float pnds,
GtkWidget *wpredok)
{
iceb_u_str repl;
char strsql[512];
SQL_str row;
SQLCURSOR cur;
SQL_str row1;
SQLCURSOR cur1;


iceb_u_str prnc1;
prnc1.plus("");
iceb_u_spisok menu;
menu.plus(gettext("Переоценка документа"));

menu.plus(gettext("При 0 проценте переоценивается по учетным ценам"));
menu.plus(gettext("Если ввести \"*\", то цены будуть взяты из списка материалов"));
menu.plus(gettext("Для увеличения цены, процент нужно вводить со знаком \"+\", или без знака"));
menu.plus(gettext("Для уменьшения цены, процент нужно вводить со знаком \"-\""));
     
menu.plus(gettext("Введите процент"));

naz:;

if(iceb_menu_vvod1(&menu,&prnc1,20,wpredok) != 0)
  return(1);

  
if(prnc1.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Процент не введено !"),wpredok);
  goto naz;
 }

float prnc=iceb_u_atof(prnc1.ravno());

printf("Процент %s\n",prnc1.ravno());

sprintf(strsql,"select kodm,cena,nomkar from Dokummat1 where \
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
int nk;
double cenam;
while(cur.read_cursor(&row) != 0)
 {
  cenan=0.;
  cena=atof(row[1]);
  kodm=atoi(row[0]);
  nk=atoi(row[2]);
  
  /*Узнаем наименование материалла*/
  sprintf(strsql,"select * from Material where kodm=%d",kodm);
  cenam=0.;
  memset(naimat,'\0',sizeof(naimat));
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("%s %d !\n","Не найден код материалла",kodm);
   }
  else
   {
    cenam=atof(row1[6]);
    strcpy(naimat,row1[2]);
   }
  printf("%-3s %d %-20.20s %10.10g > ",
  row[0],nk,naimat,cena);
  if(prnc1.ravno()[0] == '0' || prnc1.ravno()[0] == '*')
   {
    if(prnc1.ravno()[0] == '0')
     {
      sprintf(strsql,"select cena from Kart where sklad=%d and nomk=%d",
      skl,nk);
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        printf("Не нашли карточки ! Склад %d Номер карточки %d\n",
        skl,nk);
        continue;
       }
      cenan=atof(row1[0]);  
     }
    if(prnc1.ravno()[0] == '*')
     {  
      cenan=cenam-cenam*pnds/(100.+pnds);
     }
   }
  else
   {
    if(prnc >= 0.)
      cenan=cena+cena*prnc/100.;
    else
      cenan=cena+cena*prnc/(100.-prnc);
   }

  cenan=iceb_u_okrug(cenan,okrcn);

  /*Меняем цену в карточках*/
  if(nk != 0)
   {
    sprintf(strsql,"update Zkart \
set \
cena=%.10g \
where datd='%d-%02d-%02d' and nomd='%s' and sklad=%d \
and nomk=%d",cenan,gg,mm,dd,nn,skl,nk);

    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка корректировки !"),strsql,wpredok);
   }
  /*Меняем цену в документе*/
  sprintf(strsql,"update Dokummat1 \
set \
cena=%.10g \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
cenan,gg,mm,dd,skl,nn,kodm,nk);

   if(sql_zap(&bd,strsql) != 0)
    iceb_msql_error(&bd,gettext("Ошибка корректировки !"),strsql,wpredok);


  printf("%10.10g  %.2f%% %.10g\n",cenan,prnc,cenam);

  
 }
return(0);
}
