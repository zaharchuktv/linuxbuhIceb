/*$Id: iceb_rnfxml.c,v 1.1 2011-08-29 07:13:46 sasa Exp $*/
/*22.07.2011	22.07.2011	Белых А.И.	iceb_rnfxml.c
Чтение настроек необходимых для создания xml файлов с отчётностью
*/
#include "iceb_libbuh.h"
extern SQL_baza bd;

void iceb_rnfxml(class iceb_rnfxml_data *rek_zag_nn,GtkWidget *wpredok)
{
char strsql[512];

iceb_poldan("Код области",rek_zag_nn->kod_oblasti,"rnn_nast.alx",wpredok);
iceb_poldan("Код административного района",rek_zag_nn->kod_admin_r,"rnn_nast.alx",wpredok);
iceb_poldan("Код ГНИ",rek_zag_nn->kod_dpi,"rnn_nast.alx",wpredok);
iceb_poldan("Кодовая таблица",rek_zag_nn->kod_tabl,"rnn_nast.alx",wpredok);
if(rek_zag_nn->kod_tabl[0] == '\0')
 strcpy(rek_zag_nn->kod_tabl,"windows-1251");
 
//читаем реквизиты
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select naikon,adres,kod,innn,nspnds,telef,pnaim from Kontragent where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  if(row[6][0] != '\0')
    strncpy(rek_zag_nn->naim_kontr,row[6],sizeof(rek_zag_nn->naim_kontr)-1);
  else
    strncpy(rek_zag_nn->naim_kontr,row[0],sizeof(rek_zag_nn->naim_kontr)-1);
  
  strncpy(rek_zag_nn->adres,row[1],sizeof(rek_zag_nn->adres)-1);
  strncpy(rek_zag_nn->kod_edrpuo,row[2],sizeof(rek_zag_nn->kod_edrpuo)-1);
  strncpy(rek_zag_nn->innn,row[3],sizeof(rek_zag_nn->innn)-1);
  strncpy(rek_zag_nn->nspnds,row[4],sizeof(rek_zag_nn->nspnds)-1);
  strncpy(rek_zag_nn->telefon,row[4],sizeof(rek_zag_nn->telefon)-1);
 }
}
