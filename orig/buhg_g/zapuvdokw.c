/* $Id: zapuvdokw.c,v 1.8 2011-02-21 07:35:59 sasa Exp $ */
/*19.10.2005	19.04.2000	Белых А.И.	zapuvdokw.c
Запис в документ услуг
Если вернули 0 -записали
             1 -нет
*/
#include        <time.h>
#include	"buhg_g.h"
#include <unistd.h>

extern double	okrcn;  /*Округление цены*/
extern SQL_baza	bd;

int zapuvdokw(int tp, //1-приход 2-расход
short dd,short md,short gd, //Дата документа
const char *nomd,
int metka,int kodzap,double kolih,double cena,const char *ei,const char *shetu,
int podr,
const char *shet_sp, //счёт списания/приобретения
const char *dop_naim, //дополнение к наименованию услуги
GtkWidget *wpredok)
{
time_t 		vrem;
char		strsql[512];

cena=iceb_u_okrug(cena,okrcn);
time(&vrem);
int nomzap=0;
for(;;)
 {
  sprintf(strsql,"select tp from Usldokum1 where datd='%04d-%02d-%02d' \
and podr=%d and tp=%d and nomd='%s' and metka=%d and kodzap=%d and \
nz=%d",gd,md,dd,podr,tp,nomd,metka,kodzap,nomzap);   

  if(iceb_sql_readkey(strsql,wpredok) >= 1)
   {
    nomzap++;
    continue;
   }   

  sprintf(strsql,"insert into Usldokum1 \
  values (%d,'%04d-%02d-%02d','%s',%d,%d,%.10g,%.10g,'%s','%s',%d,%d,%ld,'%s',%d,'%s')",
  tp,gd,md,dd,nomd,metka,kodzap,kolih,cena,ei,shetu,podr,iceb_getuid(wpredok),vrem,dop_naim,nomzap,shet_sp);
  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
     {
      printf("zapuvdokw-%s\n",strsql);
      iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
     }
    else
      iceb_msql_error(&bd,"zapuvdokw-Ошибка записи !",strsql,wpredok);
    return(1);
   }
  break;
 }
return(0);
}
