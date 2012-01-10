/*$Id: zarstfn53w.c,v 1.11 2011-02-21 07:36:00 sasa Exp $*/
/*29.09.2009	17.03.2004	Белых А.И.	zarstfn53w.c
Шапка типовой формы N53 - платежная ведомость
*/
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"

extern char *organ;
extern SQL_baza bd;

int  zarstfn53w_0(char *naipodr,char *mesqc,short gr,const char *shet,int *kolstr,FILE *ff);
int  zarstfn53w_1(const char *naipodr,char *mesqc,short gr,const char *shet,const char *nomerved,int *kolstr,FILE *ff);

int  zarstfn53w(char *naipodr,short mr,short gr,
const char *shet,
const char *nomerved,
int *kolstr,
FILE *ff)
{
char mesqc[50];
memset(mesqc,'\0',sizeof(mesqc));

iceb_mesc(mr,0,mesqc);
if(iceb_u_sravmydat(16,6,2009,30,mr,gr) > 0)
  return(zarstfn53w_0(naipodr,mesqc,gr,shet,kolstr,ff));
else
  return(zarstfn53w_1(naipodr,mesqc,gr,shet,nomerved,kolstr,ff));

}
/**********************************/
/*********************************/

int  zarstfn53w_0(char *naipodr,char *mesqc,short gr,
const char *shet,
int *kolstr,
FILE *ff)
{
char stroka[1024];
char kod[30];
SQLCURSOR cur;
SQL_str   row;

memset(kod,'\0',sizeof(kod));

//читаем код 
sprintf(stroka,"select kod from Kontragent where kodkon='00'");
if(sql_readkey(&bd,stroka,&row,&cur) == 1)
  strncpy(kod,row[0],sizeof(kod)-1);



SQL_str row_alx;
class SQLCURSOR cur_alx;
char strsql[512];
const char *imaf_alx={"zarspv.alx"};
int kolstr1=0;

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr1=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr1 == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

int nomstr=0;
char bros[512];
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomstr++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(nomstr)
   {
    case 1:
     //в первых двух символах команды для принтера
     iceb_u_bstab(stroka,organ,3,40,1);
     break;

    case 2:
     iceb_u_bstab(stroka,kod,32,48,1);
     break;

    case 4:
     iceb_u_bstab(stroka,naipodr,0,47,1);
     break;

    case 10:
     sprintf(bros,"%d%s",gr,gettext("г."));
     
     iceb_u_bstab(stroka,bros,21,35,1);
     break;

    case 14:
     iceb_u_bstab(stroka,shet,42,50,1);
     break;

    case 16:
     sprintf(bros,"%d%s",gr,gettext("г."));
     
     iceb_u_bstab(stroka,bros,14,23,1);
     break;

    case 18:
     sprintf(bros,"%s %d%s",mesqc,gr,gettext("г."));
     
     iceb_u_bstab(stroka,bros,28,51,1);
     break;
        
   }
  *kolstr+=1;     
  fprintf(ff,"%s",stroka);
  
 }
 

return(0);

}
/*************************************/
/**************************************/
int  zarstfn53w_1(const char *naipodr,char *mesqc,short gr,const char *shet,const char *nomerved,int *kolstr,FILE *ff)
{
char stroka[1024];
char kod[30];
SQLCURSOR cur;
SQL_str   row;

memset(kod,'\0',sizeof(kod));

//читаем код 
sprintf(stroka,"select kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(stroka,&row,&cur,NULL) == 1)
  strncpy(kod,row[0],sizeof(kod)-1);


SQL_str row_alx;
class SQLCURSOR cur_alx;
char strsql[512];
const char *imaf_alx={"zarspv2.alx"};
int kolstr1=0;

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr1=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr1 == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

int nomstr=0;
char bros[512];
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomstr++;
  memset(stroka,'\0',sizeof(stroka));
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(nomstr)
   {
    case 1:
     //в первых двух символах команды для принтера
     iceb_u_bstab(stroka,organ,3,40,1);
     break;

    case 2:
     iceb_u_bstab(stroka,kod,32,48,1);
     break;

    case 4:
     iceb_u_bstab(stroka,naipodr,0,47,1);
     break;

    case 10:
     sprintf(bros,"%d%s",gr,gettext("г."));
     
     iceb_u_bstab(stroka,bros,21,35,1);
     break;

    case 14:
     iceb_u_bstab(stroka,shet,42,50,1);
     break;

    case 16:
     sprintf(bros,"%d%s",gr,gettext("г."));
     
     iceb_u_bstab(stroka,bros,14,23,1);
     break;

    case 18:
     iceb_u_bstab(stroka,nomerved,43,53,1);
     break;

    case 19:
     sprintf(bros,"%s %d%s",mesqc,gr,gettext("г."));
     
     iceb_u_bstab(stroka,bros,28,51,1);
     break;
        
   }
  *kolstr+=1;     
  fprintf(ff,"%s",stroka);
  
 }
 

return(0);

}
