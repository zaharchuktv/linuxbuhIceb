/* $Id: sumzapuosw.c,v 1.9 2011-02-21 07:35:57 sasa Exp $ */
/*15.10.2010	18.01.2001	Белых А.И.	sumzapuosw.c
Получение суммы по документа учета основных средств
*/
#include        <stdlib.h>
#include        <math.h>
#include        <ctype.h>
#include        "buhg_g.h"

extern double	okrg1;  /*Округление 1*/
extern SQL_baza bd;

void sumzapuosw(int d,int m,int g, //Дата
const char *nn,  //Номер документа
double *suma,  //сума по документа
double *sumabn, //в том числе без НДС
int lnds, //Метка льгот по НДС
int tp,  //1-приход 2-расход
GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row;
char		strsql[512];
double		bb;

*sumabn=0.;
*suma=0.;

/*Узнаем сумму всех записей*/
if(tp == 2)
  sprintf(strsql,"select cena from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s' and tipz=%d",
  g,m,d,nn,tp);
else
  sprintf(strsql,"select bs from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s' and tipz=%d",
  g,m,d,nn,tp);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  
  bb=atof(row[0]);
  bb=iceb_u_okrug(bb,okrg1);

  if(lnds > 0)
    *sumabn+=bb;
  else
    *suma+=bb;

 }

}
