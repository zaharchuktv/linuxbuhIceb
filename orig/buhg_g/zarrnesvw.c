/*$Id: zarrnesvw.c,v 1.4 2011-08-29 07:13:44 sasa Exp $*/
/*12.08.2011	26.01.2011	Белых А.И.	zarrnesvw.c
Чтение настроек для расчёта единого сочиального взноса
*/
#include <stdlib.h>
#include "buhg_g.h"

int kod_esv=0; /*Код удержания единого социального взноса*/
int kod_esv_bol=0; /*Код удержания единого социального взноса*/
int kod_esv_inv=0; /*Код удержания единого социального взноса*/
int kod_esv_dog=0; /*Код удержания единого социального взноса*/
int kod_esv_vs=0;
class iceb_u_str kods_esv_all; /*Все коды единого социального взноса*/

int kodf_esv=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_bol=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_inv=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_dog=0; /*Код фонда удержания единого социального взноса*/
int kodf_esv_vs=0; /*Код фонда удержания единого социального взноса*/

class iceb_u_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/
class iceb_u_str kod_zv_gr; /*коды званий гражданских*/
void zarrnesvw(FILE *ff_prot,GtkWidget *wpredok)
{

iceb_poldan("Код удержания единого социального взноса",&kod_esv,"zarnast.alx",wpredok);
iceb_poldan("Код удержания единого социального взноса для больничных",&kod_esv_bol,"zarnast.alx",wpredok);
iceb_poldan("Код удержания единого социального взноса для инвалидов",&kod_esv_inv,"zarnast.alx",wpredok);
iceb_poldan("Код удержания единого социального взноса для договоров",&kod_esv_dog,"zarnast.alx",wpredok);
iceb_poldan("Код удержания единого социального взноса для военных",&kod_esv_vs,"zarnast.alx",wpredok);

kods_esv_all.plus(kod_esv);
kods_esv_all.z_plus(kod_esv_bol);
kods_esv_all.z_plus(kod_esv_inv);
kods_esv_all.z_plus(kod_esv_vs);

iceb_poldan("Коды начислений не входящие в расчёт удержания единого социального взноса",&knvr_esv_r,"zarnast.alx",wpredok);
iceb_poldan("Коды званий гражданских",&kod_zv_gr,"zarnast.alx",wpredok);

iceb_poldan("Код фонда единого социального взноса",&kodf_esv,"zarnast.alx",wpredok);
iceb_poldan("Код фонда единого социального взноса для инвалидов",&kodf_esv_inv,"zarnast.alx",wpredok);
iceb_poldan("Код фонда единого социального взноса для больничных",&kodf_esv_bol,"zarnast.alx",wpredok);
iceb_poldan("Код фонда единого социального взноса для договоров",&kodf_esv_dog,"zarnast.alx",wpredok);
iceb_poldan("Код фонда единого социального взноса для военных",&kodf_esv_vs,"zarnast.alx",wpredok);

if(ff_prot != NULL)
 fprintf(ff_prot,"%s-Прочитали настройки\n\
Код удержания единого социального взноса:%d\n\
Код удержания единого социального взноса для больничных:%d\n\
Код удержания единого социального взноса для инвалидов:%d\n\
Код удержания единого социального взноса для договоров:%d\n\
Коды начислений не входящие в расчёт удержания единого социального взноса:%s\n\
Код фонда единого социального взноса:%d\n\
Код фонда единого социального взноса для инвалидов:%d\n\
Код фонда единого социального взноса для больничных:%d\n\
Код фонда единого социального взноса для договоров:%d\n\
Код фонда единого социального взноса для военных:%d\n",
  __FUNCTION__,
  kod_esv,
  kod_esv_bol,
  kod_esv_inv,
  kod_esv_dog,
  knvr_esv_r.ravno(),
  kodf_esv,
  kodf_esv_bol,
  kodf_esv_inv,
  kodf_esv_dog,
  kodf_esv_vs);

}