/* $Id: iceb_nomnak.c,v 1.14 2011-02-21 07:36:07 sasa Exp $ */
/*18.02.2008	02.02.1999	Белых А.И.	iceb_nomnak.c
Определение следующего номера накладной
*/
#include        <stdlib.h>
#include	"iceb_libbuh.h"

extern SQL_baza	bd;
extern short   mfnn; /*Метка формирования номера накладной
                      -1 - Автоматическая нумерация выключена
                       0 - Приходы и расходы - нумерация общая
                       1 - Приходы отдельно расходы отдельно
                      */
void iceb_nomnak1(short g,const char *skll,iceb_u_str *ndk,int tz,int metka,int,GtkWidget *wpredok);


void iceb_nomnak(const char *datad,
const char *skll, //Склад
iceb_u_str *ndk,  //Номер документа
int tz,
int metka, //0-мат. учет 1-услуги 2-кассовые ордера 3-командировочные расходы
int metka_lid_nol, //0-номер документа без лидирующих нолей 1-с лидирующими нолями
GtkWidget *wpredok)
{
 short d,m,g;
 iceb_u_rsdat(&d,&m,&g,datad,0);
 
 iceb_nomnak1(g,skll,ndk,tz,metka,metka_lid_nol,wpredok);

}
void iceb_nomnak(short g,
const char *skll, //Склад
iceb_u_str *ndk,  //Номер документа
int tz,
int metka, //0-мат. учет 1-услуги 2-кассовые ордера 3-командировочные расходы
int metka_lid_nol, //0-номер документа без лидирующих нолей 1-с лидирующими нолями
GtkWidget *wpredok)
{
 iceb_nomnak1(g,skll,ndk,tz,metka,metka_lid_nol,wpredok);

}
/****************************************************/

void iceb_nomnak1(short g,
const char *skll, //Склад
iceb_u_str *ndk,  //Номер документа
int tz,
int metka, //0-мат. учет 1-услуги 2-кассовые ордера 3-командировочные расходы
int metka_lid_nol, //0-номер документа без лидирующих нолей 1-с лидирующими нолями
GtkWidget *wpredok)
{
char		bros[512];
long            mpn;  /*Максимальный порядковый номер*/
long            pn;   /*Порядковый номер*/
char		skkk[20];
char		strsql[512];
int		kolstr=0;
SQL_str		row;
/*printf("iceb_nomnak-g=%d skll=%s mfnn=%d\n",g,skll,mfnn);*/

class iceb_clock kk(wpredok);

if(g == 0 && skll[0] == '\0')
  return;

if(mfnn == -1)
  return;

memset(skkk,'\0',sizeof(skkk));
if(mfnn == 1)
 sprintf(skkk,"%s",skll);  
pn=mpn=0;

if(metka == 0)
 sprintf(strsql,"select nomd from Dokummat where tip=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and sklad=%s",tz,g,g,skll);

if(metka == 1)
 sprintf(strsql,"select nomd from Usldokum where tp=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and podr=%s",tz,g,g,skll);

if(metka == 2)
 sprintf(strsql,"select nomd from Kasord where tp=%d and \
god=%d and kassa=%s",tz,g,skll);

if(metka == 3)
 sprintf(strsql,"select nomd from Ukrdok where god=%d",g);

/*printf("iceb_nomnak-%s\n",strsql);*/

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
//  printf("%d %s\n",kolstr,row[0]);  

  if(metka == 3)
   {
    pn=atoi(row[0]);
   }

  if(mfnn == 0)
   {
    iceb_u_pole(row[0],bros,2,'/');
    pn=atoi(bros);
   }

  if(mfnn == 1)
   {
    if(tz == 1)
     if(iceb_u_pole(row[0],bros,1,'+') == 0)
      if(iceb_u_SRAV(skkk,bros,0) == 0) 
       { 
        iceb_u_pole(row[0],bros,2,'+');
        pn=atoi(bros);
       }

    if(tz == 2)
     if(iceb_u_pole(row[0],bros,1,'-') == 0)
      if(iceb_u_SRAV(skkk,bros,0) == 0) 
       { 
        iceb_u_pole(row[0],bros,2,'-');
        pn=atoi(bros);
       } 
   }

  if(metka == 3)
   {
    pn=atoi(row[0]);
   }

  if(pn > mpn)
   mpn=pn;

 }
//printw("kolstr=%d mpn=%d\n",kolstr,mpn);
//refresh();
memset(strsql,'\0',sizeof(strsql));
if(mfnn == 0) //Приходы и расходы - общая нумерация
 {
  if(metka_lid_nol == 0)
   {
    if(metka != 3)
     sprintf(strsql,"%s/%ld",skll,mpn+1);
    else
     sprintf(strsql,"%ld",mpn+1);
   }
  else
   {
    if(metka != 3)
     sprintf(strsql,"%s/%05ld",skll,mpn+1);
    else
     sprintf(strsql,"%05ld",mpn+1);
   }
 }
 
if(mfnn == 1)
 {
  if(metka_lid_nol == 0)
   {
    if(tz == 1)
      sprintf(strsql,"%s+%ld",skll,mpn+1);
    if(tz == 2)
      sprintf(strsql,"%s-%ld",skll,mpn+1);
   }
  else
   {
    if(tz == 1)
      sprintf(strsql,"%s+%05ld",skll,mpn+1);
    if(tz == 2)
      sprintf(strsql,"%s-%05ld",skll,mpn+1);
   }
 }
ndk->new_plus(strsql);  
}
