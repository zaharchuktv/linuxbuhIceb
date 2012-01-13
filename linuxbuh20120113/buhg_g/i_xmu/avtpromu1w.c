/*$Id: avtpromu1w.c,v 1.10 2011-09-05 08:18:27 sasa Exp $*/
/*31.08.2011	14.04.2009	Белых А.И	avtpromu1.c
Автоматическое выполнение проводок для приходных документов материального учёта
*/
#include <stdlib.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern double   okrg1; /*Округление*/
extern short    vtara; /*Код группы возвратная тара*/
extern iceb_u_double snn; /*Суммы по накладных*/
extern double	*kofusl; //Коэффициенты распределения услуг
extern double	sumusl; //Сумма услуг по документа
extern SQL_baza bd;

void avtpromu1(int tipz,short dd,short md,short gd,const char *nomdok,FILE *ff_prot,
GtkWidget *wpredok)
{
class iceb_avp avp;

SQL_str row,row1;
class SQLCURSOR cur,cur1;
char strsql[512];
if(ff_prot != NULL)
 fprintf(ff_prot,"Автоматическое выполнение проводок для приходных документов\n\
Дата документа:%d.%d.%d Номер документа:%s\n",dd,md,gd,nomdok);
 
/*Читаем шапку документа*/
sprintf(strsql,"select sklad,kontr,kodop,nomon,pn from Dokummat where datd='%04d-%02d-%02d' and nomd='%s' and tip=%d",
gd,md,dd,nomdok,tipz);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %d.%d.%d %s",gettext("Не найдена шапка документа!"),dd,md,gd,nomdok);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

int sklad=atoi(row[0]);

if(iceb_u_SRAV(row[1],"00-",1) == 0)
  avp.kontr_v_dok.new_plus("00");
else
  avp.kontr_v_dok.new_plus(row[1]);

avp.kodop.new_plus(row[2]);
class iceb_u_str nomdok_par(row[3]);
float pnds=atof(row[4]);

/*Номер документа поставщика*/
avp.nomer_dok.new_plus(row[3]);

avp.suma_nds_vv=0.; /*Сумма НДС введённая в документ вручную*/
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=6",
gd,sklad,nomdok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  avp.suma_nds_vv=atof(row[0]); 

double suma_korrekt=0.; /*Сумма корректировки к документа*/
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=13",
gd,sklad,nomdok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  suma_korrekt=atof(row[0]); 

int metka_nds=0; /*Метка документа с НДС*/
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=11",
gd,sklad,nomdok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  metka_nds=atoi(row[0]); 

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"Склад:%d Контрагент:%s Код операции:%s Процент НДС:%f Номер парного документа:%s\n\
Сумма НДС введённая вручную:%.2f Сумма корректировки к документа:%.2f\n\
Метка НДС=%d",  
  sklad,avp.kontr_v_dok.ravno(),avp.kodop.ravno(),pnds,nomdok_par.ravno(),
  avp.suma_nds_vv,suma_korrekt,metka_nds);
 }
int kolstr=0;
sprintf(strsql,"select nomkar,kolih,cena,nds,kodm,nomkarp,shet from \
Dokummat1 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' \
order by kodm asc",gd,md,dd,sklad,nomdok);  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено ни одной записи в документе!\n");
  return;
 }
double prockor=0.;
if(suma_korrekt != 0.)
  prockor=suma_korrekt*100./snn.suma();


