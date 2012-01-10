/* $Id: perecenuw.c,v 1.8 2011-02-21 07:35:55 sasa Exp $ */
/*25.11.2005	07.08.1998	Белых А.И.	perecenuw.c
Увеличение или уменьшение цен в документе на заданный процент для
услуг
Может выполняться только для расходных документов
*/
#include <stdlib.h>
#include        <pwd.h>
#include        <math.h>
#include	"buhg_g.h"

extern SQL_baza bd;
extern double   okrcn;

void perecenuw(short dd,short mm,short gg, //Дата
const char *nn, //Номер документа
int podr, //Код подразделения
int tp, //1-приход 2-расход
float pnds,
GtkWidget *wpredok)
{
double		prnc;
char		strsql[512];
long		kolstr=0;
SQL_str         row,row1;
char		naimat[512];
double		cena,cenan,cenam;
int		kodm;
short		metka;
SQLCURSOR curr;


iceb_u_str prnc1;
prnc1.plus("");
iceb_u_spisok menu;
menu.plus(gettext("Переоценка документа"));

menu.plus(gettext("Если ввести \"*\", то цены будуть взяты из списка услуг"));
menu.plus(gettext("Для увеличения цены, процент нужно вводить со знаком \"+\", или без знака"));
menu.plus(gettext("Для уменьшения цены, процент нужно вводить со знаком \"-\""));
     
menu.plus(gettext("Введите процент"));

naz:;

if(iceb_menu_vvod1(&menu,&prnc1,20,wpredok) != 0)
  return;

  
if(prnc1.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Процент не введено !"),wpredok);
  goto naz;
 }

prnc=iceb_u_atof(prnc1.ravno());

printf("Процент %s\n",prnc1.ravno());


sprintf(strsql,"select metka,kodzap,cena from Usldokum1 where \
datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
gg,mm,dd,podr,nn,tp);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;

while(cur.read_cursor(&row) != 0)
 {
  cenan=0.;
  metka=atoi(row[0]);
  kodm=atoi(row[1]);
  cena=atof(row[2]);
  
  if(metka == 0)
   {
    /*Читаем наименование материалла*/
    sprintf(strsql,"select naimat,cenapr from Material where kodm=%d",
    kodm);
    if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodm);
      iceb_menu_soob(strsql,wpredok);
       
     }
    else
     strncpy(naimat,row1[0],sizeof(naimat)-1);
   }  

  if(metka == 1)
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius,cena from Uslugi where kodus=%s",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
      {
       sprintf(strsql,"%s %s !",gettext("Не найден код услуги"),row[1]);
       iceb_menu_soob(strsql,wpredok);
      }
    else
     strncpy(naimat,row1[0],sizeof(naimat)-1);
   }  
  cenam=atof(row1[1]);

//  printw("%-3s %-20.20s %10.10g > ",row[0],naimat,cena);

  if(prnc1.ravno()[0] == '*')
   {
    cenan=cenam-cenam*pnds/(100.+pnds);
   }
  else
   {
    if(prnc >= 0.)
      cenan=cena+cena*prnc/100.;
    else
      cenan=cena+cena*prnc/(100.-prnc);
   }

  cenan=iceb_u_okrug(cenan,okrcn);

  /*Меняем цену в подтверждающей записи*/
  sprintf(strsql,"update Usldokum2 \
set \
cena=%.10g \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d",cenan,gg,mm,dd,podr,nn,metka,kodm,tp);

  iceb_sql_zapis(strsql,1,0,wpredok);

  /*Меняем цену в документе*/
  sprintf(strsql,"update Usldokum1 \
set \
cena=%.10g \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d",cenan,gg,mm,dd,podr,nn,metka,kodm,tp);

  iceb_sql_zapis(strsql,1,0,wpredok);


//  printw("%10.10g  %.2f%% %.10g\n",cenan,prnc,cenam);

//  refresh();
  
 }

}
