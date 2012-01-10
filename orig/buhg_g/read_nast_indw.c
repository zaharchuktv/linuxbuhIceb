/*$Id: read_nast_indw.c,v 1.9 2011-04-06 09:49:17 sasa Exp $*/
/*04.04.2011	30.01.2009	Белых А.И.	read_nast_indw.c
Чтение таблицы для расчёта индексации зарплаты
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "read_nast_indw.h"

extern SQL_baza bd;

int read_nast_ind(short mr,short gr,class index_ua *data,GtkWidget *wpredok)
{
data->free_class();
short mi,gi;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zarindex.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],strsql,sizeof(strsql),1,'|') != 0)
    continue;

  if(iceb_u_SRAV("Дата индексации",strsql,0) == 0)
   {
    iceb_u_polen(row_alx[0],strsql,sizeof(strsql),2,'|');
    if(iceb_u_SRAV("Текущая",strsql,0) == 0)
     {
      mi=mr;
      gi=gr;
     }
    else
     if(iceb_u_rsdat1(&mi,&gi,strsql) != 0)
       continue;
    data->mr.plus(mi,-1);
    data->gr.plus(gi,-1);
//    iceb_u_polen(row_alx[0],strsql,sizeof(strsql),3,'|');
//    data->prog_min.plus(atof(strsql),-1);
    continue;
   }

  if(atoi(strsql) <= 0) //Значит первое поле не цифровое
   continue;

  if(iceb_u_rsdat1(&mi,&gi,strsql) != 0)
     continue;
  data->mi.plus(mi,-1);
  data->gi.plus(gi,-1);

  iceb_u_polen(row_alx[0],strsql,sizeof(strsql),2,'|');
  data->koef.plus(atof(strsql),-1);
 }


/*Читаем таблицу для индексации зарплаты выплаченной с задержкой*/
sprintf(strsql,"select str from Alx where fil='zarindexv.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],strsql,sizeof(strsql),1,'|') != 0)
    continue;

  if(iceb_u_SRAV("Код начисления индексации невовремя выплаченной зарплаты",strsql,0) == 0)
   {
    iceb_u_polen(row_alx[0],&data->kninvz,2,'|');
    continue;
   }

  if(atoi(strsql) <= 0) //Значит первое поле не цифровое
   continue;

  if(iceb_u_rsdat1(&mi,&gi,strsql) != 0)
   continue;

  data->miv.plus(mi,-1);
  data->giv.plus(gi,-1);

  iceb_u_polen(row_alx[0],strsql,sizeof(strsql),2,'|');
  data->koefv.plus(atof(strsql),-1);

 }

if(iceb_poldan("Код перечисления зарплаты на счет",strsql,"zarnast.alx",wpredok) == 0)
 {
  data->kodpzns=atoi(strsql);
 }

if(iceb_poldan("Код получения зарплаты в кассе",strsql,"zarnast.alx",wpredok) == 0)
 {
  data->kodpzvk=atoi(strsql);
 }


return(0);

}