class iceb_u_spisok sp_prov;
class iceb_u_double sum_prov_dk;
int kgrm=0;
int nomkart=0;
double kolihpod=0.;
double cena_uheta=0.;
double suma_uheta=0.;
double i_suma_uheta=0.;
double cena_v_dok=0.;
double suma_po_dok=0.;
double i_suma_po_dok=0.;
double itogo_pod=0.;
short dp1=0,mp1=0,gp1=0;
double nds=0.;
char bros[512];
while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"Прочитали запись в документе=%s %s %s %s %s %s %s\n\
-------------------------------------------------------------------------\n",
    row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
   }
  /*Читаем карточку материалла*/
  nomkart=atoi(row[0]);

  if(nomkart == 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Запись не привязана к карточке! Пропускаем\n");
    continue;

   }
  nds=atof(row[3]);
  cena_v_dok=atof(row[2]);
  avp.shetsp.new_plus(row[6]);  


  sprintf(strsql,"select shetu,krat,cena from Kart where sklad=%d and nomk=%d",
  sklad,nomkart);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"%s-Не нашли карточки %d склад %d\n",__FUNCTION__,nomkart,sklad);
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }

  cena_uheta=atof(row1[2]);
  avp.shetuh.new_plus(row1[0]);	


  /*Определяем группу материалла*/
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[4]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(bros,"%s-%s %s",__FUNCTION__,gettext("Не найден код материалла"),row[4]);
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s\n",row1[1]);     
  kgrm=atoi(row1[0]);


  /*Читаем количество в карточке*/
  
     
  sprintf(strsql,"select datdp,kolih from Zkart where sklad=%d and \
nomk=%d and nomd='%s' and datd='%d-%02d-%02d' order by datdp asc",
  sklad,nomkart,nomdok,gd,md,dd);
  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,"prospr",strsql,wpredok);
    continue;
   }

  if(kolstr == 0)
    continue;
  i_suma_uheta=0.;
  i_suma_po_dok=0.;
  kolihpod=0.;
  if(ff_prot != NULL)
    fprintf(ff_prot,"Подтверждено:\n");
  while(cur1.read_cursor(&row1) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s %s\n",row1[0],row1[1]);
    kolihpod=atof(row1[1]);
    suma_uheta=kolihpod*cena_uheta;
    suma_uheta=iceb_u_okrug(suma_uheta,okrg1);
    i_suma_uheta+=suma_uheta;

    suma_po_dok=atof(row1[1])*cena_v_dok;
    suma_po_dok=iceb_u_okrug(suma_po_dok,okrg1);
    i_suma_po_dok+=suma_po_dok;
 
    iceb_u_rsdat(&dp1,&mp1,&gp1,row1[0],2);

    if(iceb_u_sravmydat(avp.dp,avp.mp,avp.gp,dp1,mp1,gp1) < 0)
     {
      avp.dp=dp1; avp.mp=mp1; avp.gp=gp1;
     }
   }
  itogo_pod+=i_suma_uheta;

  double kofusl=i_suma_po_dok/snn.suma();
  avp.suma_bez_nds=i_suma_po_dok+sumusl*kofusl;
  avp.suma_bez_nds+=avp.suma_bez_nds*prockor/100.;

  if(nds < 0. || kgrm == vtara || metka_nds > 0)
    avp.suma_nds=0.;
  else
    avp.suma_nds=i_suma_po_dok*pnds/100.;

  avp.suma_uheta=i_suma_uheta;
  avp.suma_s_nds=avp.suma_bez_nds+avp.suma_nds;

  if(tipz == 1)
   iceb_avp(&avp,"avtpromu1.alx",&sp_prov,&sum_prov_dk,ff_prot,wpredok);
  if(tipz == 2)
   iceb_avp(&avp,"avtpromu2.alx",&sp_prov,&sum_prov_dk,ff_prot,wpredok);
  
 }
iceb_zapmpr1(nomdok,avp.kodop.ravno(),sklad,time(NULL),gettext("МУ"),dd,md,gd,tipz,&sp_prov,&sum_prov_dk,ff_prot,wpredok);
if(itogo_pod == 0.)
 {
  sprintf(strsql,"%s\n%d.%d.%d %s",gettext("Документ не подтверждён!"),dd,md,gd,nomdok);
  iceb_menu_soob(strsql,wpredok);
  return;
 }
 
}

