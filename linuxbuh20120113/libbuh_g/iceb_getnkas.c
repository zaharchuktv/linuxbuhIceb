/* $Id: iceb_getnkas.c,v 1.9 2011-02-21 07:36:07 sasa Exp $ */
/*10.12.2010	29.01.2000	Белых А.И.	iceb_u_getnkas.c
Чтение настроек для работы с кассовым регистратором
*/
#include        <stdlib.h>
#include        <errno.h>
#include	<ctype.h>
//#include        <pwd.h>
#include        <unistd.h>
#include        "iceb_libbuh.h"

extern SQL_baza bd;

#define MAXKASN	16
int	iceb_getnkas
   /* входные параметры 	*/
   (    int autoid,		// 1 - автовыбор по ид. раб. места; 0 - меню
   /* выходные параметры 	*/
    KASSA *kasr)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
char	bros[256];
char	buf[256], buf1[256];
int 	kom;
KASSA	kassa[MAXKASN];
int i, j, ret;
char *whost=NULL;
char	*p;
int 	kassano;

if ((p = getenv("KASSANO")) != NULL) 
  kassano = atoi(p);
else 
  kassano = -1;

kasr->prodavec.new_plus(iceb_u_getlogin());

sprintf(strsql,"select str from Alx where fil='kassa.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдены настройки kassa.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }



if ((kassano == -1) && ((whost=workid()) == NULL)) 
  return(1);

i = 0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if (i >= MAXKASN) 
    break;
//  printw("%s",row_alx[0]);
  if((row_alx[0][0] == '#') || (strlen(row_alx[0]) == 0) || isspace(row_alx[0][0]))
        continue;

  if(iceb_u_polen(row_alx[0],buf1,sizeof(buf1),2,'#') != 0)
    continue;

  iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'#');
  
  iceb_u_polen(bros,buf,sizeof(buf),1,'|');
  kassa[i].sklad = atoi(buf);
  iceb_u_polen(bros,buf,sizeof(buf),2,'|');
  kassa[i].nomer = atoi(buf);
//  if (autoid != 0)
  if (kassano != -1) 
   {
    if (kassa[i].nomer != kassano) 
     continue;
   }
  else 
   if (iceb_u_SRAV(whost,buf1,1) != 0) 
      continue;

  iceb_u_pole(bros,buf,3,'|');
  kassa[i].host = (char*) malloc(strlen(buf)+1);
  strcpy(kassa[i].host, buf);
  
  iceb_u_pole(bros,buf,4,'|');
  if ((j = strlen(buf)) != 0) 
   {
    kassa[i].port = (char*) malloc(strlen(buf)+1);
    strcpy(kassa[i].port, buf);
   }
  else kassa[i].port = NULL;
  
  iceb_u_pole(bros,buf,5,'|');
  kassa[i].parol = (char*) malloc(strlen(buf)+1);
  strcpy(kassa[i].parol, buf);
  i++;
 }


if(i == 0)
 return(1);
 
 i--;

ret = kom = 0;

while (i>=0) 
 {
  if ((i == kom) && (ret == 0)) 
   {
    kasr->host = kassa[i].host;
    kasr->port = kassa[i].port;
    kasr->parol = kassa[i].parol;
    kasr->sklad = kassa[i].sklad;
    kasr->nomer = kassa[i].nomer;
   }
  else 
   {
    free(kassa[i].host);
    free(kassa[i].port);
    free(kassa[i].parol);
   }
  i--;
 }
return(ret);
}
