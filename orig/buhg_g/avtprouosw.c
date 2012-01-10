/*$Id: avtprouosw.c,v 1.8 2011-02-21 07:35:51 sasa Exp $*/
/*11.09.2010	13.06.2009	Белых А.И.	avtprouosw.c
Автоматическое выполение проводок для подсистемы "Учёт основных средств"
*/
#include <stdlib.h>
#include "buhg_g.h"

extern SQL_baza bd;

void avtprouosw(int tipz,short dd,short md,short gd,const char *nomdok,FILE *ff_prot,GtkWidget *wpredok)
{
class iceb_avp avp;
avp.dp=dd;
avp.mp=md;
avp.gp=gd;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[512];
if(ff_prot != NULL)
 {
  fprintf(ff_prot,"Автоматическое выполнение проводок\n");
  if(tipz == 1)
   fprintf(ff_prot,"Приходный документ\n");
  if(tipz == 2)
   fprintf(ff_prot,"Расходный документ\n");
  fprintf(ff_prot,"Дата документа:%d.%d.%d Номер документа:%s\n",dd,md,gd,nomdok);
 } 
/*Читаем шапку документа*/
sprintf(strsql,"select kodop,kontr,podr,nds,pn from Uosdok where datd='%04d-%02d-%02d' and nomd='%s' and tipz=%d",
gd,md,dd,nomdok,tipz);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %d.%d.%d %s",gettext("Не найдена шапка документа!"),dd,md,gd,nomdok);
  iceb_menu_soob(strsql,wpredok);
  return;
 }
avp.kodop.new_plus(row[0]);

if(iceb_u_SRAV(row[1],"00-",1) == 0)
  avp.kontr_v_dok.new_plus("00");
else
  avp.kontr_v_dok.new_plus(row[1]);

int podr=atoi(row[2]);
int metka_nds=atoi(row[3]);
float pnds=atof(row[4]);
avp.nomer_dok.new_plus(nomdok);

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"Контрагент:%s Код операции:%s Процент НДС:%f Метка НДС=%d\n",  
  avp.kontr_v_dok.ravno(),avp.kodop.ravno(),pnds,metka_nds);
 }
class iceb_u_str imaf_nastpr;

if(tipz == 1)
  imaf_nastpr.plus("avtprouos1.alx");
if(tipz == 2)
  imaf_nastpr.plus("avtprouos2.alx");
int metka_nalu=0;
if(iceb_poldan("Выполнять проводки по данным налогового учёта",strsql,imaf_nastpr.ravno(),wpredok) == 0)
 if(iceb_u_SRAV("Вкл",strsql,1) == 0)
  metka_nalu=1;  

int kolstr=0;
if(metka_nalu == 0)
 {
  sprintf(strsql,"select innom,bsby,izby,shs from Uosdok1 where datd='%d-%02d-%02d' and nomd='%s' and podt=1",gd,md,dd,nomdok);  
  if(ff_prot != NULL)
   fprintf(ff_prot,"Проводки по данным бух.учёта\n");
   
 }

if(metka_nalu == 1)
 {
  sprintf(strsql,"select innom,bs,iz,shs from Uosdok1 where datd='%d-%02d-%02d' and nomd='%s' and podt=1",gd,md,dd,nomdok);  
  if(ff_prot != NULL)
   fprintf(ff_prot,"Проводки по данным налогового учёта\n");
 }
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено ни одной подтверждённой записи в документе!\n");
  return;
 }
class iceb_u_spisok sp_prov;
class iceb_u_double sum_prov_dk;

double suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"Прочитали запись в документе=%s %s %s %s\n\
-------------------------------------------------------------------------\n",
    row[0],row[1],row[2],row[3]);
   }
  suma=atof(row[1]);
  avp.shetsp.new_plus(row[3]);

  /*Определяем счёт учёта*/
  sprintf(strsql,"select shetu from Uosinp where innom=%s and ( god < %d or (god=%d and mes <= %d)) order by god,mes desc limit 1",
  row[0],gd,gd,md);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) <= 0)
   {
    if(ff_prot != NULL)
     {
      fprintf(ff_prot,"Не определили счёт учёта для %s!!\n",row[0]);
      fprintf(ff_prot,"Запрос=%s\n",strsql);
     }
    sprintf(strsql,"Не определили счёт учёта для %s!!",row[0]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }  
  avp.shetuh.new_plus(row1[0]);

  avp.suma_bez_nds=suma;
  avp.suma_iznosa=atof(row[2]);

  if(metka_nds == 0)
    avp.suma_nds=suma*pnds/100.;
  else
    avp.suma_nds=0.;
    
  avp.suma_s_nds=avp.suma_bez_nds+avp.suma_nds;
  avp.suma_uheta=suma;

  iceb_avp(&avp,imaf_nastpr.ravno(),&sp_prov,&sum_prov_dk,ff_prot,wpredok);

 }

iceb_zapmpr1(nomdok,avp.kodop.ravno(),podr,time(NULL),gettext("УОС"),dd,md,gd,tipz,&sp_prov,&sum_prov_dk,ff_prot,wpredok);

 
}
