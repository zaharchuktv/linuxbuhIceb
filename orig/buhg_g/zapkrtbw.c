/* $Id: zapkrtbw.c,v 1.10 2011-02-21 07:35:59 sasa Exp $ */
/*22.02.2010	23.08.2006	Белых А.И.	zapkrtbw.c
Запись карточки табельного номера в заработной плате
*/
#include        <time.h>
#include        <pwd.h>
#include	"buhg_g.h"
#include "vkartz.h"

extern SQL_baza bd;

void zapkrtbw(int stbn, //старый табельный номер
class vkartz_vs *data,GtkWidget *wpredok)
{
char		strsql[2048];
short		kart=0;
char		bros[512];

if(data->mtd != 0 || data->blok != 0 || data->flagrk != 0)
 return;
 
if(stbn == 0)
 {   
  sprintf(strsql,"insert into Kartb \
values (%d,'%s','%s',%d,%d,%d,'%s','%s',\
'%s',%d,'%s','%s','%s','%s','%s',\
%d,'%s','%s','%s','%s','%s',\
'%s',%d,'%s',%d,'%s')",
data->tabnom.ravno_atoi(),data->fio.ravno_filtr(),data->dolgn.ravno_filtr(),data->zvanie.ravno_atoi(),
data->podr.ravno_atoi(),data->kateg.ravno_atoi(),
data->data_pnr.ravno_sqldata(),data->data_usr.ravno_sqldata(),
data->shet.ravno_filtr(),data->sovm,data->in.ravno_filtr(),data->adres.ravno_filtr(),
data->nomer_pasp.ravno_filtr(),data->kem_vidan.ravno_filtr(),data->telefon.ravno_filtr(),
data->gorod_ni.ravno_atoi(),data->har_rab.ravno_filtr(),data->kod_lg.ravno_filtr(),data->kart_shet.ravno_filtr(),
data->data_vp.ravno_sqldata(),data->tip_kk.ravno_filtr(),
data->data_rog.ravno_sqldata(),data->pol,data->data_ppz.ravno_sqldata(),data->kodbank,data->kodss.ravno());
  
//  printf("zapkrtbw-%s\n",strsql);
  
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
     return;
   else
    {
     sprintf(bros,"zapkrtb-%s",gettext("Ошибка записи !"));
     iceb_msql_error(&bd,bros,strsql,wpredok);
    }
  }
 }

if(stbn != 0)
 {
  //Запись постоянной информации
  sprintf(strsql,"update Kartb \
set \
tabn=%d,\
fio='%s',\
inn='%s',\
adres='%s',\
nomp='%s',\
vidan='%s',\
telef='%s',\
kodg=%d,\
harac='%s',\
bankshet='%s',\
datvd='%s',\
tipkk='%s',\
denrog='%s',\
pl=%d,\
dppz='%s',\
kb=%d,\
kss='%s' \
where tabn=%d",
data->tabnom.ravno_atoi(),data->fio.ravno_filtr(),data->in.ravno_filtr(),
data->adres.ravno_filtr(),data->nomer_pasp.ravno_filtr(),data->kem_vidan.ravno_filtr(),
data->telefon.ravno_filtr(),data->gorod_ni.ravno_atoi(),data->har_rab.ravno_filtr(),data->kart_shet.ravno_filtr(),
data->data_vp.ravno_sqldata(),data->tip_kk.ravno_filtr(),
data->data_rog.ravno_sqldata(),data->pol,data->data_ppz.ravno_sqldata(),data->kodbank,data->kodss.ravno(),
stbn);

//  printf("zapkrtbw-%s\n",strsql);
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR)
     return;
   else
    {
     sprintf(bros,"zapkrtb-%s",gettext("Ошибка записи !"));
     iceb_msql_error(&bd,bros,strsql,wpredok);
    }
   }
  if(data->mtd+data->blok != 0)
   return;

  //Проверка необходимости записи переменных данных
  sprintf(strsql,"select tabn from Zarn where ((god > %d or \
(god=%d and  mes >=%d)) and tabn=%d)",
  data->gp,data->gp,data->mp,data->tabnom.ravno_atoi());

  if(iceb_sql_readkey(strsql,wpredok) <= 1)
      kart=1;

  struct  tm      *bf;
  time_t          tmm;

  time(&tmm);
  bf=localtime(&tmm);

  if(bf->tm_mon+1 == data->mp && bf->tm_year+1900 == data->gp)
    kart=1;

  if(kart != 0)
   {
    sprintf(strsql,"update Kartb \
set \
dolg='%s',\
datn='%s',\
datk='%s',\
zvan=%d,\
podr=%d,\
kateg=%d,\
shet='%s',\
sovm=%d,\
lgoti='%s' \
where tabn=%d",
    data->dolgn.ravno_filtr(),
    data->data_pnr.ravno_sqldata(),
    data->data_usr.ravno_sqldata(),
    data->zvanie.ravno_atoi(),
    data->podr.ravno_atoi(),
    data->kateg.ravno_atoi(),
    data->shet.ravno_filtr(),
    data->sovm,
    data->kod_lg.ravno_filtr(),
    stbn);

//    printf("zapkrtbw-%s\n",strsql);

    if(sql_zap(&bd,strsql) != 0)
     iceb_msql_error(&bd,"zapkrtb-2 Ошибка корректировки записи карточки !",strsql,wpredok);
/*********
    Если удалить все начисления и удержания то при вводе даты увольнения
    промежуточная запись не обновляется
*******/
    sprintf(strsql,"update Zarn \
set \
podr=%d,\
datn='%s',\
datk='%s' \
where god=%d and mes=%d and tabn=%d",
    data->podr.ravno_atoi(),
    data->data_pnr.ravno_sqldata(),
    data->data_usr.ravno_sqldata(),
    data->gp,data->mp,data->tabnom.ravno_atoi());

//    printf("zapkrtbw-%s\n",strsql);
    if(sql_zap(&bd,strsql) != 0)
     iceb_msql_error(&bd,"zapkrtb-3 Ошибка корректировки записи !",strsql,wpredok);
   }   

 }


}
