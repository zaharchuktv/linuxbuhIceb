/*$Id: ukrkuw.c,v 1.11 2011-02-21 07:35:57 sasa Exp $*/
/*14.12.2010	15.09.2003	Белых А.И.	ukrku.c
Распечатка командировочного удостоверения
*/
#include <stdlib.h>
#include <errno.h>
#include "buhg_g.h"
#include <unistd.h>

extern SQL_baza bd;

void ukrkuw(short gd,const char *nomd,GtkWidget *wpredok)
{
FILE    *ffku;
char    imaf[32];
char    strsql[512];
SQLCURSOR cur;
SQLCURSOR curr;
char	kont[32];
char    pnaz[512];
char    organ[512];
char    celk[512];
short   dn,mn,gn;
short   dk,mk,gk;
char    nompr[32];
short   dp,mp,gp;
SQL_str row;
char    stroka[1024];
int     nomer=0;
char    fio[512];
char    dolgn[512];
char    prist[512];


sprintf(strsql,"select kont,pnaz,organ,celk,datn,datk,nompr,datp \
from Ukrdok where god=%d and nomd='%s'",gd,nomd);

/*printf("ukrku-%s\n",strsql);*/

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  iceb_menu_soob(gettext("Не нашли шапку документа!"),wpredok);
  return;
 }
memset(kont,'\0',sizeof(kont));
strncpy(kont,row[0],sizeof(kont)-1);

memset(pnaz,'\0',sizeof(pnaz));
strncpy(pnaz,row[1],sizeof(pnaz)-1);

memset(organ,'\0',sizeof(organ));
strncpy(organ,row[2],sizeof(organ)-1);

memset(celk,'\0',sizeof(celk));
strncpy(celk,row[3],sizeof(celk)-1);

iceb_u_rsdat(&dn,&mn,&gn,row[4],2);

iceb_u_rsdat(&dk,&mk,&gk,row[5],2);

memset(nompr,'\0',sizeof(nompr));
strncpy(nompr,row[6],sizeof(nompr)-1);

iceb_u_rsdat(&dp,&mp,&gp,row[7],2);

memset(fio,'\0',sizeof(fio));
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kont);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
  strncpy(fio,row[0],sizeof(fio)-1);

iceb_poldan("Приставка к табельному номеру",prist,"zarnast.alx",wpredok);
short dlinnaprist=strlen(prist);
short dlinnakont=strlen(kont);

char tabnom[20];
char nomp[30];

memset(tabnom,'\0',sizeof(tabnom));
memset(dolgn,'\0',sizeof(dolgn));
memset(nomp,'\0',sizeof(nomp));

int i2=0;
for(int i=dlinnaprist; i < dlinnakont;i++)
  tabnom[i2++]=kont[i];

//Читаем должность
sprintf(strsql,"select fio,dolg,nomp from Kartb where tabn=%d",atoi(tabnom));
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 {
  memset(fio,'\0',sizeof(fio));
  strncpy(fio,row[0],sizeof(fio)-1);
  strncpy(dolgn,row[1],sizeof(dolgn)-1);
  strncpy(nomp,row[2],sizeof(nomp)-1);
 }
int koldn=iceb_u_period(dn,mn,gn,dk,mk,gk,0);


SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='ukrku.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"ukrku.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }


sprintf(imaf,"ku%d.lst",getpid());

if((ffku = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka));
  switch(nomer)
   {
    case 9:
      iceb_u_bstab(stroka,nomd,53,66,1);
      break;

    case 11:
      iceb_u_bstab(stroka,fio,30,68,1);
      break;

    case 12:
      iceb_u_bstab(stroka,dolgn,23,68,1);
      break;

    case 13:
      iceb_u_bstab(stroka,pnaz,40,68,1);
      break;

    case 14:
      iceb_u_bstab(stroka,organ,23,68,1);
      break;

    case 15:
      iceb_u_bstab(stroka,celk,23,68,1);
      break;

    case 17:
      sprintf(strsql,"%d",koldn);
      iceb_u_bstab(stroka,strsql,19,21,1);
      memset(strsql,' ',sizeof(strsql));
      if(dn != 0)
       sprintf(strsql,"%02d.%02d.%04d",dn,mn,gn);

      iceb_u_bstab(stroka,strsql,29,39,1);
/************
      По просьбе клиента дату конца не печататем
      memset(strsql,' ',sizeof(strsql));
      if(dk != 0)
        sprintf(strsql,"%02d.%02d.%04d",dk,mk,gk);
      iceb_u_bstab(stroka,strsql,45,55,1);
***************/
      break;

    case 18:
      iceb_u_bstab(stroka,nompr,17,26,1);

      sprintf(strsql,"%02d.%02d.%04d",dp,mp,gp);
      iceb_u_bstab(stroka,strsql,31,41,1);

      break;

    case 19:
      iceb_u_bstab(stroka,nomp,33,63,1);
      break;
     
   }

  fprintf(ffku,"%s",stroka);

 }

iceb_podpis(ffku,wpredok);

fclose(ffku);

class iceb_u_spisok imafil;
class iceb_u_spisok naimfil;

imafil.plus(imaf);
naimfil.plus(gettext("Командировочное удостоверение"));
iceb_ustpeh(imaf,1,wpredok);
iceb_rabfil(&imafil,&naimfil,"",0,wpredok);


}
