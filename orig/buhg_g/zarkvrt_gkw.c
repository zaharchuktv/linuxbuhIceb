/*$Id: zarkvrt_gkw.c,v 1.1 2011-08-29 07:13:44 sasa Exp $*/
/*05.05.2011	21.04.2005	Белых А.И.	zarkvrt_gkw.c
Поиск проводок для формы 1дф
Возвращаем 0-если есть хоть одна сумма
           1-если нет ни одной суммы
*/
#include <stdlib.h>
#include "buhg_g.h"

extern SQL_baza bd;

int zarkvrt_gk_r(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *shet_deb,
const char *shet_kre,
iceb_u_spisok *kod_kom,
iceb_u_double *suma_vip,
iceb_u_double *suma_nah,
int priz_nv,int,FILE*,
GtkWidget *wredok);



int zarkvrt_gkw(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *shkas_shkom,
const char *prist_k_tabnom,
int kod_doh,
const char *imatmptab,
FILE *ff_prot,
GtkWidget *wpredok)
{

if(shkas_shkom[0] == '\0')
 {
  fprintf(ff_prot,"Не введены настройки для поиска проводок\n");
  return(1);
 } 

//printw("Код дохода:%d\nПошук проводок по рахунках:%s\n",kod_doh,shkas_shkom);
//refresh();
fprintf(ff_prot,"Код дохода:%d\nПошук проводок по рахунках:%s\n",kod_doh,shkas_shkom);

char strsql[1024];
char bros[512];
char sheta_doh[1024];
char sheta_pnal[1024];
memset(sheta_doh,'\0',sizeof(sheta_doh));
memset(sheta_pnal,'\0',sizeof(sheta_pnal));

if(iceb_u_polen(shkas_shkom,sheta_doh,sizeof(sheta_doh),1,'>') != 0)
  strncpy(sheta_doh,shkas_shkom,sizeof(sheta_doh)-1);
else
  iceb_u_polen(shkas_shkom,sheta_pnal,sizeof(sheta_pnal),2,'>');
  
int kolpol=iceb_u_pole2(sheta_doh,';');

int  priz_nv=0; //1-начислено 2-выплачено
int  priz_deb_kre; //1-дебет 2-кредит
class iceb_u_str shet_kre("");
class iceb_u_str shet_deb("");

class iceb_u_spisok kod_kom;
class iceb_u_double suma_vip; //Сумма выплаченного дохода
class iceb_u_double suma_nah; //Сумма начисленного дохода
class iceb_u_double podoh_nah; //Сумма начисленного подоходного налога
class iceb_u_double podoh_vip; //Сумма выплаченого подоходного налога
//Смотрим доход начисленный и выплаченный
for(int nn=0; nn < kolpol ; nn++)
 {

  iceb_u_polen(sheta_doh,strsql,sizeof(strsql),nn+1,';');
  if(strsql[0] == '\0')
   continue;

  priz_nv=0;
  iceb_u_polen(strsql,bros,sizeof(bros),1,':');

  if(bros[0] == '+')
   priz_nv=1;

  if(bros[0] == '-')
   priz_nv=2;

  if(bros[0] == '*')
   priz_nv=3;

  if(priz_nv == 0)
   continue;

  iceb_u_polen(strsql,bros,sizeof(bros),2,':');
  if(bros[0] == 'd')
   priz_deb_kre=1;
  else
   priz_deb_kre=2;
    
  iceb_u_polen(strsql,&shet_deb,3,':');
  iceb_u_polen(strsql,&shet_kre,4,':');
  zarkvrt_gk_r(dn,mn,gn,dk,mk,gk,shet_deb.ravno(),shet_kre.ravno(),&kod_kom,&suma_vip,&suma_nah,priz_nv,priz_deb_kre,ff_prot,wpredok);
   
 }

//Списки должны быть с одинаковым количеством элементов
podoh_vip.make_class(suma_vip.kolih());
podoh_nah.make_class(suma_nah.kolih());

if(sheta_pnal[0] != '\0')
 {
  kolpol=iceb_u_pole2(sheta_pnal,';');
  //Смотрим подоходный налог начисленный и выплаченный
  for(int nn=0; nn < kolpol ; nn++)
   {

    iceb_u_polen(sheta_pnal,strsql,sizeof(strsql),nn+1,';');
    if(strsql[0] == '\0')
     continue;

    priz_nv=0;

    iceb_u_polen(strsql,bros,sizeof(bros),1,':');

    if(bros[0] == '+')
     priz_nv=1;

    if(bros[0] == '-')
     priz_nv=2;

    if(bros[0] == '*')
     priz_nv=3;

    if(priz_nv == 0)
     continue;

    iceb_u_polen(strsql,bros,sizeof(bros),2,':');
    if(bros[0] == 'd')
     priz_deb_kre=1;
    else
     priz_deb_kre=2;
      
    iceb_u_polen(strsql,&shet_deb,3,':');
    iceb_u_polen(strsql,&shet_kre,4,':');
    zarkvrt_gk_r(dn,mn,gn,dk,mk,gk,shet_deb.ravno(),shet_kre.ravno(),&kod_kom,&podoh_vip,&podoh_nah,priz_nv,priz_deb_kre,ff_prot,wpredok);
     
   }
 }  

//Списки должны быть с одинаковым количеством элементов
int raznica=podoh_vip.kolih()-suma_vip.kolih();
if(raznica > 0)
 for(int i=0; i < raznica; i++)
  {
    suma_vip.plus(0.,-1);
    suma_nah.plus(0.,-1);
  }

//printw("Записываем данные:%d\n",kod_kom.kolih());
//refresh();
class iceb_u_str fio;
char inn[32];
char kod[32];
double suma_v;
double suma_n;
double podoh_n;
double podoh_v;

SQL_str row;
SQLCURSOR cur;
for(int nom=0; nom < kod_kom.kolih(); nom++)
 {
  memset(inn,'\0',sizeof(inn));
  fio.new_plus("");
  memset(kod,'\0',sizeof(kod));
  strncpy(kod,kod_kom.ravno(nom),sizeof(kod)-1);
  
  if(iceb_u_strstrm(kod,prist_k_tabnom) == 1)
   {
    sprintf(strsql,"select fio,inn from Kartb where tabn=%s",&kod[strlen(prist_k_tabnom)]);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
     {
      fio.new_plus(row[0]);
      strncpy(inn,row[1],sizeof(inn)-1);
     }
    
   }
  else
   {
    sprintf(strsql,"select naikon,kod,regnom from Kontragent where kodkon='%s'",kod);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1 )
     {
      if(row[2][0] == '\0')
       continue;
      fio.new_plus(row[0]);
      strncpy(inn,row[1],sizeof(inn)-1);
     }
    else
     continue;
   } 
  suma_v=suma_vip.ravno(nom);
  suma_n=suma_nah.ravno(nom);
  podoh_v=podoh_vip.ravno(nom);
  podoh_n=podoh_nah.ravno(nom);

  
  if(iceb_u_strlen(inn) < 10)
   {
    sprintf(strsql,"%s %s %s!",gettext("Идентификационный номер имеет меньше 10 знаков"),inn,kod);
    iceb_menu_soob(strsql,wpredok);
   }   

//  printw("%*s %s\n",iceb_u_kolbait(6,kod),kod,fio.ravno());
  zarkvrt_zapvtabl(imatmptab,inn,kod_doh,"",suma_n,suma_v,podoh_n,podoh_v,"0000-00-00","0000-00-00",fio.ravno_filtr(),kod,ff_prot,wpredok);

 }

