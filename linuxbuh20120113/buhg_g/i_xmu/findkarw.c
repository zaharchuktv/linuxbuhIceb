/*$Id: findkarw.c,v 1.6 2011-02-21 07:35:51 sasa Exp $*/
/*17.11.2004	06.11.2001	Белых А.И.  	findkarw.c
Поиск карточек к которым можно привязать заданное количество материалла
Возвращает количество карточек к которым можно привязаться
           0- не найдено ни одной карточки с которой можно списать
              нужное количество
           -1 карточек вообще нет
*/
#include        <stdlib.h>
#include        <math.h>
#include        "../headers/buhg_g.h"

extern SQL_baza bd;

int findkarw(short dd,short md,short gd, //Дата привязки
int sklad,int kodmat,double kolih,
iceb_u_int *nomkart, //Массив с номерами карточек
iceb_u_double *kolihp, //Массив с количеством, которое можно привязать к карточкам
int kodopuc, //0-к любой 1-с нужной ценой
double cena, //Цена карточки к которой надо привязать
const char *shetu, //Счет учета
GtkWidget *wpredok)
{
SQL_str		row;
int		kolstr;
char		strsql[512];
double		kolihpriv=0.;
int		nomkar=0;
int		kolkar=0;

if(kodopuc == 0)
 sprintf(strsql,"select nomk from Kart where sklad=%d and kodm=%d",
 sklad,kodmat);

if(kodopuc == 1)
 sprintf(strsql,"select nomk from Kart where sklad=%d and kodm=%d and cena=%.10g",
sklad,kodmat,cena);

if(shetu[0] != '\0')
 {
  strcat(strsql," and shetu='");
  strcat(strsql,shetu);
  strcat(strsql,"'");
 }

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,"findkar-Ошибка создания курсора",strsql,wpredok);
if(kolstr == 0)
  return(-1);
class ostatok ost;
while(cur.read_cursor(&row) != 0)
 {
  nomkar=atoi(row[0]);
  ostkarw(1,1,gd,dd,md,gd,sklad,nomkar,&ost);
  if(ost.ostg[3] < 0.0000001)
    continue;

  kolkar++;
  nomkart->plus(nomkar,-1);

  if(ost.ostg[3] > kolih-kolihpriv)
   {
    kolihp->plus(kolih-kolihpriv,-1);
    kolihpriv+=kolih-kolihpriv;
   }
  else
   {
    kolihp->plus(ost.ostg[3],-1);
    kolihpriv+=ost.ostg[3];
   }

  if(fabs(kolih-kolihpriv) < 0.0000001)
    return(kolkar);
 }
return(kolkar);
}
