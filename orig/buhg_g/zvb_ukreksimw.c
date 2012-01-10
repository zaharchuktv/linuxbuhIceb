/*$Id: zvb_ukreksimw.c,v 1.9 2011-02-21 07:36:00 sasa Exp $*/
/*20.10.2006	08.07.2005	Белых А.И.	zvb_ukreksimw.c
Выгрузка сумм для зачисления на карт-счета для Укрпромбанка
*/
#include <stdlib.h>
#include    "buhg_g.h"
#include "l_nahud.h"

extern char *organ;
extern SQL_baza bd;


void zvb_ukreksimw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);
char strsql[512];
char imafdbf[32];
char imaftmp[32];
char imafr[32];
FILE  *fftmp;
FILE  *ffr;
int   kolstr;
char		rukov[512];
char            glavbuh[512];
char  bros[512];


SQLCURSOR cur;
SQLCURSOR curr;
SQL_str row,row1;
if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }

memset(rukov,'\0',sizeof(rukov));
memset(glavbuh,'\0',sizeof(glavbuh));

memset(bros,'\0',sizeof(bros));
iceb_poldan("Табельный номер руководителя",bros,"zarnas.alx",wpredok);
if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(rukov,row1[0],sizeof(rukov)-1);
 } 

memset(bros,'\0',sizeof(bros));
iceb_poldan("Табельный номер бухгалтера",bros,"zarnas.alx",wpredok);

if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(glavbuh,row1[0],sizeof(glavbuh)-1);
 }

if(ukreksim_hapw(imafr,imaftmp,&fftmp,&ffr,wpredok) != 0)
 return;


char fio[512];

float kolstr1=0;
int nomer=0;

char kartshet[32];
double suma;
double sumai=0.;
char inn[32];
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
char tabnom[32];

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  suma=atof(row[4]);
  if(suma == 0.)
   continue;

  memset(kartshet,'\0',sizeof(kartshet));
  memset(fio,'\0',sizeof(fio));
  memset(inn,'\0',sizeof(inn));
  memset(tabnom,'\0',sizeof(tabnom));
  strncpy(tabnom,row[1],sizeof(tabnom)-1);
  
  sprintf(strsql,"select fio,inn,bankshet from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
   {
    strncpy(fio,row1[0],sizeof(fio)-1);
    strncpy(inn,row1[1],sizeof(inn)-1);
    strncpy(kartshet,row1[2],sizeof(kartshet)-1);

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

   
  if(prn == 2)
    suma*=(-1);   
  
  ukreksim_zstrw(fftmp,ffr,&nomer,suma,fio,kartshet,tabnom,dt,mt,gt);

  sumai+=suma;
    
 }

ukreksim_endw(nomer,imaftmp,imafdbf,sumai,rukov,glavbuh,fftmp,ffr,wpredok);

class iceb_u_spisok imaf;
class iceb_u_spisok naimf;
imaf.plus(imafr);
imaf.plus(imafdbf);
naimf.plus(gettext("Ведомость для зачисления на карт-счёта."));
naimf.plus(gettext("Файл для передачи в банк."));

gdite.close();

iceb_rabfil(&imaf,&naimf,"",0,wpredok);

}

