/*$Id: pvbanks_kprovw.c,v 1.6 2011-02-21 07:35:56 sasa Exp $*/
/*06.12.2010	20.02.2008	Белых А.И.	pvbanks_kprovw.c
проверка контрагента
*/
#include <stdlib.h>

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze


int pvbanks_kprovw(class iceb_u_spisok *KONT,int snom,const char *pris,int metkadk,double *sum,char *tabnom,
double *deb,double *kre,int dlpris,char *fio,char *bankshet,char *inn,const char *grupk,
int kod_banka,
GtkWidget *wpredok)
{
char strsql[1024];
char kontrag[56];
memset(kontrag,'\0',sizeof(kontrag));

strncpy(kontrag,KONT->ravno(snom),sizeof(kontrag)-1);

if(grupk[0] != '\0')
 {
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select grup from Kontragent where kodkon='%s'",kontrag);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
     if(iceb_u_proverka(grupk,row[0],0,0) != 0)
       return(1);
   }
 }


if(iceb_u_strstrm(kontrag,pris) == 0)
 return(1);
 
if(metkadk == 0)
  *sum=deb[snom]-kre[snom];
if(metkadk == 1)
  *sum=kre[snom]-deb[snom];

if(*sum <= 0.009)
 return(1);
 
memset(tabnom,'\0',sizeof(tabnom));
int shethik=0;
int dlkodkon=strlen(kontrag);
for(int i=dlpris; i < dlkodkon; i++)
 tabnom[shethik++]=kontrag[i];

SQL_str row1;
SQLCURSOR cur1;
  
//Читем в карточке фамилию и картсчет   
sprintf(strsql,"select fio,bankshet,inn,kb from Kartb where tabn=%s",tabnom);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 {
  strcpy(fio,row1[0]);
  strcpy(bankshet,row1[1]);
  strcpy(inn,row1[2]);
 }
if(kod_banka != 0)
 {
  sprintf(strsql,"select nks from Zarkh where tn=%s and kb=%d",tabnom,kod_banka);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"Для работника %s %s нет картсчёта для кода банка %d!",tabnom,fio,kod_banka);
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
  strcpy(bankshet,row1[0]);
 }
   
if(bankshet[0] == '\0')
 {
  sprintf(strsql,"%s\n%s:%s %s",
  gettext("Не введён счёт!"),
  gettext("Табельный номер"),tabnom,fio);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
return(0);
}
