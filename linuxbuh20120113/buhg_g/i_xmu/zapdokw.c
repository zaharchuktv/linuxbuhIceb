/* $Id: zapdokw.c,v 1.20 2011-02-21 07:35:59 sasa Exp $ */
/*16.12.2009    14.04.1997      Белых А.И. zapdokw.c
Запись накладной
Если вернули 0 порядок
	     1 такой документ уже есть
*/
#include <stdlib.h>
#include        <time.h>
#include        <pwd.h>
#include	"../headers/buhg_g.h"
#include        <unistd.h>
#include        "zapdokw.h"

extern short           mvnp; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
extern SQL_baza bd;


int zapdokw(class zapdokw_data *data,class iceb_lock_tables *lokt,
GtkWidget *wpredok)
{
char		bros[312];
char		strsql[1024];
time_t          tmm;
short           mz0;
short		pro,pod;
int	ktorab=iceb_getuid(wpredok);
float pnds=0.;
short dd,md,gd; //Дата документа
short dpnn,mpnn,gpnn; //Дата получения налоговой накладной
//printf("Записываем шапку документа.\n");
iceb_u_str datad;
datad.plus(data->datad.ravno_sqldata());
iceb_u_rsdat(&dd,&md,&gd,data->datad.ravno(),1);
iceb_u_rsdat(&dpnn,&mpnn,&gpnn,data->data_pnn.ravno_sqldata(),1);

mz0=0;
pro=pod=0;
if(mvnp != 0 && data->tipz == 1)
  pro=1;

int metka_opl=0;
//Узнаём нужно ли для этой операции делать проводки
if(data->tipz == 1)
 sprintf(strsql,"select prov,vido from Prihod where kod='%s'",data->kodop.ravno());
if(data->tipz == 2)
 sprintf(strsql,"select prov,vido from Rashod where kod='%s'",data->kodop.ravno());
SQL_str row;
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 if(row[0][0] == '1')
  pro=1;

if(atoi(row[1]) != 0)
 metka_opl=1;
   
time(&tmm);

/**************/
/*Новая запись*/
/***************/
pnds=iceb_pnds(dd,md,gd,wpredok);

if(data->dd_kor == 0)
 {   
//  iceb_poldan("Н.Д.С.",bros,"matnast.alx",wpredok);
//  pnds=iceb_u_atof(bros);
  
  if(zap_s_mudokw(data->tipz,dd,md,gd,data->sklad.ravno_atoi(),
  data->kontr.ravno(),
  data->nomdok.ravno_filtr(),data->nomnalnak.ravno_filtr(),data->kodop.ravno_filtr(),pod,pro,
  data->nomdok_p.ravno_filtr(),dpnn,mpnn,gpnn,metka_opl,wpredok) != 0)
    return(1);


  /*Записываем парную накладную*/
  if(data->sklad_par != 0 && data->tipz == 2)
   {
    pod=0;
    pro=1;
    sprintf(bros,"00-%s",data->sklad.ravno());

    zap_s_mudokw(1,dd,md,gd,data->sklad_par,
    bros,
    data->nomdok_p.ravno_filtr(),"",data->kodop.ravno_filtr(),pod,pro,
    data->nomdok.ravno_filtr(),0,0,0,metka_opl,wpredok);
   }

  //Разблокировка таблиц
  lokt->unlock();
  


naz:;


  /*Номер и дата выдачи доверенности*/
  if(data->dover.getdlinna() > 1 || data->data_dover.getdlinna() >1)
   {
    sprintf(bros,"%s#%s",data->dover.ravno(),data->data_dover.ravno());
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),1,bros);
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }


  /*Через кого*/
  if(data->sherez.getdlinna() > 1)
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),2,data->sherez.ravno_filtr());
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }


  if(data->metkapr == 0)
   {
    //Блокировка документа для выписки
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%d')",
    data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),4,ktorab);
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }

  if(data->loginrash != 0)
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%d')",
    data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),5,data->loginrash);
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }

  /*НДС*/
  if(data->nds != 0 )
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,%d)",
    data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),11,data->nds);
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }

    /*Пишем в ответный документ*/
    if(data->sklad_par != 0 && mz0 == 0)
     {
      sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,%d)",
      data->datad.ravno_god(),data->sklad_par,data->nomdok_p.ravno_filtr(),11,data->nds);
      if(sql_zap(&bd,strsql) != 0)
       {
        sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
        iceb_msql_error(&bd,bros,strsql,wpredok);
       }
     }   
   }

  /*Основание для выписки накладной*/
  if(data->tipz == 2 && data->osnov.getdlinna() > 1)
   {
    sprintf(strsql,"insert into Dokummat2 values (%d,%s,'%s',%d,'%s')",
    data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),3,data->osnov.ravno_filtr());
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }
  /*Коментарий для приходного документа*/
  if(data->tipz == 1 && data->osnov.getdlinna() > 1)
   {
    sprintf(strsql,"insert into Dokummat2 values (%d,%s,'%s',%d,'%s')",
    data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),3,data->osnov.ravno_filtr());
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }
  /*Условие продажи.*/
  if(data->tipz == 2 && data->uslov_prod.getdlinna() > 1)
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
    data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),7,data->uslov_prod.ravno_filtr());
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
        iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }  

  /*Форма оплаты.*/
  if(data->tipz == 2 && data->forma_oplat.getdlinna() > 1)
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
    data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),8,data->forma_oplat.ravno_filtr());
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }


  /*Дата оплаты.*/
  if(data->data_opl.getdlinna() > 1)
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
    data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),9,data->data_opl.ravno());
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }



  /*Номер машины*/
  if(data->tipz == 2 && data->nomer_mah.getdlinna() > 1)
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
    data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),10,data->nomer_mah.ravno_filtr());
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
        iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }


  /*Дата накладной поставщика*/
  if(data->tipz == 1 && data->data_dok_p.getdlinna() > 1)
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%s')",
    data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),14,data->data_dok_p.ravno());
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
        iceb_msql_error(&bd,bros,strsql,wpredok);
     }
   }
  if(data->suma_kor_dok != 0.)
   {
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',%d,'%.2f')",
    data->datad.ravno_god(),data->sklad.ravno(),data->nomdok.ravno_filtr(),13,data->suma_kor_dok);
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка записи !"));
        iceb_msql_error(&bd,bros,strsql,wpredok);
     }

   }
 } 



