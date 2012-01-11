/*$Id: pdokmuw.c,v 1.6 2011-02-21 07:35:55 sasa Exp $*/
/*22.01.2010	23.12.2009	Белых А.И.	pdokmuw.c
Просмотр документа из подсистемы "Материальный учёт"
Если вернули 0-документ нашли
             1-нет
*/
#include <stdlib.h>
#include	<pwd.h>
#include	<time.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include <unistd.h>

extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int pdokmuw(const char *datadok,int tipz,int sklad,const char *nomdok,GtkWidget *wpredok)
{
short dd,md,gd;
char strsql[512];
char bros[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
FILE *ff;
char imaf[56];
class iceb_u_str naim("");

iceb_u_rsdat(&dd,&md,&gd,datadok,1);

sprintf(strsql,"select kontr,nomnn,kodop,ktoi,vrem,datpnn,pn,mo from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and tip=%d",
gd,md,dd,sklad,nomdok,tipz);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s\n%d.%d.%d %d %s %d",gettext("Не найден документ!"),
  dd,md,gd,sklad,nomdok,tipz);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

float pnds=atof(row[6]);

sprintf(imaf,"pdokmu%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }

fprintf(ff,"%s\n",gettext("Материальный учёт"));
if(tipz == 1)
 fprintf(ff,"%s\n",gettext("Приход"));
if(tipz == 2)
 fprintf(ff,"%s\n",gettext("Расход"));
 
fprintf(ff,"%s:%d.%d.%d %s:%s\n",
gettext("Дата документа"),dd,md,gd,
gettext("Номер документа"),nomdok);

naim.new_plus("");
sprintf(strsql,"select naik from Sklad where kod=%d",sklad);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%d %s\n",gettext("Склад"),sklad,naim.ravno());

naim.new_plus("");
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[0]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),row[0],naim.ravno());

naim.new_plus("");
if(tipz == 1)
  sprintf(strsql,"select naik from Prihod where kod='%s'",row[2]);
