/*$Id: zarrnesvpw.c,v 1.2 2011-05-06 08:37:47 sasa Exp $*/
/*28.04.2011	08.04.2011	Белых А.И.	zarrnesvpw.c
Чтение процентов для расчёта удержаний с работника
*/
#include <stdlib.h>
#include "buhg_g.h"


extern int kodf_esv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_bol; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_inv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_dog; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_vs;

void zarrnesvpw(short mr,short gr,class zarrnesvp_rek *all_proc,FILE *ff_prot,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

/*читаем процент*/
sprintf(strsql,"select pr1 from Zaresv where kf=%d and datnd <='%04d-%02d-01' order by datnd desc limit 1",kodf_esv,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден процент удержания единого социального взноса!\n\%s\n",strsql);
 }
else
  all_proc->proc_esv=atof(row[0]);

sprintf(strsql,"select pr1 from Zaresv where kf=%d and datnd <='%04d-%02d-01' order by datnd desc limit 1",kodf_esv_bol,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден процент удержания единого социального взноса для больничного!\n\%s\n",strsql);
 }
else
   all_proc->proc_esv_bol=atof(row[0]);


sprintf(strsql,"select pr1 from Zaresv where kf=%d and datnd <='%04d-%02d-01' order by datnd desc limit 1",kodf_esv_inv,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден процент удержания единого социального взноса для инвалидов!\n\%s\n",strsql);
 }
else
   all_proc->proc_esv_inv=atof(row[0]);

sprintf(strsql,"select pr1 from Zaresv where kf=%d and datnd <='%04d-%02d-01' order by datnd desc limit 1",kodf_esv_dog,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден процент удержания единого социального взноса для договоров!\n\%s\n",strsql);
 }
else
   all_proc->proc_esv_dog=atof(row[0]);

sprintf(strsql,"select pr1 from Zaresv where kf=%d and datnd <='%04d-%02d-01' order by datnd desc limit 1",kodf_esv_vs,gr,mr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден процент удержания единого социального взноса для военных!\n\%s\n",strsql);
 }
else
   all_proc->proc_esv_vs=atof(row[0]);

if(ff_prot != NULL)
 fprintf(ff_prot,"%s-Прочитали настройки\n\
Процент удержания единого социального взноса:%.2f\n\
Процент удержания единого социального взноса для больничного:%.2f\n\
Процент удержания единого социального взноса для инвалидов:%.2f\n\
Процент удержания единого социального взноса для договоров:%.2f\n\
Процент удержания единого социального взноса для военных:%.2f\n",
  __FUNCTION__,
  all_proc->proc_esv,
  all_proc->proc_esv_bol,
  all_proc->proc_esv_inv,
  all_proc->proc_esv_dog,
  all_proc->proc_esv_vs);

}
