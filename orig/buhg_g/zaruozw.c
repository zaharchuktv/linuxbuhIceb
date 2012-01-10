/*$Id: zaruozw.c,v 1.5 2011-02-21 07:36:00 sasa Exp $*/
/*22.12.2010	25.04.2007	Белых А.И.	zaruozw.c
Расчёт удержание однодневного зароботка
*/
#include <stdlib.h>
#include "buhg_g.h"
#include        "zarp1.h"

extern short kod_ud_dnev_zar; //Код удержания дневного зароботка
extern short *kodnah_nvr_dnev_zar; //Коды начислений не входящие в расчёт удержания дневного зароботка
extern short *kodtab_nvr_dnev_zar; //Коды табеля не входящие в расчёт удержания однодневного зароботка
extern struct ZAR zar;
extern double   okrg;
extern SQL_baza bd;

void zaruozw(int tabnom,short mr,short gr,int podr,const char *uder_only,FILE *ff_prot,GtkWidget *wpredok)
{

if(kod_ud_dnev_zar == 0)
 return;

if(ff_prot != NULL)
 fprintf(ff_prot,"\nРасчёт удержания однодневного зароботка\n\
---------------------------------------------------------\n"); 

if(iceb_u_proverka(uder_only,kod_ud_dnev_zar,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код %d исключён из расчёта\n",kod_ud_dnev_zar);
  return;
 }

SQL_str row;
class SQLCURSOR cur;

char strsql[512];
static char shet_uder[56];

if(shet_uder[0] == '\0')
 {
  sprintf(strsql,"select shet from Uder where kod=%d",kod_ud_dnev_zar);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(shet_uder,row[0],sizeof(shet_uder)-1);
  else
   if(ff_prot != NULL)
    {
     fprintf(ff_prot,"Не найден код удержания %d в списке удержаний !",kod_ud_dnev_zar);
     return;
    }
 }

if(shet_uder[0] == '\0')
 return;

double suma_nah=0.;
for(int ii=0;ii<razm;ii++)
 {
  if(zar.prnu[ii] == 2)
   break;

  if(zar.mes[ii] != mr)
   continue;

  if(provkodw(kodnah_nvr_dnev_zar,zar.knu[ii]) >= 0)
   continue;

  suma_nah+=zar.sm[ii];
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"Сумма взятая в расчёт=%.2f\n",suma_nah);

if(suma_nah <= 0.)
 return;
  
sprintf(strsql,"select kodt,dnei from Ztab where god=%d and mes=%d and tabn=%d",gr,mr,tabnom);
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
int kolih_dnei=0;
while(cur.read_cursor(&row) != 0)
 {
  if(provkodw(kodtab_nvr_dnev_zar,row[0]) >= 0)
   continue;
  kolih_dnei+=atoi(row[1]);
 }

if(ff_prot != NULL)
 fprintf(ff_prot,"Количество дней взятое в расчёт=%d\n",kolih_dnei);

if(kolih_dnei == 0)
 return;
 
double suma_ud=suma_nah/kolih_dnei;
suma_ud=iceb_u_okrug(suma_ud,okrg);

if(ff_prot != NULL)
 fprintf(ff_prot,"%.2f/%d=%.2f\n",suma_nah,kolih_dnei,suma_ud);
suma_ud*=-1;

char br[10];
short d=1;
iceb_u_dpm(&d,&mr,&gr,5);
memset(br,'\0',sizeof(br));

struct ZARP     zp;


zp.tabnom=tabnom;
zp.prn=2;
zp.knu=kod_ud_dnev_zar;
zp.dz=d;
zp.mz=mr;
zp.gz=gr;
zp.mesn=mr; zp.godn=gr;
zp.nomz=0;
zp.podr=podr;
strncpy(zp.shet,shet_uder,sizeof(zp.shet)-1);

zapzarpw(&zp,suma_ud,d,mr,gr,0,shet_uder,"",0,podr,"",wpredok);


/*********
zp.dz=d;
zp.mesn=mr; zp.godn=gr;
zp.nomz=0;
zp.podr=podr;
strcpy(zp.shet,shet_uder);
zapzarp(d,mr,gr,tabnom,2,kod_ud_dnev_zar,suma_ud,shet_uder,mr,gr,0,0,br,podr,"",zp); 
***********/
 
}
