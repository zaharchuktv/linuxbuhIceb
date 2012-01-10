/*$Id: uoszaphd.c,v 1.15 2011-08-29 07:13:44 sasa Exp $*/
/*16.12.2009	28.10.2007	Белых А.И.	uoszaphd.c
Запись шапки документа для учёта основных средств
*/
#include <stdlib.h>
#include "buhg_g.h"
#include "uosvhd.h"
#include <unistd.h>

void  uosinp(short dd,short md,short gd,short mz,short gz,const char *nomdok,GtkWidget*);

extern SQL_baza bd;

int uoszaphd(class uosvhd_data *data,GtkWidget *wpredok)
{

char strsql[512];
SQL_str row1;
class SQLCURSOR curr;

time_t vrem=time(NULL);
short d,m,g;    /*Новая дата документа*/
short dz,mz,gz; /*Старая дата окумента*/
short dvd,mvd,gvd; //дата выдачи доверенности
short dop,mop,gop; //дата оплаты
short dnn,mnn,gnn; //Дата получения/выдачи налоговой накладной

iceb_u_rsdat(&d,&m,&g,data->datadok.ravno(),1);
iceb_u_rsdat(&dz,&mz,&gz,data->datadok_z.ravno(),1);
iceb_u_rsdat(&dop,&mop,&gop,data->data_opl.ravno(),1);
iceb_u_rsdat(&dnn,&mnn,&gnn,data->data_vnn.ravno(),1);
iceb_u_rsdat(&dvd,&mvd,&gvd,data->data_vd.ravno(),1);


/*Проверяем дату новую дату*/
if(iceb_pbpds(m,g,wpredok) != 0)
  return(1);
/*Проверяем старую дату*/
if(dz != 0)
if(iceb_pbpds(mz,gz,wpredok) != 0)
  return(1);

int metka_opl=0;

//Узнаём нужно ли для этой операции делать проводки
if(data->tipz == 1)
 sprintf(strsql,"select prov,vido from Uospri where kod='%s'",data->kod_op.ravno());
if(data->tipz == 2)
 sprintf(strsql,"select prov,vido from Uosras where kod='%s'",data->kod_op.ravno());

int prov=1;
if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
 if(row1[0][0] == '1')
  prov=0;

if(atoi(row1[1]) != 0)
 metka_opl=1;


if(data->datadok_z.getdlinna() <= 1) /* Запись нового документа*/
 {
  
//  iceb_poldan("Н.Д.С.",strsql,"matnast.alx",wpredok);
//  float nds=atof(strsql);
  float nds=iceb_pnds(d,m,g,wpredok);


  /*Записываем шапку документа*/

  sprintf(strsql,"insert into Uosdok \
values ('%04d-%d-%d',%d,'%s','%s','%s',%d,%d,%d,'%s',%d,%d,%d,'%s',%d,%ld,\
'%s','%04d-%d-%d','%s','%04d-%d-%d','%s','%s','%04d-%d-%d',%d,%.2f,'%s',%d,'%s')",
  g,m,d,data->tipz,data->kod_op.ravno(),data->kontr.ravno(),data->nomdok.ravno(),data->podr.ravno_atoi(),data->mat_ot.ravno_atoi(),
  data->podr_v.ravno_atoi(),data->nomdok_post.ravno(),data->mat_ot_v.ravno_atoi(),prov,0,data->osnov.ravno(),
  iceb_getuid(wpredok),vrem,data->dover.ravno(),gvd,mvd,dvd,data->forop.ravno(),
  gnn,mnn,dnn,data->nomnalnak.ravno(),data->sherez.ravno(),gop,mop,dop,
  data->lnds,nds,data->dvnn.ravno_sqldata(),metka_opl,data->vid_dog.ravno_filtr());
  
  /*printf("strsql=%s\n",strsql);*/
  
  if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
    return(1);

  if(data->mdd == 1)
   {
    sprintf(strsql,"insert into Uosdok \
    values ('%04d-%d-%d',%d,'%s','%s','%s',%d,%d,%d,'%s',%d,%d,%d,'%s',\
%d,%ld,'%s','%04d-%d-%d','%s','%04d-%d-%d','%s','%s','%04d-%d-%d',%d,%.2f,'%s',%d)",
    g,m,d,data->tipzv,data->kod_op.ravno(),data->kontr.ravno(),data->nomdok_post.ravno(),
    data->podr_v.ravno_atoi(),
    data->mat_ot_v.ravno_atoi(),
    data->podr.ravno_atoi(),data->nomdok.ravno(),data->mat_ot.ravno_atoi(),0,0,data->osnov.ravno(),
    iceb_getuid(wpredok),vrem,data->dover.ravno(),gvd,mvd,dvd,data->forop.ravno(),
    0,0,0,"","",0,0,0,0,nds,data->dvnn.ravno_sqldata(),metka_opl);

    if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
      return(1);
   }
 }
else /*Корректировка  шапки документа*/
 {

  if(m != mz || g != gz);
   if(iceb_pvkdd(3,dz,mz,gz,m,g,data->podr_z.ravno_atoi(),data->nomdok_z.ravno(),data->tipz,wpredok) != 0)
    return(1);
   
  sprintf(strsql,"update Uosdok \
set \
datd='%04d-%d-%d',\
tipz=%d,\
kodop='%s',\
kontr='%s',\
nomd='%s',\
podr=%d,\
kodol=%d,\
podrv=%d,\
nomdv='%s',\
kodolv=%d,\
osnov='%s',\
ktoz=%d,\
vrem=%ld,\
prov=%d,\
dover='%s',\
datvd='%04d-%d-%d',\
forop='%s',\
datpnn='%04d-%d-%d',\
nomnn='%s',\
sherez='%s',\
datop='%04d-%d-%d',\
nds=%d,\
dvnn='%s',\
vd='%s' \
where datd='%d-%d-%d' and nomd='%s'",
  g,m,d,data->tipz,data->kod_op.ravno(),data->kontr.ravno(),data->nomdok.ravno(),data->podr.ravno_atoi(),data->mat_ot.ravno_atoi(),
  data->podr_v.ravno_atoi(),data->nomdok_post.ravno(),data->mat_ot_v.ravno_atoi(),data->osnov.ravno(),iceb_getuid(wpredok),vrem,prov,
  data->dover.ravno(),gvd,mvd,dvd,data->forop.ravno(),
  gnn,mnn,dnn,data->nomnalnak.ravno(),data->sherez.ravno(),gop,mop,dop,
  data->lnds,data->dvnn.ravno_sqldata(),
  data->vid_dog.ravno_filtr(),
  gz,mz,dz,data->nomdok_z.ravno());
//  printw("\n%s\n",strsql);
//  OSTANOV();

  if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
    return(1);

  if(data->mdd == 1)
   {
    sprintf(strsql,"update Uosdok \
set \
datd='%04d-%d-%d',\
tipz=%d,\
kodop='%s',\
kontr='%s',\
nomd='%s',\
podr=%d,\
kodol=%d,\
podrv=%d,\
nomdv='%s',\
kodolv=%d,\
osnov='%s',\
ktoz=%d,\
vrem=%ld,\
datpnn='%04d-%d-%d',\
nomnn='%s',\
sherez='%s',\
datop='%04d-%d-%d',\
nds=%d \
where datd='%04d-%d-%d' and nomd='%s'",
    g,m,d,data->tipzv,data->kod_op.ravno(),data->kontr.ravno(),
    data->nomdok_post.ravno(),data->podr_v.ravno_atoi(),data->mat_ot_v.ravno_atoi(),
    data->podr.ravno_atoi(),data->nomdok.ravno(),data->mat_ot.ravno_atoi(),data->osnov.ravno(),iceb_getuid(wpredok),vrem,0,0,0,"","",0,0,0,0,
    gz,mz,dz,data->nomdok_post_z.ravno());

    if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
      return(1);

   }
  /*Исправляем записи в документе*/
//  if(SRAV1(g,m,d,gz,mz,dz) != 0 || SRAV(data->nomdok.ravno(),data->nomdok_z.ravno(),0) != 0 ||
  if(iceb_u_sravmydat(d,m,g,dz,mz,gz) != 0 || iceb_u_SRAV(data->nomdok.ravno(),data->nomdok_z.ravno(),0) != 0 ||
  data->podr.ravno_atoi() != data->podr_z.ravno_atoi() || data->mat_ot.ravno_atoi() != data->mat_ot_z.ravno_atoi())
   {
    sprintf(strsql,"update Uosdok1 \
set \
datd='%04d-%d-%d',\
nomd='%s',\
podr=%d,\
kodol=%d \
where datd='%04d-%d-%d' and nomd='%s'",
    g,m,d,data->nomdok.ravno(),data->podr.ravno_atoi(),data->mat_ot.ravno_atoi(),
    gz,mz,dz,data->nomdok_z.ravno());

    if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
      return(1);

   }

  if(data->mdd == 1)
//  if(SRAV1(g,m,d,gz,mz,dz) != 0 || SRAV(data->nomdok.ravno()p,data->nomdok_z.ravno()1,0) != 0)
  if(iceb_u_sravmydat(d,m,g,dz,mz,gz) != 0 || iceb_u_SRAV(data->nomdok_post.ravno(),data->nomdok_post_z.ravno(),0) != 0)
   {
    sprintf(strsql,"update Uosdok1 \
set \
datd='%04d-%d-%d',\
nomd='%s' \
where datd='%04d-%d-%d' and nomd='%s'",
    g,m,d,data->nomdok_post.ravno(),
    gz,mz,dz,data->nomdok_post_z.ravno());

    if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
      return(1);
   }

//  Исправляем дату в промежуточных записях
//  if(SRAV1(g,m,d,gz,mz,dz) != 0 )
  if(iceb_u_sravmydat(d,m,g,dz,mz,gz) != 0 )
   {
    uosinp(d,m,g,mz,gz,data->nomdok.ravno(),wpredok);
   }  

//  if(SRAV1(g,m,d,gz,mz,dz) != 0 || SRAV(data->nomdok.ravno(),data->nomdok_z.ravno(),0) != 0)
  if(iceb_u_sravmydat(g,m,d,gz,mz,dz) != 0 || iceb_u_SRAV(data->nomdok.ravno(),data->nomdok_z.ravno(),0) != 0)
   {
    /*Исправляем проводки в которых дата проводки совпадает с
    датой документа*/

    sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datp='%04d-%d-%d',\
datd='%04d-%d-%d',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%d-%d' and datp='%04d-%d-%d' and nomd='%s' and pod=%d and \
kto='%s' and tz=%d",
    data->kod_op.ravno(),data->nomdok.ravno(),g,m,d,g,m,d,iceb_getuid(wpredok),vrem,
    gz,mz,dz,gz,mz,dz,data->nomdok_z.ravno(),data->podr_z.ravno_atoi(),gettext("УОС"),
    data->tipz);

    iceb_sql_zapis(strsql,1,0,wpredok);
    /*Исправляем остальные проводки*/

    sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datd='%04d-%d-%d',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%d-%d' and nomd='%s' and pod=%d and kto='%s' and tz=%d",
   data->kod_op.ravno(),data->nomdok.ravno(),g,m,d,iceb_getuid(wpredok),vrem,
   gz,mz,dz,data->nomdok_z.ravno(),data->podr_z.ravno_atoi(),gettext("УОС"),
   data->tipz);

    iceb_sql_zapis(strsql,1,0,wpredok);
   }

  if(iceb_u_SRAV(data->kontr_z.ravno(),data->kontr.ravno(),0) != 0)
   {
    /*Исправляем код контрагента в проводках*/
    sprintf(strsql,"update Prov \
set \
kodkon='%s' \
where datd='%04d-%d-%d' and nomd='%s' and pod=%d and kto='%s' and \
kodkon='%s' and tz=%d",
    data->kontr.ravno(),
    gz,mz,dz,data->nomdok_z.ravno(),data->podr.ravno_atoi(),gettext("УОС"),data->kontr_z.ravno(),
    data->tipz);

    iceb_sql_zapis(strsql,1,0,wpredok);
   }

 }

return(0);


}
/***************************/

void  uosinp(short dd,short md,short gd, //Дата документа
short mz,short gz, //Предыдущая дата документа
const char *nomdok,
GtkWidget *wpredok)
{
char strsql[512];
int	kolstr=0;
SQL_str	row;

sprintf(strsql,"select innom from Uosdok1 \
where datd='%04d-%02d-%02d' and nomd='%s'",
gd,md,dd,nomdok);


SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 return;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"update Uosinp \
set \
mes=%d,\
god=%d \
where god=%d and mes=%d and innom=%s",
    md,gd,gz,mz,row[0]);
  iceb_sql_zapis(strsql,1,0,wpredok);

 }

}
