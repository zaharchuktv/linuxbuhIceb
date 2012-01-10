/*$Id: zvb_promekonomw.c,v 1.5 2011-02-21 07:36:00 sasa Exp $*/
/*10.12.2009	10.12.2009	Белых А.И.	zvb_promekonomw.c
Формирование файлов для зачисления на картсчета зарплаты
*/
#include <stdlib.h>
#include    "buhg_g.h"
#include "l_nahud.h"

extern SQL_baza bd;

void zvb_promekonomw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
char strsql[512];
char imafr[32];
char imaf_dbf[56];
char imaf_dbf_tmp[56];

FILE  *ff_dbf_tmp;
FILE  *ffr;
int   kolstr;


SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR curr;
SQL_str row,row1;
double suma=0.;



if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }
char fio[512];

if(zvb_promekonom_startw(imafr,imaf_dbf,imaf_dbf_tmp,&ff_dbf_tmp,&ffr,wpredok) != 0)
 return;


char kartshet[32];
double sumai=0.;
char inn[32];

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

  zvb_promekonom_strw(++nomer_zap,fio,kartshet,suma,inn,row[1],ff_dbf_tmp,ffr);


    
 }

zvb_promekonom_endw(imaf_dbf,imaf_dbf_tmp,sumai,nomer_zap,ffr,ff_dbf_tmp,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimf;
imafo.plus(imafr);
naimf.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imaf_dbf);
naimf.plus(gettext("Документ в электронном формате"));

iceb_ustpeh(imafo.ravno(0),1,wpredok);

iceb_rabfil(&imafo,&naimf,"",0,wpredok);

}