return(0);

}
/********************************/
/*чтение проводок*/
/**********************/

int zarkvrt_gk_r(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *shet_deb,
const char *shet_kre,
iceb_u_spisok *kod_kom,
iceb_u_double *suma_vip,
iceb_u_double *suma_nah,
int priz_nv,
int priz_deb_kre,
FILE *ff_prot,
GtkWidget *wpredok)
{
char strsql[512];
//printw("%s\n",__FUNCTION__);
//refresh();
//Берём строго за квартал
if(priz_deb_kre == 1)  
    sprintf(strsql,"select sh,shk,kodkon,deb,datp from Prov where datp >= '%04d-%d-%d' and \
datp <= '%04d-%d-%d' and kodkon <> '' and val=0 and deb <> 0. \
order by datp asc",gn,mn,1,gn,mn+2,31); 
else
    sprintf(strsql,"select sh,shk,kodkon,kre,datp from Prov where datp >= '%04d-%d-%d' and \
datp <= '%04d-%d-%d' and kodkon <> '' and val=0 and kre <> 0. \
order by datp asc",gn,mn,1,gn,mn+2,31); 

fprintf(ff_prot,"shet_deb=%s\nshet_kre=%s\n%s\n",shet_deb,shet_kre,strsql);


SQL_str row;
SQLCURSOR cur;
int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
//printw("kolstr=%d\n",kolstr);
if(kolstr == 0)
 {
//  printw("Нужных проводок не обнаружено\n");
  fprintf(ff_prot,"Нужных проводок не обнаружено\n");
  return(1);
 }

//int kolstr1=0;
int nomer;
double deb;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);

  if(iceb_u_proverka(shet_deb,row[0],1,0) != 0)
     continue;
  if(iceb_u_proverka(shet_kre,row[1],1,0) != 0)
     continue;

  fprintf(ff_prot,"%s %-6s %-6s %-6s %10s\n",row[4],row[0],row[1],row[2],row[3]);
  
  if((nomer=kod_kom->find_r(row[2])) < 0)
    kod_kom->plus(row[2]);


  
  deb=atof(row[3]);
  //Количество элементов массива должно быть равным
  if(priz_nv == 1)
   {
    suma_nah->plus(deb,nomer);  
    if(nomer < 0 )
     suma_vip->plus(0.,nomer);  
   }
  if(priz_nv == 2)
   {
    suma_vip->plus(deb,nomer);  
    if(nomer < 0 )
      suma_nah->plus(0.,nomer);  
   }

  if(priz_nv == 3)
   {
    suma_vip->plus(deb,nomer);  
    suma_nah->plus(deb,nomer);  
   }

 }

return(0);
}
