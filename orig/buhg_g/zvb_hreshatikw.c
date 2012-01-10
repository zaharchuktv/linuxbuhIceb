/*$Id: zvb_hreshatikw.c,v 1.7 2011-02-21 07:36:00 sasa Exp $*/
/*16.10.2009	04.04.2004	Белых А.И.	zvb_hreshatikw.c
Выгузка для банка Хрещатик
телефон 59-20-30
        59-20-31
        59-20-39
        59-20-40
Тюренков Алексей Валерьевич моб. 8068 8386276 <Tyurenkov@xbank.com.ua>
Начальник Кашпрук Валерий Валерьевич <Kashpruk@xbank.com.ua>

*/
#include <stdio.h>
#include <stdlib.h>
#include    "buhg_g.h"
#include "l_nahud.h"
#include "zvb_hreshatik.h"

extern SQL_baza bd;



void zvb_hreshatikw(short prn,const char *zapros,class l_nahud_rek *poisk,
GtkWidget *wpredok)
{
char strsql[512];
char imaf[32];
char imafr[32];
FILE  *ff;
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
double sum=0.;
while(cur.read_cursor(&row) != 0)
 {
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;
//  if(poidirnu(row,poisk,fio) != 0)
//     continue;

  sum=atof(row[4]);
  if(prn == 2)
    sum*=(-1);   
  suma+=sum;
 }

short d_pp=0,m_pp=0,g_pp=0;


if(zvb_hreshatik_startw(imafr,imaf,suma,&d_pp,&m_pp,&g_pp,&ff,&ffr,wpredok) != 0)
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


  zvb_hreshatik_strw(d_pp,m_pp,g_pp,&nomer_zap,fio,kartshet,inn,suma,ff,ffr);

    
 }

zvb_hreshatik_endw(sumai,nomer_zap,ffr,ff,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimf;
imafo.plus(imafr);
naimf.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imaf);
naimf.plus(gettext("Документ в электронном формате"));

iceb_ustpeh(imafo.ravno(0),1,wpredok);

iceb_rabfil(&imafo,&naimf,"",0,wpredok);

}


