/*$Id: zapzarpvw.c,v 1.10 2011-02-21 07:35:59 sasa Exp $*/
/*01.09.2006	03.03.2002	Белых А.И.	zapzarpvw.c
Запись в таблицу Zarp и Zarn1
Если вернули 0 - записали
             1 - нет
*/
#include        "buhg_g.h"


int zapzarpvw(class ZARP *zp,
double suma,
short denz, //день записи
short mv,short gv,  //В счет какого месяца
short dd,short md,short gd, //дата до которой действует
short kdn, //Количество дней
const char *shet, //Счет в запись
const char *koment, //Коментарий
short nz, //Номер записи
int podr, //подразделение
const char *nomdok, //номер документа
GtkWidget *wpredok)
{
int i=0;



if((i=zapzarpw(zp,suma,denz,mv,gv,kdn,shet,koment,nz,podr,nomdok,wpredok)) == 1)
 {
  printf("%s-запись уже есть\n",__FUNCTION__);
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }


if(i == 3)
 {
  iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
  return(3); 
 }

if(i != 0)
  return(2);
 
zapzarn1w(zp->tabnom,zp->prn,zp->knu,dd,md,gd,shet,wpredok);
return(0);
}
