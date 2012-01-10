/* $Id: zapzagotw.c,v 1.8 2011-02-21 07:35:59 sasa Exp $ */
/*10.09.2006    09.11.1992      Белых А.И.      zapzagotw.c
Подпрограмма поиска и записи необходимых начислений/удержаний
Если вернули 1 значит не введены коды начислений и удержаний
             2 у оператора доступ только на чтение
*/
#include <stdlib.h>
#include        <time.h>
#include        <pwd.h>
#include        "buhg_g.h"

int	zapzagot1(short,short,int,int,int,GtkWidget*);

extern struct   passwd  ktor; /*Кто работает*/
extern short	*obud; /*Обязательные удержания*/
extern short	*obnah; /*Обязательные начисления*/
extern SQL_baza bd;

int zapzagotw(short mp,short gp,
int tabn,
int prn,  //1-начисление 2-удержание
int podr,
GtkWidget *wpredok)
{
short		d,m,g;
long		kolstr;
SQL_str         row;
SQLCURSOR cur;
char		strsql[512];
time_t		vrem;
class ZARP zp;
short		den;

//printf("zapzagotw-%d.%d tabn=%d prn=%d podr=%d\n",mp,gp,tabn,prn,podr);

if(podr == 0)
 {
  sprintf(strsql,"select podr from Kartb where tabn=%d",tabn);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   podr=atoi(row[0]);  
 }

sprintf(strsql,"select * from Zarn1 where tabn=%d and prn='%d'",
tabn,prn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(3);
 }

if(kolstr == 0)
 {
  if(zapzagot1(mp,gp,tabn,prn,podr,wpredok) == 0)
   return(1);
  else
   return(0);
 }
iceb_u_dpm(&den,&mp,&gp,5);
zp.tabnom=tabn;
zp.prn=prn;

zp.dz=0; //если день записи равен 0 то не выполняется предварительное удаление записи
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

time(&vrem);
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_rsdat(&d,&m,&g,row[3],1) != 0)
    d=m=g=0;

  if((m != 0 && m < mp) && (g != 0 && g <= gp ))
    continue;

  strcpy(zp.shet,row[4]);
  zp.knu=atoi(row[2]);

//  if(zapzarp(den,mp,gp,tabn,prn,atoi(row[2]),0.,row[4],mp,gp,0,0,"\0",podr,"",zp) == 3)
  if(zapzarpw(&zp,0.,den,mp,gp,0,row[4],"",0,podr,"",wpredok) == 3)
    return(2);

 }

return(0);

}
/****************************************************************/
/* Запись в пустую настройку обязательных начислений и удержаний*/
/****************************************************************/
/*Возвращает количество сделанных записей*/

int	zapzagot1(short mp,short gp,int tabn,int prn,int podr,GtkWidget *wpredok)
{
int		i;
short		den;
char		shet[32];
char		strsql[512];
SQL_str		row;
int		kolzap;
class ZARP     zp;
SQLCURSOR curr;

zp.tabnom=tabn;
zp.prn=prn;
zp.podr=podr;

kolzap=0;
if(prn == 1 && obnah != NULL)
 {
  den=0;
  iceb_u_dpm(&den,&mp,&gp,5);
  zp.dz=0; //если равен 0 то не выполняестя предварительное удаление записи
  zp.mz=mp;
  zp.gz=gp;
  zp.mesn=mp; zp.godn=gp;
  zp.nomz=0;
  zp.podr=podr;
  for(i=1; i <= obnah[0]; i++)
   {
    memset(shet,'\0',sizeof(shet));
    /*Поверяем есть ли удержание в списке удержаний*/
    sprintf(strsql,"select shet from Nash where kod=%d",obnah[i]);
//    printw("kod=%d\n",obnah[i]);
//    OSTANOV();
    
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     {
      strncpy(shet,row[0],sizeof(shet)-1);
      if(shet[0] == '\0')
       {
        sprintf(strsql,"select shet from Kartb where tabn=%d",tabn);
        if(sql_readkey(&bd,strsql,&row,&curr) == 1)
         strncpy(shet,row[0],sizeof(shet)-1);

       }       

      if(shet[0] != '\0')
       {
        strcpy(zp.shet,shet);
        zp.knu=obnah[i];
        //zapzarp(d,mp,gp,tabn,prn,obnah[i],0.,shet,mp,gp,0,0,"\0",podr,"",zp); 
        zapzarpw(&zp,0.,den,mp,gp,0,row[4],"",0,podr,"",wpredok);
        zapzarn1w(tabn,(short)prn,obnah[i],0,0,0,shet,wpredok);
        kolzap++;
       }
     }

   }  

 }

if(prn == 2 && obud != NULL)
 {
  den=0;

  iceb_u_dpm(&den,&mp,&gp,5);
  zp.dz=0;  //если равен 0 то не выполняестя предварительное удаление записи
  zp.mz=mp;
  zp.gz=gp;
  zp.mesn=mp; zp.godn=gp;
  zp.nomz=0;
  zp.podr=podr;
  for(i=1; i <= obud[0]; i++)
   {
    /*Поверяем есть ли удержание в списке удержаний*/
    sprintf(strsql,"select shet from Uder where kod=%d",obud[i]);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     {
      strncpy(shet,row[0],sizeof(shet)-1);
      strcpy(zp.shet,shet);
      zp.knu=obud[i];

//      zapzarp(d,mp,gp,tabn,prn,obud[i],0.,shet,mp,gp,0,0,"\0",podr,"",zp); 
      zapzarpw(&zp,0.,den,mp,gp,0,row[4],"",0,podr,"",wpredok);
      zapzarn1w(tabn,(short)prn,obud[i],0,0,0,shet,wpredok);
      kolzap++;
     }

   }  

 }
return(kolzap);
}
