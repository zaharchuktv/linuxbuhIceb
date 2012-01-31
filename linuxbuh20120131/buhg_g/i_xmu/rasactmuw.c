/*$Id: rasactmuw.c,v 1.15 2011-02-21 07:35:56 sasa Exp $*/
/*24.03.2010	19.07.2003	Белых А.И.	rasactmuw.c
Распечатка акта приема передачи материалов
*/
#include        <stdlib.h>
#include        <errno.h>
#include	"../headers/buhg_g.h"
#include        <unistd.h>

extern double	okrg1;
extern SQL_baza  bd;

void rasactmuw(short dd,short md,short gd, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
GtkWidget *wpredok)
{
char		strsql[512];
SQL_str		row,row1;
int 		kolstr=0;
char		imaf[32],imaftmp[32];
FILE		*ff,*fftmp;
char		shetu[32];
char            shetz[32];
char		naim[512];
char		stroka[1024];
int		kolstrok=0;
int		kollist=0;
char		datv[11];
char		innom[32];
char		regnom[32];

//printw("\nrasactmu %d.%d.%d %d %s\n",dd,md,gd,skl,nomdok);
//OSTANOV();

sprintf(strsql,"select sklad,nomkar,kodm,kolih,cena,ei from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",gd,md,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 { 
  iceb_u_str repl;
  
  repl.plus(gettext("Не найдено ни одной записи !"));
  iceb_menu_soob(&repl,wpredok);
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
  //Узнаем счет учета материалла
  sprintf(strsql,"select shetu,datv,innom,rnd from Kart where sklad=%s and nomk=%s",row[0],row[1]);
  memset(shetu,'\0',sizeof(shetu));
  memset(regnom,'\0',sizeof(regnom));
  memset(innom,'\0',sizeof(innom));
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    strncpy(shetu,row1[0],sizeof(shetu)-1);
    strcpy(datv,row[1]);
    strcpy(innom,row1[2]);
    strncpy(regnom,row1[3],sizeof(row1[3])-1);
   }
  //Узнаем наименование материалла
  memset(naim,'\0',sizeof(naim));
  sprintf(strsql,"select naimat from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   strncpy(naim,row1[0],sizeof(naim)-1);

  fprintf(fftmp,"%s|%s|%s|%s|%s|%s|%s|%s|\n",
  shetu,naim,row[3],row[4],row[5],datv,innom,regnom);

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

double kolih=0.;
double cena=0.;
char ei[32];
int  nomerstr=0;
double summa=0;
double kolihi=0.;
double sumai=0.;
char   naimshet[512];
double kolihshet=0.;
double sumashet=0.;
short dv,mv,gv;
char	dopstr[300];

memset(shetz,'\0',sizeof(shetz));

while(fgets(stroka,sizeof(stroka),fftmp) != NULL)
 {
  iceb_u_polen(stroka,shetu,sizeof(shetu),1,'|');
  iceb_u_polen(stroka,naim,sizeof(naim),2,'|');

  iceb_u_polen(stroka,strsql,sizeof(strsql),3,'|');
  kolih=atof(strsql);

  iceb_u_polen(stroka,strsql,sizeof(strsql),4,'|');
  cena=atof(strsql);

  iceb_u_polen(stroka,ei,sizeof(ei),5,'|');

  iceb_u_polen(stroka,datv,sizeof(datv),6,'|');
  iceb_u_rsdat(&dv,&mv,&gv,datv,2);
  
  iceb_u_polen(stroka,innom,sizeof(innom),7,'|');

  iceb_u_polen(stroka,regnom,sizeof(regnom),8,'|');
  
  cena=iceb_u_okrug(cena,okrg1);
  summa=kolih*cena;
  
  if(iceb_u_SRAV(shetz,shetu,0) != 0)
   {

    if(shetz[0] != '\0')
     {
      sapkk1ivw(&kolstrok,&kollist,ff);
      sprintf(strsql,"%s %s %s",gettext("Итого по счету"),shetz,naimshet);
      fprintf(ff,"%*.*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,strsql),iceb_u_kolbait(62,strsql),strsql,kolihshet,sumashet);
     }

    memset(naimshet,'\0',sizeof(naimshet));
    sprintf(strsql,"select nais from Plansh where ns=%s",shetu);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     strncpy(naimshet,row1[0],sizeof(naimshet)-1);

    sapkk1ivw(&kolstrok,&kollist,ff);
    fprintf(ff,"%20s ******%s %s %s*******\n"," ",gettext("Счет учета"),shetu,naimshet);

    strcpy(shetz,shetu);
    kolihshet=0.;
    sumashet=0.;
   }  
  
  
  sumai+=summa;
  kolihi+=kolih;
  sumashet+=summa;
  kolihshet+=kolih;  
  
  sapkk1ivw(&kolstrok,&kollist,ff);
  
  fprintf(ff,"%3d|%-*.*s|%-*s|%10.10g|%10.10g|%10.2f|%10s|%10s|%10s|%10s|\n",
  ++nomerstr,
  iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim,
  iceb_u_kolbait(6,ei),ei,
  cena,kolih,summa," "," "," "," ");

  if(innom[0] != '\0')
   {
    if( regnom[0] == '\0')
     {
      sprintf(dopstr,"%s:%s",gettext("Инвентарный номер"),innom);
      if(dv != 0)
       {
        memset(strsql,'\0',sizeof(strsql));
        sprintf(strsql," %s:%d.%d.%d%s",
        gettext("Дата ввода в эксплуатацию"),
        dv,mv,gv,gettext("г."));
        strcat(dopstr,strsql);
       }
     }        
    else
     {
      sprintf(dopstr,"%s:%s",gettext("Регистрационный номер"),regnom);
      memset(strsql,'\0',sizeof(strsql));
      sprintf(strsql," %s:%s",gettext("Серия"),innom);
      strcat(dopstr,strsql);
      if(dv != 0)
       {
        memset(strsql,'\0',sizeof(strsql));
        sprintf(strsql," %s:%d.%d.%d%s",
        gettext("Дата регистрации"),
        dv,mv,gv,gettext("г."));
        strcat(dopstr,strsql);
       }
     }
   }
  if(dopstr[0] != '\0')
   {
    sapkk1ivw(&kolstrok,&kollist,ff);
    fprintf(ff,"%3s %s\n"," ",dopstr);
   }
  sapkk1ivw(&kolstrok,&kollist,ff);
  fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    
 }
krasactw(shetz,naimshet,kolihshet,sumashet,kolihi,sumai,ff);
fclose(ff);
fclose(fftmp);

unlink(imaftmp);


iceb_u_spisok imafils;
iceb_u_spisok naimf;
imafils.plus(imaf);
naimf.plus(gettext("Распечатка акта приемки-передачи"));
iceb_ustpeh(imaf,3,wpredok);
iceb_rabfil(&imafils,&naimf,"",0,wpredok);

}