if(tipz == 2)
  sprintf(strsql,"select naik from Rashod where kod='%s'",row[2]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim.new_plus(row1[0]);

fprintf(ff,"%s:%s %s\n",gettext("Код операции"),row[2],naim.ravno());


fprintf(ff,"%s:%s\n",gettext("Записал"),iceb_kszap(row[4],1,wpredok));
fprintf(ff,"%s:%s\n",gettext("Дата записи"),iceb_u_vremzap(row[3]));

if(row[1][0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Номер налоговой накладной"),row[1]);
if(row[5][0] != '0')
  fprintf(ff,"%s:%s\n",gettext("Дата получения налоговой накладной"),iceb_u_datzap(row[5]));
if(atoi(row[7]) == 0)
 fprintf(ff,"%s\n",gettext("Документ неоплачен"));
if(atoi(row[7]) == 1)
 fprintf(ff,"%s\n",gettext("Документ оплачен"));


sprintf(strsql,"select nomerz,sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s'",
gd,sklad,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 }
int nomerz=0;
double suma_nds=0.;
double suma_kor=0.;
int mnds=0;
while(cur.read_cursor(&row) != 0)
 {
  nomerz=atoi(row[0]);
  switch (nomerz)
   {
    case 1:
     iceb_u_polen(row[1],bros,sizeof(bros),1,'#');
     fprintf(ff,"%s:%s ",gettext("Доверенность"),bros);
     iceb_u_polen(row[1],bros,sizeof(bros),2,'#');
     fprintf(ff,"%s:%s\n",gettext("Дата"),bros);
     
     break;

    case 2:
     fprintf(ff,"%s:%s\n",gettext("Через кого"),row[1]);
     break; 

    case 3:
     if(tipz == 2)
      fprintf(ff,"%s:%s\n",gettext("Основание"),row[1]);
     if(tipz == 1)
      fprintf(ff,"%s:%s\n",gettext("Коментарий"),row[1]);
     break;

    case 6:
      suma_nds=atof(row[1]);
      fprintf(ff,"%s:%s\n",gettext("Введена сумма НДС"),row[1]);
      break;

    case 7:
      fprintf(ff,"%s:%s\n",gettext("Условие продажи"),row[1]);
      break;

    case 8:
      naim.new_plus("");       
      sprintf(strsql,"select naik from Foroplat where kod='%s'",row[1]);
      if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
       naim.new_plus(row1[0]);
      fprintf(ff,"%s:%s %s\n",gettext("Форма оплаты"),row[1],naim.ravno());
      break;

    case 9:
      fprintf(ff,"%s:%s\n",gettext("Дата оплаты"),row[1]);
      break;

    case 10:
      fprintf(ff,"%s:%s\n",gettext("Государственный номер автомобиля"),row[1]);
      break;

    case 11:
      mnds=atoi(row[1]);
      break;

    case 13:
      suma_kor=atof(row[1]);
      break;

    case 14:
      fprintf(ff,"%s:%s\n",gettext("Дата выписки налоговой накладной"),row[1]);
      break;

    case 15:
      fprintf(ff,"%s:%s\n",gettext("Номер путевого листа"),row[1]);
      break;

    case 16:
      fprintf(ff,"%s:%s\n",gettext("Марка автомобиля"),row[1]);
      break;

    case 17:
      fprintf(ff,"%s:%s\n",gettext("Марка и номер прицепа"),row[1]);
      break;

    case 18:
      fprintf(ff,"%s:%s\n",gettext("Перевозчик груза"),row[1]);
      break;

    case 19:
      fprintf(ff,"%s:%s\n",gettext("Заказчик"),row[1]);
      break;

    case 20:
      fprintf(ff,"%s:%s\n",gettext("Фамилия водителя"),row[1]);
      break;

    case 21:
      fprintf(ff,"%s:%s\n",gettext("Пункт загрузки"),row[1]);
      break;

    case 22:
      fprintf(ff,"%s:%s\n",gettext("Пункт разгрузки"),row[1]);
      break;

    case 23:
      fprintf(ff,"%s:%s\n",gettext("Акцизный сбор"),row[1]);
      break;

    case 24:
      fprintf(ff,"%s:%s\n",gettext("Лицензия на алкоголь"),row[1]);
      break;

    case 25:
      fprintf(ff,"%s:%s\n",gettext("Лицензия на алкоголь"),row[1]);
      break;
   }  
 }


fprintf(ff,"\
-------------------------------------------------------------------------------\n");
fprintf(ff,"\
Код м.|  Наименование материалла      |Eд.изм.|Количество|  Цена    | Сумма    |\n");
fprintf(ff,"\
-------------------------------------------------------------------------------\n");

sprintf(strsql,"select kodm,kolih,cena,ei,voztar,ktoi,vrem,shet,dnaim from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,sklad,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
char naim_mat[112];
double kolih=0.;
double cena=0.;
double suma=0.;
double itogo=0.;
if(kolstr >= 0)
while(cur.read_cursor(&row) != 0)
 {
  memset(naim_mat,'\0',sizeof(naim_mat));
  sprintf(strsql,"select naimat from Material where kodm=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   strncpy(naim_mat,row1[0],sizeof(naim_mat)-1);

  if(row[8][0] != '\0')
   {
    strcat(naim_mat," ");
    strcat(naim_mat,row[8]);
   }  

  kolih=atof(row[1]);
  cena=atof(row[2]);
  cena=iceb_u_okrug(cena,okrcn);
  suma=kolih*cena;  
  itogo+=suma;
  
  fprintf(ff,"%6s %-*.*s %-*.*s %10.10g %10.10g %10.2f %s %s\n",
  row[0],
  iceb_u_kolbait(30,naim_mat),
  iceb_u_kolbait(30,naim_mat),
  naim_mat,
  iceb_u_kolbait(7,row[3]),
  iceb_u_kolbait(7,row[3]),
  row[3],
  kolih,cena,suma,
  iceb_u_vremzap(row[6]),
  iceb_kszap(row[5],1,wpredok));

  if(iceb_u_strlen(naim_mat) > 30)
   fprintf(ff,"%6s %s\n","",iceb_u_adrsimv(30,naim_mat));
 }

sprintf(strsql,"select ei,kolih,cena,nds,naimu,ktoi,vrem,ku from Dokummat3 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",gd,md,dd,sklad,nomdok);
 
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[1]);
  cena=atof(row[2]);
  cena=iceb_u_okrug(cena,okrcn);
  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;
  itogo+=suma;

  memset(naim_mat,'\0',sizeof(naim_mat));
  if(atoi(row[7]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[7]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
     strncpy(naim_mat,row1[0],sizeof(naim_mat));
   }  

  if(naim_mat[0] == '\0')
   strncpy(naim_mat,row[4],sizeof(naim_mat)-1);  
  else
   {
    strcat(naim_mat," ");
    strcat(naim_mat,row[4]);
   }

  fprintf(ff,"%6s %-*.*s %-*.*s %10.10g %10.10g %10.2f %s %s\n",
  row[7],
  iceb_u_kolbait(30,naim_mat),
  iceb_u_kolbait(30,naim_mat),
  naim_mat,
  iceb_u_kolbait(7,row[0]),
  iceb_u_kolbait(7,row[0]),
  row[0],
  kolih,cena,suma,
  iceb_u_vremzap(row[6]),
  iceb_kszap(row[5],1,wpredok));

  if(iceb_u_strlen(naim_mat) > 30)
   fprintf(ff,"%6s %s\n","",iceb_u_adrsimv(30,naim_mat));

 }
fprintf(ff,"\
-------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",
iceb_u_kolbait(67,gettext("Итого")),
gettext("Итого"),itogo);

if(suma_kor < 0.)
  fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(67,gettext("Скидка")),gettext("Скидка"),suma_kor);
if(suma_kor > 0.)
  fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(67,gettext("Надбавка")),gettext("Надбавка"),suma_kor);

double oplata=itogo-suma_kor;

if(mnds == 0) 
 {
  double nds=0.;

  if(suma_nds != 0.)
   nds=suma_nds;
  else
    nds=oplata*pnds/100.;

  oplata=oplata+nds;
  fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(67,gettext("НДС")),gettext("НДС"),nds);
 }    
if(itogo != oplata)
 fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(67,gettext("Итого")),gettext("Итого"),oplata);
iceb_podpis(ff,wpredok);
fclose(ff);

iceb_prosf(imaf,wpredok);
sleep(1);
unlink(imaf);

return(0);
}

