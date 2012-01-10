/* $Id: iceb_zapmpr.c,v 1.12 2011-03-09 08:21:27 sasa Exp $ */
/*01.03.2011	32.09.1998	Белых А.И.	iceb_zapmpr.c
Запись проводок в оперативную память
*/
#include        <stdlib.h>
#include        <math.h>
#include        "iceb_libbuh.h"


void iceb_zapmpr(short g,short m,short d, //Дата
const char *sh, //Счет
const char *shk, //Счет корреспондент
const char *kor1, //Контрагент счета
const char *kor2, //Контрагент счета корреспондента
double deb, //Сумма в дебете
double kre, //Сумма в кредите
const char *kom, //Коментарий
int kolpr, //Количество проводок 1- для внебалансового счета 2-для балансового
int kekv,  //код классификации экономических затрат
class iceb_u_spisok *sp_prov, /*Список проводок*/
class iceb_u_double *sum_prov_dk, /*Суммы по дебету/кредиту проводов*/
FILE *ff_prot)
{
char		bros[1024];
int		metkaprov=0;

if(kolpr == 1)
  metkaprov=-1;


//Округлять будем при выполнении проводок
//deb=okrug(deb,0.01);
//kre=okrug(kre,0.01);

if(fabs(deb) > 0.009 && fabs(kre) > 0.009)
 {
  iceb_u_str repl;
  sprintf(bros,"%s-Дебет (%.2f) и кредит (%.2f) не равны нулю !",__FUNCTION__,deb,kre);
  repl.plus(bros);
  repl.ps_plus("Попытка сделать логически не верную проводку !!");
  iceb_menu_soob(&repl,NULL);
  return;
 }


bros[0]='\0';
if(fabs(deb) > 0.009)
 sprintf(bros,"%d.%d.%d|%s|%s|%s|%s|%s|%d|%d|1|%d",
 d,m,g,sh,shk,kor1,kor2,kom,kolpr,metkaprov,kekv);
if(fabs(kre) > 0.009)
 sprintf(bros,"%d.%d.%d|%s|%s|%s|%s|%s|%d|%d|2|%d",
 d,m,g,sh,shk,kor1,kor2,kom,kolpr,metkaprov,kekv);

if(bros[0] == '\0')
  return;

if(ff_prot != NULL)
 fprintf(ff_prot,"%s:%s => deb=%f kre=%f\n",__FUNCTION__,bros,deb,kre);
 
int nomer=0;

if((nomer=sp_prov->find(bros)) < 0)
 {
  sp_prov->plus(bros);
  sum_prov_dk->plus(deb,-1);
  sum_prov_dk->plus(kre,-1);
 }
else
 {
  sum_prov_dk->plus(deb,nomer*2);
  sum_prov_dk->plus(kre,nomer*2+1);
 }

}
