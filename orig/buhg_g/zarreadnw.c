/*$Id: zarreadnw.c,v 1.9 2011-04-14 16:09:36 sasa Exp $*/
/*14.04.2011	23.01.2009	Белых А.И.	zarreadnw.c
Чтение настроек перед автоматическим расчётом начислений и удерданий*/
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"
#include "read_nast_indw.h"

int read_nast_ind(short mr,short gr,class index_ua *data,GtkWidget *wpredok);
void zar_mpslw(GtkWidget *wpredok);

extern float procent_fb_dog; /*Прочент начисления на фонд зарплаты для фонда безработицы для работающий по договору*/
extern float dnei,hasov; //Количество рабочих дней и часов в определённом месяце. Читает программа redkalw.c перед началом расчёта начислений/удержаний
float pnds=20.;
class index_ua tabl_ind; /*Таблица для индексации зарплаты*/

void zarreadnw(short dr,short mr,short gr,
int metka_r,   //1-начисления 2-удержания 3-начисления и удержания 4-соц отчисления 5-проводки
FILE *ff_prot,GtkWidget *wpredok)
{
//Читаем повторно-может их ктото изменил
redkalw(mr,gr,&dnei,&hasov,wpredok);

zar_read_tnw(dr,mr,gr,ff_prot,wpredok); 
/*Читаем таблицу разрядов-коэффициентов*/
zartarrozw(wpredok);
zartarroz1w(wpredok);

class iceb_u_str znah("");

if(iceb_poldan("Процент начисления для фонда безработицы",&znah,"zardog.alx",wpredok) == 0)
 {
  procent_fb_dog=znah.ravno_atof();
 }
if(ff_prot != NULL)
   fprintf(ff_prot,"Процент на фонд зарплаты для фонда безработицы для работающих по договорам=%.2f\n",procent_fb_dog);

read_nast_ind(mr,gr,&tabl_ind,wpredok);
pnds=iceb_pnds(dr,mr,gr,wpredok);

/*Чтение кодов единого социального взноса*/
zarrnesvw(ff_prot,wpredok);
if(metka_r == 2 || metka_r == 3)
 zar_mpslw(wpredok);


}

