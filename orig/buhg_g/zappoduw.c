/* $Id: zappoduw.c,v 1.7 2011-02-21 07:35:59 sasa Exp $ */
/*08.10.2004	20.04.2000	Белых А.И.	zappoduw.c
Запись подтверждающих записей для услуг
Если вернули 0-записали
             1-нет
*/
#include        <pwd.h>
#include        <time.h>
#include	"buhg_g.h"
#include <unistd.h>

extern double	okrcn;  /*Округление цены*/

int zappoduw(int tp, //1-приход 2-расход
short dd,short md,short gd, //Дата документа
short dp,short mp,short gp, //Дата подтверждения документа
const char nomd[], //Номер документа
int metka, //0-материал 1-услуга
int kodzap, //Код материалла/услуги
double kolih, //Количество
double cena, //Цена
const char ei[], //Единица измерения
const char shetu[], //Счет учета
int podr, //Подразделение
int nomzap,
GtkWidget *wpredok)
{
time_t 		vrem;
char		strsql[512];

cena=iceb_u_okrug(cena,okrcn);

time(&vrem);

sprintf(strsql,"insert into Usldokum2 \
values (%d,'%04d-%02d-%02d','%04d-%02d-%02d','%s',%d,%d,%.10g,%.10g,'%s','%s',%d,%d,%ld,%d)",
tp,gd,md,dd,gp,mp,dp,nomd,metka,kodzap,kolih,cena,ei,shetu,podr,iceb_getuid(wpredok),vrem,nomzap);

return(iceb_sql_zapis(strsql,0,0,wpredok));
}
