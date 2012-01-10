/*$Id: zvb_kreditpromw.c,v 1.10 2011-02-21 07:36:00 sasa Exp $*/
/*19.10.2006	31.05.2005	Белых А.И.	zvb_kreditpromw.c
Выгрузка данных для Кредитпром банка
*/
#include <stdlib.h>
#include        <math.h>
#include        <errno.h>
#include    "buhg_g.h"
#include <unistd.h>
#include "l_nahud.h"

extern char *organ;
extern SQL_baza bd;

void zvb_kreditpromw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
char strsql[512];
char		rukov[512];
char            glavbuh[512];
char bros[512];
SQL_str row1;
SQLCURSOR cur1;
char imaf_d[32];
short dt,mt,gt;

iceb_u_poltekdat(&dt,&mt,&gt);

sprintf(imaf_d,"Pa3%02d0001.iss",mt);

if(iceb_menu_vvod1(gettext("Введите имя файла"),imaf_d,sizeof(imaf_d),wpredok) != 0)
 return;

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);


memset(rukov,'\0',sizeof(rukov));
memset(glavbuh,'\0',sizeof(glavbuh));

memset(bros,'\0',sizeof(bros));
iceb_poldan("Табельный номер руководителя",bros,"zarnast.alx",wpredok);
if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
    strncpy(rukov,row1[0],sizeof(rukov)-1);
 } 

memset(bros,'\0',sizeof(bros));
iceb_poldan("Табельный номер бухгалтера",bros,"zarnast.alx",wpredok);

if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
    strncpy(glavbuh,row1[0],sizeof(glavbuh)-1);
 }


SQLCURSOR cur;
int kolstr;
if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }
char imaf_r[32];
FILE *ffr;
FILE *ffd;
sprintf(imaf_r,"kpb%d.lst",getpid());

if((ffd = fopen(imaf_d,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_d,"",errno,wpredok);
  return;
 }

if((ffr = fopen(imaf_r,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_r,"",errno,wpredok);
  return;
 }
iceb_u_startfil(ffr);

fprintf(ffr,"%s\n\n",organ);

fprintf(ffr,"\
          Відомість для зарахування на карт-рахунки N_____\n\
                від %d.%d.%dр.\n\n",dt,mt,gt);

fprintf(ffr,"\
-----------------------------------------------------------------------\n");
fprintf(ffr,"\
 Н/п|Таб.номер| Карт-счёт |  Сумма    | Призвище,і'мя, по батькові\n");
fprintf(ffr,"\
-----------------------------------------------------------------------\n");

fprintf(ffd,"%s%04d%02d%02d\n",imaf_d,gt,mt,dt);


float kolstr1=0.;
SQL_str row;
double sumai=0.;
char fio[512];
char kartshet[56];
double suma=0.;
int nomer=0;
int suma_kop=0;
int isuma_kop=0;
double suma_cel;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
//  strzag(LINES-1,0,kolstr,++kolstr1);
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;
  memset(kartshet,'\0',sizeof(kartshet));
  memset(fio,'\0',sizeof(fio));
  
  sprintf(strsql,"select fio,bankshet from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    strncpy(fio,row1[0],sizeof(fio)-1);
    strncpy(kartshet,row1[1],sizeof(kartshet)-1);

   }
  if(kartshet[0] == '\0')
   {
    class iceb_u_str repl;
    repl.plus(gettext("Не введён карт-счёт !"));

    sprintf(strsql,"%s:%s %s",gettext("Табельный номер"),row[1],fio);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    continue;
   }

  suma=atof(row[4]);
  if(prn == 2)
    suma*=(-1);   
  suma_cel=suma=iceb_u_okrug(suma,0.01);
  suma_cel*=100.;
    
  modf(suma_cel,&suma_cel);

  suma_kop=(int)suma_cel;
  isuma_kop+=suma_kop;
   
  fprintf(ffr,"%4d %-9s %-14s %10.2f %s\n",
  ++nomer,row[1],kartshet,suma,fio);
  sumai+=suma;
  
  fprintf(ffd,"%-14s%12dUAH%04d%02d%02d\n",kartshet,suma_kop,gt,mt,dt);
 }
fprintf(ffr,"\
-----------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_u_kolbait(29,"Разом"),"Разом",sumai);

fprintf(ffd,"%s%-8d%-14d\n",imaf_d,nomer+2,isuma_kop);

fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov,glavbuh);

iceb_podpis(ffr,wpredok);

fclose(ffr);
fclose(ffd);

class iceb_u_spisok imafr;
class iceb_u_spisok naimf;
imafr.plus(imaf_r);
imafr.plus(imaf_d);
naimf.plus(gettext("Ведомость для зачисления на карт-счёта"));
naimf.plus(gettext("Файл для передачи в банк"));

gdite.close();

iceb_rabfil(&imafr,&naimf,"",0,wpredok);

 
}
