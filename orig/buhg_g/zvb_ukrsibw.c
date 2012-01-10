/*$Id: zvb_ukrsibw.c,v 1.8 2011-02-21 07:36:00 sasa Exp $*/
/*19.10.2006	04.04.2004	Белых А.И.	zvb_ukrsibw.c
Выгузка для укрсиббанка
*/
#include <stdlib.h>
#include    "buhg_g.h"
#include "l_nahud.h"

void		ukrsib_h(char *imaf,long kolz);

extern char *organ;
extern SQL_baza bd;

void zvb_ukrsibw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
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
SQLCURSOR cur1;
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
class iceb_u_str operatorr("");
ukrsib_startw(imafr,imaftmp,&operatorr,&fftmp,&ffr,wpredok);

char fio[512];
char tabnom[56];

float kolstr1=0.;
int nomer=0;

char kartshet[32];
double suma;
double sumai=0.;
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  memset(kartshet,'\0',sizeof(kartshet));
  memset(fio,'\0',sizeof(fio));
  memset(tabnom,'\0',sizeof(tabnom));
  strncpy(tabnom,row[1],sizeof(tabnom));
    
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

  sumai+=suma;

  ukrsib_strw(dt,mt,gt,suma,kartshet,fio,&nomer,tabnom,operatorr.ravno(),ffr,fftmp);

    
 }

ukrsib_endw(imafdbf,imaftmp,sumai,nomer,rukov,glavbuh,ffr,fftmp,wpredok);

class iceb_u_spisok imaf;
class iceb_u_spisok naimf;
imaf.plus(imafr);
imaf.plus(imafdbf);
naimf.plus(gettext("Ведомость для зачисления на карт-счёта"));
naimf.plus(gettext("Файл для передачи в банк"));

gdite.close();

iceb_rabfil(&imaf,&naimf,"",0,wpredok);
}
