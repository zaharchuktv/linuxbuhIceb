/*$Id: zar_eks_ko.c,v 1.13 2011-02-21 07:35:59 sasa Exp $*/
/*23.06.2009	20.10.2006	Белых А.И.	zar_eks_ko.c
Экспорт кассовых ордеров в подсистему "Учет кассовых ордеров".
*/
#include <stdlib.h>
#include "buhg_g.h"
#include "l_nahud.h"
#include "zar_eks_ko.h"

int zar_eks_ko_v(class zar_eks_ko_rek *rek,GtkWidget *wpredok);

extern SQL_baza bd;
extern char	pktn[16]; /*Приставка к табельному номеру*/

void zar_eks_ko(int prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{

class zar_eks_ko_rek data;
data.prn=prn;
data.zapros.new_plus(zapros);

if(zar_eks_ko_v(&data,wpredok) != 0)
 return;

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);


SQLCURSOR cur,cur1;
int kolstr=0;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),wpredok);
  return;
 }

short dko=0,mko=0,gko=0;
iceb_u_rsdat(&dko,&mko,&gko,data.data.ravno(),1);

SQL_str	row1;
char fio[512];
int kolstr1=0;
char podr[20];
char podrz[20];
class iceb_u_str nomd;
double suma=0.;
char tabnomz[20];
char kodkont[20];
char strsql[512];

memset(podrz,'\0',sizeof(podrz));
memset(tabnomz,'\0',sizeof(tabnomz));

if(data.metka_r == 2)
 {
  iceb_lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
  iceb_nomnak(gko,data.kassa.ravno(),&nomd,2,2,1,wpredok);
  zaphkorw(0,data.kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),data.shetk.ravno(),data.kodop.ravno(),data.naimop.ravno(),"","","","","",0,0,0,"","",wpredok);
  printf("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());
 }
SQL_str row;

while(cur.read_cursor(&row) != 0)
 {
//   printw("%s %s %s\n",row[0],row[1],row[2]);
  
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
//  strzag(LINES-1,0,kolstr,++kolstr1);
  
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  sprintf(strsql,"select podr from Kartb where tabn=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"%s %s %s !",gettext("Не найден табельный номер"),row[1],fio);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }

  strncpy(podr,row1[0],sizeof(podr)-1);

  if(data.metka_r == 0)
  if(iceb_u_SRAV(podrz,podr,0) != 0)
   {
    if(podrz[0] != '\0')
     {
      sprintf(kodkont,"%s%s",pktn,tabnomz);
      zapzkorw(data.kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),kodkont,suma,"","",wpredok);
      memset(tabnomz,'\0',sizeof(tabnomz));
     }

    iceb_lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
    iceb_nomnak(gko,data.kassa.ravno(),&nomd,2,2,1,wpredok);
    zaphkorw(0,data.kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),data.shetk.ravno(),data.kodop.ravno(),data.naimop.ravno(),"","","","","",0,0,0,"","",wpredok);
    printf("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());

    strcpy(podrz,podr);
   }

  if(iceb_u_SRAV(tabnomz,row[1],0) != 0)
   {
    if(tabnomz[0] != '\0')
     {
      if(data.metka_r == 1)
       {
        iceb_lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
        iceb_nomnak(gko,data.kassa.ravno(),&nomd,2,2,1,wpredok);
        zaphkorw(0,data.kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),data.shetk.ravno(),data.kodop.ravno(),data.naimop.ravno(),"","","","","",0,0,0,"","",wpredok);
        printf("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());
       }
      sprintf(kodkont,"%s%s",pktn,tabnomz);
      zapzkorw(data.kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),kodkont,suma,"","",wpredok);
     }
    suma=0.;
    strcpy(tabnomz,row[1]);
   }
  if(prn == 1)
    suma+=atof(row[4]);
  if(prn == 2)
    suma+=atof(row[4])*-1;
 }
if(data.metka_r == 1)
 {
  iceb_lock_tables("LOCK TABLES Kasord WRITE,icebuser READ");
  iceb_nomnak(gko,data.kassa.ravno(),&nomd,2,2,1,wpredok);
  zaphkorw(0,data.kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),data.shetk.ravno(),data.kodop.ravno(),data.naimop.ravno(),"","","","","",0,0,0,"","",wpredok);
  printf("%s:%s\n",gettext("Кассовый ордер"),nomd.ravno());
 }

sprintf(kodkont,"%s%s",pktn,tabnomz);
zapzkorw(data.kassa.ravno(),prn,dko,mko,gko,nomd.ravno(),kodkont,suma,"","",wpredok);



}
