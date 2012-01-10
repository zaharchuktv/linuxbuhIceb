/*$Id: zvb_ibank2uaw.c,v 1.7 2011-06-07 08:52:27 sasa Exp $*/
/*17.05.2011	18.09.2008	Белых А.И.	zvb_ibank2uaw.c
Выгрузна в файл для экспорта в систему iBank2 UA
*/
#include <math.h>
#include <stdlib.h>
#include    "buhg_g.h"
#include "l_nahud.h"

extern SQL_baza bd;

void zvb_ibank2uaw(short prn,const char *zapros,class l_nahud_rek *poisk,const char *kod00,GtkWidget *wpredok)
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

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }
class iceb_u_str vidnahis(""); 

if(zvb_ibank2ua_startw(imafr,imaf,&vidnahis,kod00,&ff,&ffr,wpredok) != 0)
 return;

char kartshet[32];
char inn[32];
char fio[512];
double suma=0.;
double sumai=0.;
int nomer_zap=0;
while(cur.read_cursor(&row) != 0)
 {
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;
  suma=atof(row[4]);
  if(fabs(suma) < 0.01)
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

  zvb_ibank2ua_strw(&nomer_zap,fio,kartshet,inn,suma,ff,ffr);

 }
 
zvb_ibank2ua_endw(sumai,vidnahis.ravno(),ffr,ff,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimf;

imafo.plus(imafr);
naimf.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imaf);
naimf.plus(gettext("Документ в электронном формате"));

iceb_ustpeh(imafo.ravno(0),1,wpredok);

iceb_rabfil(&imafo,&naimf,"",0,wpredok);

}
