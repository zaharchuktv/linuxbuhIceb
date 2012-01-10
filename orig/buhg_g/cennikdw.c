/*$Id: cennikdw.c,v 1.12 2011-02-21 07:35:51 sasa Exp $*/
/*10.11.2009	07.05.2003	Белых А.И.	cennikdw.c
Распечатка ценников для документа
*/
#include        <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include	"buhg_g.h"
#include <unistd.h>

void	cennikw(short d,short m,short g,char *kodtv,char *naim,double cena,char *shrihkod,char *organ,FILE *ff);

extern double	okrg1;  /*Округление 1*/
extern char     *organ;
extern SQL_baza  bd;

void   cennikdw(short dd,short md,short gd, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
GtkWidget *wpredok)
{
char 	strsql[512];
SQL_str	row,row1;
int 	kolstr=0;


sprintf(strsql,"select distinct kodm from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;

FILE *ff;
char imaf[40];

sprintf(imaf,"cen%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

char naim[512];
double cena=0.;
char   kodtv[40];
char   strihkod[32];
time_t          tmm;
struct  tm      *bf;
time(&tmm);
bf=localtime(&tmm);


while(cur.read_cursor(&row) != 0)
 {
  cena=0.;
  memset(naim,'\0',sizeof(naim));
  memset(strihkod,'\0',sizeof(strihkod));
  memset(kodtv,'\0',sizeof(kodtv));

  //Читаем цену продажи и наименование товара
  sprintf(strsql,"select naimat,strihkod,cenapr from Material where kodm=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    strncpy(naim,row1[0],sizeof(naim)-1);
    strncpy(strihkod,row1[1],sizeof(strihkod)-1);

    cena=atof(row1[2]);
    cena=iceb_u_okrug(cena,okrg1);
   }  
  strcpy(kodtv,row[0]);
  cennikw(bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,kodtv,naim,cena,strihkod,organ,ff);
 }

fclose(ff);

cenniksw(imaf,wpredok); //Преобразование в колонки

iceb_u_spisok imafils;
iceb_u_spisok naimf;

imafils.plus(imaf);
naimf.plus(gettext("Распечатка ценников для товаров"));

iceb_rabfil(&imafils,&naimf,"",0,wpredok);


}
