/*$Id: zvb_alfabankw.c,v 1.5 2011-02-21 07:36:00 sasa Exp $*/
/*19.10.2009	15.10.2009	Белых А.И.	zvb_alfabankw.c
Формирование файлов для зачисления на картсчета зарплаты
*/
#include <stdlib.h>
#include    "buhg_g.h"
#include "l_nahud.h"

int zvb_alfabank_startw(char *imafr,char *imaf_dbf, char *imaf_dbf_tmp,FILE **ff,FILE **ffr,GtkWidget *wpredok);
void zvb_alfabank_endw(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp,GtkWidget *wpredok);
void zvb_alfabank_strw(int *nomerpp,const char *fio,const char *nomersh,double suma,const char *inn,const char *kedrpou,FILE *ff_dbf_tmp,FILE *ffras);

extern SQL_baza bd;


void zvb_alfabankw(short prn,const char *zapros,class l_nahud_rek *poisk,
GtkWidget *wpredok)
{
char strsql[512];
char imafr[32];
char imaf_dbf[56];
char imaf_dbf_tmp[56];
char kedrpou[32];
FILE  *ff_dbf_tmp;
FILE  *ffr;
int   kolstr;


SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR curr;
SQL_str row,row1;
double suma=0.;

memset(kedrpou,'\0',sizeof(kedrpou));

sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 strncpy(kedrpou,row[0],sizeof(kedrpou)-1);




if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }
char fio[512];

if(zvb_alfabank_startw(imafr,imaf_dbf,imaf_dbf_tmp,&ff_dbf_tmp,&ffr,wpredok) != 0)
 return;


char kartshet[32];
double sumai=0.;
char inn[20];

cur.poz_cursor(0);
int nomer_zap=0;
while(cur.read_cursor(&row) != 0)
 {
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  suma=atof(row[4]);
  if(suma == 0.)
   continue;
  if(prn == 2)
    suma*=(-1);   

  memset(kartshet,'\0',sizeof(kartshet));
  memset(fio,'\0',sizeof(fio));
  memset(inn,'\0',sizeof(inn));
    
  sprintf(strsql,"select fio,bankshet,inn from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    strncpy(fio,row1[0],sizeof(fio)-1);
    strncpy(kartshet,row1[1],sizeof(kartshet)-1);
    strncpy(inn,row1[2],sizeof(inn)-1);
   }

  if(kartshet[0] == '\0')
   {
    sprintf(strsql,"%s\n%s:%s %s",gettext("Не ввели счёт!"),gettext("Табельный номер"),row[1],fio);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }


  sumai+=suma;

  zvb_alfabank_strw(&nomer_zap,fio,kartshet,suma,inn,kedrpou,ff_dbf_tmp,ffr);


    
 }

zvb_alfabank_endw(imaf_dbf,imaf_dbf_tmp,sumai,nomer_zap,ffr,ff_dbf_tmp,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimf;
imafo.plus(imafr);
naimf.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imaf_dbf);
naimf.plus(gettext("Документ в электронном формате"));

iceb_ustpeh(imafo.ravno(0),1,wpredok);

iceb_rabfil(&imafo,&naimf,"",0,wpredok);

}
