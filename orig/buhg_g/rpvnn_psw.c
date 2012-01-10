/*$Id: rpvnn_psw.c,v 1.11 2011-02-21 07:35:57 sasa Exp $*/
/*17.02.2011	30.08.2005	Белых А.И.	rpvnn_psw.c
Печать шапки реестра налоговых накладных
Возвращаем количество строк в шапке
*/
#include <errno.h>
#include <unistd.h>
#include "buhg_g.h"

int rpvnn_ps0w(short dn,short mn,short gn,short dk,short mk,short gk,FILE *ff,GtkWidget *wpredok);
int rpvnn_ps1w(short dn,short mn,short gn,short dk,short mk,short gk,FILE *ff,GtkWidget *wpredok);
int rpvnn_ps2w(short dn,short mn,short gn,short dk,short mk,short gk,FILE *ff,GtkWidget *wpredok);
extern SQL_baza bd;

int rpvnn_psw(short dn,short mn,short gn,
short dk,short mk,short gk,
FILE *ff,
GtkWidget *wpredok)
{
if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) < 0)
  return(rpvnn_ps0w(dn,mn,gn,dk,mk,gk,ff,wpredok));

if(iceb_u_sravmydat(dn,mn,gn,1,1,2010) >= 0 and iceb_u_sravmydat(dn,mn,gn,1,1,2011) < 0)
  return(rpvnn_ps1w(dn,mn,gn,dk,mk,gk,ff,wpredok));

if(iceb_u_sravmydat(dn,mn,gn,1,1,2011) >= 0)
  return(rpvnn_ps2w(dn,mn,gn,dk,mk,gk,ff,wpredok));
return(1);
}
/**********************************/

int rpvnn_ps2w(short dn,short mn,short gn,short dk,short mk,short gk,FILE *ff,GtkWidget *wpredok)
{
char strok[1024];
char strsql[512];
SQL_str row;
SQLCURSOR cur;
char naiorg[512];
char inn[24];
char nspnds[32];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
memset(naiorg,'\0',sizeof(naiorg));
memset(inn,'\0',sizeof(inn));
memset(nspnds,'\0',sizeof(nspnds));


sprintf(strsql,"select naikon,pnaim,innn,nspnds from Kontragent where kodkon='00'");

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(row[1][0] != '\0')
   strncpy(naiorg,row[1],sizeof(naiorg)-1);
  else
   strncpy(naiorg,row[0],sizeof(naiorg)-1);

  strncpy(inn,row[2],sizeof(inn)-1);

  strncpy(nspnds,row[3],sizeof(nspnds)-1);
  
 }

sprintf(strsql,"select str from Alx where fil='rpvnn0_2.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"rpvnn0.alx");
  iceb_menu_soob(strsql,wpredok);
  return(0);
 }


int nomer_str=0;
 
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  memset(strok,'\0',sizeof(strok));
  strncpy(strok,row_alx[0],sizeof(strok)-1);
  switch(nomer_str)
   {
    case 29:
      iceb_u_bstab(strok,naiorg,15,106,1);
//      sprintf(strsql,"%02d.%02d.%d",dn,mn,gn);
//      iceb_u_bstab(strok,strsql,123,135,1);
      break;    

    case 35:
      iceb_u_bstab(strok,inn,76,100,1);
//      sprintf(strsql,"%02d.%02d.%d",dk,mk,gk);
//      iceb_u_bstab(strok,strsql,123,135,1);
      break;    

    case 39:
      iceb_u_bstab(strok,nspnds,76,100,1);
      break;    

   }
 
  fprintf(ff,"%s",strok);
  
 }
return(nomer_str); 
}




/********************************/

int rpvnn_ps0w(short dn,short mn,short gn,
short dk,short mk,short gk,
FILE *ff,
GtkWidget *wpredok)
{
char strok[1024];
char strsql[512];
SQL_str row;
SQLCURSOR cur;
char naiorg[512];
char inn[24];
char nspnds[32];
memset(naiorg,'\0',sizeof(naiorg));
memset(inn,'\0',sizeof(inn));
memset(nspnds,'\0',sizeof(nspnds));


sprintf(strsql,"select naikon,pnaim,innn,nspnds from Kontragent where kodkon='00'");

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(row[1][0] != '\0')
   strncpy(naiorg,row[1],sizeof(naiorg)-1);
  else
   strncpy(naiorg,row[0],sizeof(naiorg)-1);

  strncpy(inn,row[2],sizeof(inn)-1);

  strncpy(nspnds,row[3],sizeof(nspnds)-1);
  
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
int nomer_str=0;

sprintf(strsql,"select str from Alx where fil='rpvnn0.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"rpvnn0.alx");
  iceb_menu_soob(strsql,wpredok);
  return(0);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

 
  nomer_str++;
  memset(strok,'\0',sizeof(strok));
  strncpy(strok,row_alx[0],sizeof(strok)-1);
  switch(nomer_str)
   {
    case 5:
      iceb_u_bstab(strok,naiorg,26,90,1);
      sprintf(strsql,"%02d.%02d.%d",dn,mn,gn);
      iceb_u_bstab(strok,strsql,107,120,1);
      break;    

    case 6:
      iceb_u_bstab(strok,inn,35,90,1);
      sprintf(strsql,"%02d.%02d.%d",dk,mk,gk);
      iceb_u_bstab(strok,strsql,107,120,1);
      break;    

    case 7:
      iceb_u_bstab(strok,nspnds,64,90,1);
      break;    

   }
 
  fprintf(ff,"%s",strok);
  
 }
return(nomer_str); 
}
/*************************************/
int rpvnn_ps1w(short dn,short mn,short gn,
short dk,short mk,short gk,
FILE *ff,
GtkWidget *wpredok)
{
char strok[1024];
char strsql[512];
SQL_str row;
SQLCURSOR cur;
char naiorg[512];
char inn[24];
char nspnds[32];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
memset(naiorg,'\0',sizeof(naiorg));
memset(inn,'\0',sizeof(inn));
memset(nspnds,'\0',sizeof(nspnds));


sprintf(strsql,"select naikon,pnaim,innn,nspnds from Kontragent where kodkon='00'");

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(row[1][0] != '\0')
   strncpy(naiorg,row[1],sizeof(naiorg)-1);
  else
   strncpy(naiorg,row[0],sizeof(naiorg)-1);

  strncpy(inn,row[2],sizeof(inn)-1);

  strncpy(nspnds,row[3],sizeof(nspnds)-1);
  
 }
int nomer_str=0;

sprintf(strsql,"select str from Alx where fil='rpvnn0_1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"rpvnn0_1.alx");
  iceb_menu_soob(strsql,wpredok);
  return(0);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  memset(strok,'\0',sizeof(strok));
  strncpy(strok,row_alx[0],sizeof(strok)-1);


  nomer_str++;
  switch(nomer_str)
   {
    case 7:
      iceb_u_bstab(strok,naiorg,39,106,1);
      sprintf(strsql,"%02d.%02d.%d",dn,mn,gn);
      iceb_u_bstab(strok,strsql,123,135,1);
      break;    

    case 8:
      iceb_u_bstab(strok,inn,32,90,1);
      sprintf(strsql,"%02d.%02d.%d",dk,mk,gk);
      iceb_u_bstab(strok,strsql,123,135,1);
      break;    

    case 9:
      iceb_u_bstab(strok,nspnds,61,90,1);
      break;    


   }
 
  fprintf(ff,"%s",strok);
  
 }
return(nomer_str); 
}
