/*$Id: rasactuosw.c,v 1.9 2011-02-21 07:35:56 sasa Exp $*/
/*11.01.2010	20.07.2003	Белых А.И.	rasactuosw.c
Распечатка аста приема-передачи основных средств
*/
#include <stdlib.h>
#include        <errno.h>
#include	"buhg_g.h"
#include <unistd.h>

extern char	*organ;
extern double	okrg1;
extern SQL_baza bd;

void rasactuosw(const char *data_dok, //дата документа
const char *nomdok, //Номер документа
GtkWidget *wpredok)
{
short dd,md,gd;
char		strsql[512];
SQL_str		row,row1;
SQLCURSOR curr;
int 		kolstr=0;
char		imaf[32],imaftmp[32];
FILE		*ff,*fftmp;
char		shetu[32];
char            shetz[32];
char		naim[512];
char		stroka[1024];
int		kolstrok=0;
int		kollist=0;

iceb_u_rsdat(&dd,&md,&gd,data_dok,1);

sprintf(strsql,"select innom,bs,bsby from Uosdok1 where datd='%04d-%02d-%02d' and \
nomd='%s'",gd,md,dd,nomdok);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 { 
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),wpredok);
  return;
 } 

sprintf(imaftmp,"actm%d.tmp",getpid());

if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  //Читаем запись в таблице переменных данных
  sprintf(strsql,"select shetu from Uosinp where innom=%s and god < %d or \
(god=%d  and mes <= %d) order by god,mes desc",row[0],gd,gd,md);

  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) <= 0)
   {
    sprintf(stroka,"Не найдена запись !\n%s",strsql);
    iceb_menu_soob(stroka,wpredok);
    continue;
   }
  strncpy(shetu,row1[0],sizeof(shetu)-1);  

  //Читаем наименование основного средства
  
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   strncpy(naim,row1[0],sizeof(naim)-1);

  fprintf(fftmp,"%s|%s|%s|%s|%s|\n",
  shetu,naim,row[0],row[1],row[2]);
 }

fclose(fftmp);

//sprintf(strsql,"sort -o %s -t\\|  +0 -1 +1 -2  %s",imaftmp,imaftmp);
sprintf(strsql,"sort -o %s -t\\|  -k1,2 -k2,3 %s",imaftmp,imaftmp);

system(strsql);


sprintf(imaf,"actm%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

hrasactw(&kolstrok,&kollist,ff);

if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }

double bsby=0.;
double bs=0.;
double balst=0.;
int  nomerstr=0;
double kolihi=0.;
double sumai=0.;
char   naimshet[512];
double kolihshet=0.;
double sumashet=0.;
char    innom[20];

memset(shetz,'\0',sizeof(shetz));

while(fgets(stroka,sizeof(stroka),fftmp) != NULL)
 {
  iceb_u_polen(stroka,shetu,sizeof(shetu),1,'|');
  iceb_u_polen(stroka,naim,sizeof(naim),2,'|');
  iceb_u_polen(stroka,innom,sizeof(innom),3,'|');

  iceb_u_polen(stroka,strsql,sizeof(strsql),4,'|');
  bs=atof(strsql);
  bs=iceb_u_okrug(bs,0.01);

  iceb_u_polen(stroka,strsql,sizeof(strsql),5,'|');
  bsby=atof(strsql);
  bsby=iceb_u_okrug(bsby,0.01);
  balst=bsby;

  if(balst == 0.)
   balst=bs;
     
  if(iceb_u_SRAV(shetz,shetu,0) != 0)
   {

    if(shetz[0] != '\0')
     {
      sapkk1ivw(&kolstrok,&kollist,ff);
      sprintf(strsql,"%s %s %s",gettext("Итого по счету"),shetz,naimshet);
      fprintf(ff,"%*s %10.10g %10.2f\n",iceb_u_kolbait(62,strsql),strsql,kolihshet,sumashet);
     }

    memset(naimshet,'\0',sizeof(naimshet));
    sprintf(strsql,"select nais from Plansh where ns=%s",shetu);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     strncpy(naimshet,row1[0],sizeof(naimshet)-1);

    sapkk1ivw(&kolstrok,&kollist,ff);
    fprintf(ff,"%20s ******%s %s %s*******\n"," ",gettext("Счет учета"),shetu,naimshet);

    strcpy(shetz,shetu);
    kolihshet=0.;
    sumashet=0.;
   }  

  sumai+=balst;
  kolihi+=1;

  sumashet+=balst;
  kolihshet+=1;  

  sapkk1ivw(&kolstrok,&kollist,ff);
  
  fprintf(ff,"%3d|%-*.*s|%-*s|%10.10g|%10.10g|%10.2f|%10s|%10s|%10s|%10s|\n",
  ++nomerstr,
  iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim,
  iceb_u_kolbait(6,"Шт"),"Шт",
  balst,1.0,balst," "," "," "," ");

  sapkk1ivw(&kolstrok,&kollist,ff);
  fprintf(ff,"%3s %s:%s\n"," ",gettext("Инвентарный номер"),innom);

 }

krasactw(shetz,naimshet,kolihshet,sumashet,kolihi,sumai,ff);

iceb_podpis(ff,wpredok);
fclose(ff);
fclose(fftmp);

unlink(imaftmp);


class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf);
naimf.plus(gettext("Акт приёма-передачи основных средств"));
iceb_ustpeh(imaf,3,wpredok);
iceb_rabfil(&imafil,&naimf,"",0,wpredok);

}
