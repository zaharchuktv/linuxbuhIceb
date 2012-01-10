/*$Id: avtprukrw.c,v 1.18 2011-07-06 08:12:30 sasa Exp $*/
/*09.06.2011	16.11.2002	Белых А.И.	avtprukr.c
Подпрограмма автоматического выполнения проводок для "Учета командировочных расходов"
*/
#include <stdlib.h>
#include        <time.h>
#include        <errno.h>
#include        <math.h>
#include <unistd.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void avtprukrw(short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok)
{
FILE *ff_prot=NULL;
char		strsql[512];
int		kolstr=0;
SQL_str		row;
char		kodop[32];
char		kodops[32];
char		shetu[32];
char		shetkr[32];
char		shetnds[32];
class iceb_u_str kodu("");
class iceb_u_str kodkr("");
class iceb_u_str kodnds("");
char		koment[512];
double		deb=0.;
double		kre=0.;
short		dp=0,mp=0,gp=0;
class iceb_u_str kodkont("");
class iceb_u_str kodkontr("");
class iceb_u_str kodr("");
struct OPSHET	shetv;
char		nomao[32]; //Номер авансового отчета
SQLCURSOR curr;
int kekv=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int metka_vkontr=2; /*метка режима выполнения проводки для контрагента если его нет в списке счёта 0-запрос 2-автоматическая вставка*/


class iceb_u_spisok sp_prov;
class iceb_u_double sum_prov_dk;

sprintf(strsql,"select shet,kolih,cena,snds,kontr,sn,ss,kodr from Ukrdok1 where \
datd='%04d-%d-%d' and nomd='%s'",gd,md,dd,nomdok);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
if(kolstr == 0)
 return;

//Узнаем дату авансового отчета

sprintf(strsql,"select kont,vkom,nomao,datao from Ukrdok where god=%d and nomd='%s'",gd,nomdok);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 {
  kodkont.new_plus(row[0]);
  strcpy(kodop,row[1]);
  strcpy(nomao,row[2]);
  iceb_u_rsdat(&dp,&mp,&gp,row[3],2);
 }
else
 return;
 
if(dp == 0)
 {
  memset(strsql,'\0',sizeof(strsql));
  if(iceb_menu_vvod1(gettext("Введите дату авансового отчёта"),strsql,11,wpredok) != 0)
   return;

  if(iceb_u_rsdat(&dp,&mp,&gp,strsql,1) != 0)
   {
    iceb_menu_soob(gettext("Неправильно ввели дату авансового отчёта!"),wpredok);
    return;
   }

  sprintf(strsql,"update Ukrdok set datao='%04d-%d-%d' where god=%d and nomd='%s'",
  gp,mp,dp,gd,nomdok);
  
  iceb_sql_zapis(strsql,1,0,wpredok);
  
 }
if(iceb_pvglkni(mp,gp,wpredok) != 0)
 return;

sprintf(strsql,"select str from Alx where fil='avtprukr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки avtprukr.alx\n");
  return;
 }

memset(koment,'\0',sizeof(koment));
if(nomao[0] != '\0')
 sprintf(koment,"N%s",nomao);
 
while(cur.read_cursor(&row) != 0)
 {
  cur_alx.poz_cursor(0);
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;

    if(iceb_u_polen(row_alx[0],kodops,sizeof(kodops),1,'|') != 0)
     continue;
    
    if(iceb_u_SRAV(kodop,kodops,0) != 0)
     continue;
    if(iceb_u_polen(row_alx[0],&kodr,6,'|') == 0)
     {
      if(kodr.ravno_atoi() != 0)
       if(atoi(row[7]) != kodr.ravno_atoi())
        continue;      
     }
    memset(shetu,'\0',sizeof(shetu));
    if(iceb_u_polen(row_alx[0],shetu,sizeof(shetu),2,'|') != 0)
     continue;

    if(iceb_u_SRAV(row[0],shetu,0) != 0)
     continue;

    memset(shetkr,'\0',sizeof(shetkr));
    if(iceb_u_polen(row_alx[0],shetkr,sizeof(shetkr),3,'|') != 0)
     continue;
    
    memset(shetnds,'\0',sizeof(shetnds));
    
    if(row[5][0] != '\0')
     strncpy(shetnds,row[5],sizeof(shetnds));
    else
     if(iceb_u_polen(row_alx[0],shetnds,sizeof(shetkr),4,'|') != 0)
       continue;
   
   iceb_u_polen(row_alx[0],&kekv,5,'|');

    kodu.new_plus(kodkont.ravno());
    if(iceb_provsh(&kodu,shetu,&shetv,metka_vkontr,0,wpredok) != 0)
       continue;

    kodkr.new_plus(kodkont.ravno());
    if(iceb_provsh(&kodkr,shetkr,&shetv,metka_vkontr,0,wpredok) != 0)
       continue;

    kodnds.new_plus(kodkont.ravno());
    if(iceb_provsh(&kodnds,shetnds,&shetv,metka_vkontr,0,wpredok) != 0)
       continue;

    kodkontr.new_plus(row[4]);
    if(kodkontr.getdlinna() > 1)
     if(iceb_provsh(&kodkontr,shetkr,&shetv,metka_vkontr,0,wpredok) != 0)
       continue;
    
    //Проводка на сумму без НДС
    kre=0.;
    deb=atof(row[2])+atof(row[6]);
    if(kodkontr.getdlinna() <= 1)            
      iceb_zapmpr(gp,mp,dp,shetu,shetkr,kodu.ravno(),kodkr.ravno(),deb,kre,koment,2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
    else
     {
      iceb_zapmpr(gp,mp,dp,shetkr,shetkr,kodkr.ravno(),kodkontr.ravno(),deb,kre,koment,2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
      iceb_zapmpr(gp,mp,dp,shetu,shetkr,kodkontr.ravno(),kodkontr.ravno(),deb,kre,koment,2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
     }

    //Проводка на сумму НДС
    kre=0.;
    deb=atof(row[3]);
            
    iceb_zapmpr(gp,mp,dp,shetnds,shetkr,kodnds.ravno(),kodkr.ravno(),deb,kre,koment,2,kekv,&sp_prov,&sum_prov_dk,ff_prot);
   }

  
 }

/*Запись проводок из памяти в базу*/
int pod=0;
time_t vrem=0;
time(&vrem);
char tipd[8];
sprintf(tipd,gettext("УКР"));

iceb_zapmpr1(nomdok,kodop,pod,vrem,tipd,dd,md,gd,0,&sp_prov,&sum_prov_dk,ff_prot,wpredok);


}