/***************/
/*Корректировка*/
/***************/
if(data->dd_kor != 0 && mz0 == 0)
 {   
  sprintf(strsql,"update Dokummat \
set \
tip=%d,\
datd='%s',\
sklad=%s,\
kontr='%s',\
nomd='%s',\
nomnn='%s',\
kodop='%s',\
pod=%d,\
pro=%d,\
ktoi=%d,\
vrem=%ld,\
nomon='%s',\
datpnn='%s',\
pn=%.2f \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
data->tipz,datad.ravno(),data->sklad.ravno(),
data->kontr.ravno_filtr(),data->nomdok.ravno_filtr(),
data->nomnalnak.ravno_filtr(),data->kodop.ravno_filtr(),pod,pro,ktorab,tmm,
data->nomdok_p.ravno_filtr(),
data->data_pnn.ravno_sqldata(),
pnds,
data->gd_kor,
data->md_kor,
data->dd_kor,
data->skl_kor,
data->nomdok_kor.ravno_filtr());


  //printf("zapdokw-%s\n",strsql);
  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(bros,"zapdokw %s",gettext("Ошибка корректировки записи !"));
    iceb_msql_error(&bd,bros,strsql,wpredok);

    return(2);
   }

  //Разблокировка таблиц
  lokt->unlock();
   


  /*Удаляем все дополнительные записи*/
  if(data->metkapr == 0)
    sprintf(strsql,"delete from Dokummat2 \
where god=%d and sklad=%d and nomd='%s'",
    data->gd_kor,data->skl_kor,data->nomdok_kor.ravno_filtr());

  if(data->metkapr == 1)
    sprintf(strsql,"delete from Dokummat2 \
where god=%d and sklad=%d and nomd='%s' and nomerz != 4",
    data->gd_kor,data->skl_kor,data->nomdok_kor.ravno_filtr());


/*printw("Удаление доп. зап\nstrsql=%s\n",strsql);*/

  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(bros,"zapdokw %s",gettext("Ошибка удаления записей !"));
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }

  mz0=1;

  goto naz;  

 }


if(data->dd_kor != 0)
 {

  
  /*Исправляем проводки в которых дата проводки совпадает с
  датой документа*/
//  printf("Проверяем проводки...\n");

  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datp='%s',\
datd='%s',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%d-%d' and datp='%04d-%d-%d' and nomd='%s' and \
pod=%d and kto='%s'",
  data->kodop.ravno_filtr(),data->nomdok.ravno_filtr(),
  datad.ravno(),datad.ravno(),ktorab,tmm,
  data->gd_kor,data->md_kor,data->dd_kor,data->gd_kor,data->md_kor,data->dd_kor,
  data->nomdok_kor.ravno_filtr(),data->skl_kor,gettext("МУ"));
  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(bros,"zapdokw %s",gettext("Ошибка корректировки записей !"));
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }
  /*Исправляем остальные проводки*/
