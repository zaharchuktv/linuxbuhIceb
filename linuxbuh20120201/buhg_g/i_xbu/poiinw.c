/* $Id: poiinw.c,v 1.6 2011-02-21 07:35:56 sasa Exp $ */
/*15.11.2007	28.07.1999	Белых А.И.	poiinw.c
Поиск в каком подразделении и у какого ответственного лица
находится инвентарный номер на заданную дату.
Если вернули 0 - найдено подразделение
             1 - Не введено основное средство
             2 - последняя запись расход то-есть он продан или списан
*/
#include <stdlib.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern SQL_baza bd;

/************************************************/

int poiinw(int in,//Инвентарный номер
const char *data, //Дата поиска
int *podr, //Код подразделения
int *kodotl, //Код ответственного лица
GtkWidget *wpredok)
{
short d=0,m=0,g=0;
iceb_u_rsdat(&d,&m,&g,data,1);
return(poiinw(in,d,m,g,podr,kodotl,wpredok));
}
/*************************/

int poiinw(int in,//Инвентарный номер
short d,short m,short g, //Дата поиска
int *podr, //Код подразделения
int *kodotl, //Код ответственного лица
GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row,row1;
SQLCURSOR curr;
char		strsql[512];
short		dd,md,gd;
int		mpz;
int		mvz;
int		tipz=0;
/*
printw("poiin %ld %d.%d.%d\n",in,d,m,g);
refresh();
*/
*podr=0;
*kodotl=0;

sprintf(strsql,"select datd,podr,nomd,tipz,kodop from Uosdok1 where \
datd <= '%04d-%d-%d' and innom=%d and podt=1 \
order by datd asc, tipz desc",g,m,d,in);


/*printf("\npoiinw-%s\n",strsql);*/


class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
if(kolstr <= 0)
  return(1);

mvz=mpz=0;
while(cur.read_cursor(&row) != 0)
 {

/*  printf("%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);*/
  
  tipz=0;
  mpz++;
  iceb_u_rsdat(&dd,&md,&gd,row[0],2);

  /*До первой записи*/
//  if(SRAV1(g,m,d,gd,md,dd) > 0 && mpz == 1)
  if(iceb_u_sravmydat(g,m,d,gd,md,dd) < 0 && mpz == 1)
   {
    mvz=1;
    break;
   }
  /*внутри записей*/
//  if(SRAV1(g,m,d,gd,md,dd) == 0)
  if(iceb_u_sravmydat(g,m,d,gd,md,dd) == 0)
   {
    mvz=1;
    break;
   }
  tipz=atoi(row[3]);
 }

if(tipz == 2)
 {
  cur.poz_cursor(kolstr-1);
  cur.read_cursor(&row);
  
  //Проверяем если это операция изменения стоимости то это не расход
  sprintf(strsql,"select vido from Uosras where kod='%s'",row[4]);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
   if(row1[0][0] != '2')
    {
     return(2);
    }
 }

/*После последнй записи*/
if(mvz == 0)
 {
  cur.poz_cursor(kolstr-1);
  cur.read_cursor(&row);
 }
*podr=atoi(row[1]);

/**************Читаем документ*/
sprintf(strsql,"select kodol from Uosdok where datd='%s' and \
nomd='%s'",row[0],row[2]);


if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
 {
  sprintf(strsql,"poiin-Не найден документ N%s от %s !",row[2],row[0]);
  iceb_menu_soob(strsql,wpredok);
 }
else
 {
  *kodotl=atoi(row1[0]);
 }


//printw("podr=%d\n",*podr);

//OSTANOV();
return(0);

}








