/*$Id: vzrkfw.c,v 1.7 2011-02-21 07:35:58 sasa Exp $*/
/*14.12.2010	03.04.2006	Белых А.И.	vzrkfw.c
Чтение контрагентов из файла
*/
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "dok4w.h"

extern class REC rec;
extern SQL_baza bd;

void vzrkfw(GtkWidget *wpredok)
{
char		bros[1024];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"platpor.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

for(int nom=0; nom < 6; nom++)
  cur_alx.read_cursor(&row_alx);

if(iceb_u_pole(row_alx[0],bros,2,'|') != 0)
 {
  iceb_menu_soob(gettext("Файл организаций искажен !!!\nАварийное завершение.\n"),wpredok);
  return;
 }
rec.naior.new_plus(bros);

iceb_u_pole(row_alx[0],bros,3,'|');
rec.gorod.new_plus(bros);

iceb_u_pole(row_alx[0],bros,4,'|');
rec.kod.new_plus(bros);

iceb_u_pole(row_alx[0],bros,5,'|');
rec.naiban.new_plus(bros);

iceb_u_pole(row_alx[0],bros,6,'|');
rec.mfo.new_plus(bros);

iceb_u_pole(row_alx[0],bros,7,'|');
rec.nsh.new_plus(bros);

iceb_u_pole(row_alx[0],bros,8,'|');
rec.kodor.new_plus(bros);

iceb_u_polen(row_alx[0],bros,sizeof(bros),9,'|');
rec.rsnds.new_plus(bros);
short metka=0;

/*Смотрим может есть в строке "-" если есть берем его*/
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(row_alx[0][0] == '-')
   {
    iceb_u_pole(row_alx[0],bros,2,'|');
    rec.naior.new_plus(bros);
    
    iceb_u_pole(row_alx[0],bros,3,'|');
    rec.gorod.new_plus(bros);

    iceb_u_pole(row_alx[0],bros,4,'|');
    rec.kod.new_plus(bros);

    iceb_u_pole(row_alx[0],bros,5,'|');
    rec.naiban.new_plus(bros);

    iceb_u_pole(row_alx[0],bros,6,'|');
    rec.mfo.new_plus(bros);

    iceb_u_pole(row_alx[0],bros,7,'|');
    rec.nsh.new_plus(bros);

    iceb_u_pole(row_alx[0],bros,8,'|');
    rec.kodor.new_plus(bros);

    iceb_u_polen(row_alx[0],bros,sizeof(bros),9,'|');
    rec.rsnds.new_plus(bros);

    metka++;
   }

  if(row_alx[0][0] == '+')
   {
    iceb_u_pole(row_alx[0],bros,2,'|');
    rec.naior1.new_plus(bros);
    
    iceb_u_pole(row_alx[0],bros,3,'|');
    rec.gorod1.new_plus(bros);

    iceb_u_pole(row_alx[0],bros,4,'|');
    rec.kod1.new_plus(bros);

    iceb_u_pole(row_alx[0],bros,5,'|');
    rec.naiban1.new_plus(bros);

    iceb_u_pole(row_alx[0],bros,6,'|');
    rec.mfo1.new_plus(bros);

    iceb_u_pole(row_alx[0],bros,7,'|');
    rec.nsh1.new_plus(bros);

    iceb_u_pole(row_alx[0],bros,8,'|');
    rec.kodor1.new_plus(bros);

    iceb_u_polen(row_alx[0],bros,sizeof(bros),9,'|');
    rec.rsnds1.new_plus(bros);
    metka++;
   }
  
  if(metka == 2)
   break;
 }

}