//  printf("Проверяем проводки...\n");


  sprintf(strsql,"update Prov \
set \
oper='%s',\
nomd='%s',\
datd='%s',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and nomd='%s' and \
pod=%d and kto='%s'",
  data->kodop.ravno_filtr(),data->nomdok.ravno_filtr(),
  datad.ravno(),ktorab,tmm,
  data->gd_kor,data->md_kor,data->dd_kor,
  data->nomdok_kor.ravno_filtr(),data->skl_kor,gettext("МУ"));

  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(bros,"zapdokw %s",gettext("Ошибка корректировки записей !"));
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }

//  printf("Проверяем проводки...\n");

  if(iceb_u_SRAV(data->kontr_kor.ravno(),data->kontr.ravno(),0) != 0)
   {
    /*Исправляем код контрагента в проводках*/
    sprintf(strsql,"update Prov \
set \
kodkon='%s' \
where datd='%s' and nomd='%s' and \
pod=%s and kto='%s' and kodkon='%s'",
    data->kontr.ravno_filtr(),
    datad.ravno(),
    data->nomdok.ravno(),data->sklad.ravno(),gettext("МУ"),data->kontr_kor.ravno_filtr());
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(bros,"zapdokw %s",gettext("Ошибка корректировки записей !"));
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }

   }


  /*Корректировку в накладных и карточках выполняем если была 
  изменена дата или номер документа, склад менятся не может*/
  short d,m,g;
  iceb_u_rsdat(&d,&m,&g,data->datad.ravno(),1);
  
  if((d == data->dd_kor && m == data->md_kor && g == data->gd_kor )
  && iceb_u_SRAV(data->nomdok_kor.ravno(),data->nomdok.ravno(),0) == 0)
    return(0);



  /*Корректировка в накладной*/
//  printf("Корректировка записей в документе...\n");

  sprintf(strsql,"update Dokummat1 \
set \
datd='%s',\
nomd='%s',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
datad.ravno(),data->nomdok.ravno_filtr(),ktorab,tmm,
data->gd_kor,data->md_kor,data->dd_kor,data->skl_kor,data->nomdok_kor.ravno_filtr());

/*printw("strsql=%s\n",strsql);*/

  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(bros,"zapdokw %s",gettext("Ошибка корректировки записей !"));
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }


  /*Корректировка записей услуг*/
//  printf("Корректировка записей услуг...\n");

  sprintf(strsql,"update Dokummat3 \
set \
datd='%s',\
nomd='%s',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
datad.ravno(),data->nomdok.ravno_filtr(),ktorab,tmm,
data->gd_kor,data->md_kor,data->dd_kor,data->skl_kor,data->nomdok_kor.ravno_filtr());

/*printw("strsql=%s\n",strsql);*/

  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(bros,"zapdokw %s",gettext("Ошибка корректировки записей !"));
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }


  /*Корректировка в карточках*/
//  printf("Корректировка записей в карточках...\n");

  /*Только записи в которых дата подтверждения совпадает с датой документа
  изменяем дату подтверждения тоже*/
  sprintf(strsql,"update Zkart \
set \
datdp='%s',\
datd='%s',\
nomd='%s',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' \
and datdp='%04d-%02d-%02d'",
  datad.ravno(),datad.ravno(),
  data->nomdok.ravno_filtr(),ktorab,tmm,
  data->gd_kor,data->md_kor,data->dd_kor,data->skl_kor,
  data->nomdok_kor.ravno_filtr(),
  data->gd_kor,data->md_kor,data->dd_kor);

/*
printw("strsql=%s\n",strsql);
OSTANOV();
*/

  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(bros,"zapdokw %s",gettext("Ошибка корректировки записей !"));
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }


  /*Исправляем остальные записи*/
  sprintf(strsql,"update Zkart \
set \
datd='%s',\
nomd='%s',\
ktoi=%d,\
vrem=%ld \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
  datad.ravno(),data->nomdok.ravno_filtr(),ktorab,tmm,
  data->gd_kor,data->md_kor,data->dd_kor,data->skl_kor,data->nomdok_kor.ravno_filtr());


  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(bros,"zapdokw %s",gettext("Ошибка корректировки записей !"));
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }


 }


return(0);

}
