/*$Id: uosprhd.c,v 1.9 2011-02-21 07:35:58 sasa Exp $*/
/*08.04.2008	28.10.2007	Белых А.И.	uosprhd.c
Проверка реквизитов шапки документа
Eсли вернули 0-можно записывать
             1-нельзя
*/
#include <stdlib.h>
#include "buhg_g.h"
#include "uosvhd.h"

extern SQL_baza bd;

int uosprhd(class uosvhd_data *data,GtkWidget *wpredok)
{
//printf("uosprhd\n");
SQL_str row;
class SQLCURSOR cur;

char strsql[512];
//проверяем код контрагента
sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",data->kontr.ravno());
if(iceb_sql_readkey(strsql,wpredok) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),data->kontr.ravno());
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

//проверяем код подразделения
sprintf(strsql,"select naik from Uospod where kod=%d",data->podr.ravno_atoi());
if(iceb_sql_readkey(strsql,wpredok) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),data->podr.ravno_atoi());
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

 
//проверяем код материально-ответственного
sprintf(strsql,"select naik from Uosol where kod=%d",data->mat_ot.ravno_atoi());
if(iceb_sql_readkey(strsql,wpredok) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код материально-ответственного"),data->mat_ot.ravno_atoi());
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }



if(data->datadok.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена дата документа !"),wpredok);
  return(1);
 }
//проверяем дату документа
if(data->datadok.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата документа !"),wpredok);
  return(1);
 }
//проверяем код формы оплаты
if(data->forop.getdlinna() > 1)
 {
  sprintf(strsql,"select naik from Foroplat where kod='%s'",data->forop.ravno());
  if(iceb_sql_readkey(strsql,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код формы оплаты"),data->forop.ravno());
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
 }

//проверяем код операции
if(data->tipz == 1)
 sprintf(strsql,"select vido from Uospri where kod='%s'",data->kod_op.ravno());
if(data->tipz == 2)
 sprintf(strsql,"select vido from Uosras where kod='%s'",data->kod_op.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data->kod_op.ravno());
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

int vido=atoi(row[0]);
 

if(iceb_pbpds(data->datadok.ravno(),wpredok) != 0)
 {
  sprintf(strsql,gettext("Дата %s заблокирована !"),data->datadok.ravno());
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

if(data->datadok_z.getdlinna() > 1)
  if(iceb_pbpds(data->datadok_z.ravno(),wpredok) != 0)
   {
    sprintf(strsql,gettext("Дата %s заблокирована !"),data->datadok_z.ravno());
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }

if(data->data_vd.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата выдачи доверенности !"),wpredok);
  return(1);
 }


if(data->data_vnn.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата получения налоговой накладной !"),wpredok);
  return(1);
 }

if(data->data_opl.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата оплаты !"),wpredok);
  return(1);
 }



if(iceb_u_SRAV(data->nomdok_z.ravno(),data->nomdok.ravno(),0) != 0)
if(uosprovnd(data->datadok.ravno_god(),data->tipz,data->nomdok.ravno(),wpredok) != 0)
 {
  sprintf(strsql,gettext("Документ с номером %s уже есть !"),data->nomdok.ravno());
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

if(data->podr.ravno_atoi() != data->podr_z.ravno_atoi())
 if(uosprovdok(&data->datadok,data->nomdok.ravno(),data->tipz,wpredok) != 0)
  {
   iceb_menu_soob(gettext("Код подразделения сменить уже невозможно !"),wpredok);
   return(1);  
  }

if(data->mat_ot.ravno_atoi() != data->mat_ot_z.ravno_atoi())
 if(uosprovdok(&data->datadok,data->nomdok.ravno(),data->tipz,wpredok) != 0)
  {
   iceb_menu_soob(gettext("Код материально-ответственного сменить уже невозможно !"),wpredok);
   return(1);  
  }
if(data->dvnn.prov_dat() != 0)
 {
   iceb_menu_soob(gettext("Неправильно ввели дату выписки!"),wpredok);
   return(1);  
  
 }

/*Проверяем номер документа*/
if(iceb_u_SRAV(data->nomdok.ravno(),data->nomdok_z.ravno(),0) != 0)
 if(uosprovnd(data->datadok.ravno_god(),data->tipz,data->nomdok.ravno(),wpredok) != 0)
  {
   iceb_menu_soob(gettext("Документ с таким номером уже есть!"),wpredok);
   return(1);
  }

if(data->mdd == 1) //Проверяем дополнительные реквизиты для двойного документа
 {
  sprintf(strsql,"select naik from Uospod where kod=%d",data->podr_v.ravno_atoi());
  if(iceb_sql_readkey(strsql,wpredok) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),data->podr_v.ravno_atoi());
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }

  sprintf(strsql,"select naik from Uosol where kod=%d",data->mat_ot_v.ravno_atoi());
  if(iceb_sql_readkey(strsql,wpredok) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код материально-ответственного"),data->mat_ot_v.ravno_atoi());
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }

  if(iceb_u_SRAV(data->nomdok_post.ravno(),data->nomdok_post_z.ravno(),0) != 0)
    if(uosprovnd(data->datadok.ravno_god(),data->tipzv,data->nomdok_post.ravno(),wpredok) != 0)
     {
      iceb_menu_soob(gettext("Номер парного документа уже есть!"),wpredok);
      return(1);
     }

  if(data->tipzv == 1)
   sprintf(strsql,"select vido from Uospri where kod='%s'",data->kod_op.ravno());
  if(data->tipzv == 2)
   sprintf(strsql,"select vido from Uosras where kod='%s'",data->kod_op.ravno());

  if(iceb_sql_readkey(strsql,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Для парного документа не найден код операции"),data->kod_op.ravno());
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }

 }


if(data->tipz == 2 && vido == 0) //расход внешний
 {
  /*Проверяем есть ли такой номер налоговой накладной*/
  if(data->nomnalnak.getdlinna() > 1)
  if(iceb_u_SRAV(data->nomnalnak_z.ravno(),data->nomnalnak.ravno(),0) != 0 )
   {
    short g;
    short m;
    iceb_u_rsdat1(&m,&g,data->datadok.ravno());
    if(iceb_provnomnnak(m,g,data->nomnalnak.ravno(),wpredok) != 0)
     {
      sprintf(strsql,gettext("С номером %s налоговая накладная уже есть!"),data->nomnalnak.ravno());
      iceb_menu_soob(strsql,wpredok);
      return(1);
     }        
   }
 
 }

return(0);

}
