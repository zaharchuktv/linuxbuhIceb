/*$Id: zvb_nadraw.c,v 1.11 2011-02-21 07:36:00 sasa Exp $*/
/*19.10.2006	26.09.2003	Белых А.И.	zvb_nadraw.c
Получение распечатки для передачи данных по зарплате в банк Надра
*/
#include <stdlib.h>
#include        <errno.h>
#include    "buhg_g.h"
#include <unistd.h>
#include "l_nahud.h"

extern char *organ;
extern SQL_baza bd;

void zvb_nadraw(short prn,const char *zapros,class l_nahud_rek *poisk,GtkWidget *wpredok)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);
char		strsql[512];
int		kolstr=0;
SQL_str		row,row1;
SQLCURSOR curr;
char		bros[512];
char		rukov[512];
char            glavbuh[512];
FILE		*ff;
char            imaf[32];

memset(rukov,'\0',sizeof(rukov));
memset(glavbuh,'\0',sizeof(glavbuh));

memset(bros,'\0',sizeof(bros));
iceb_poldan("Табельный номер руководителя",bros,"zarnast.alx",wpredok);
if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(rukov,row1[0],sizeof(rukov)-1);
 } 

memset(bros,'\0',sizeof(bros));
iceb_poldan("Табельный номер бухгалтера",bros,"zarnast.alx",wpredok);

if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(glavbuh,row1[0],sizeof(glavbuh)-1);
 }

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,zapros)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),zapros,wpredok);
  return;
 }

sprintf(imaf,"nadra%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
fprintf(ff,"\
                                      до Договору про надання послуг\n\
                                      N___________\"____\"________2003р.\n\n\
      Підприємство %s\n\n\
                     Зведена відомість N_____від \"_____\"____________р.\n\n",organ);
                     

fprintf(ff,"\
----------------------------------------------------------------------------\n\
 N  |N картрахунку|  Тип картки  |          ПІБ                 |   Сумма  |\n\
----------------------------------------------------------------------------\n"); 
double suma=0;
char fio[512];
char kshet[32];
double itogo=0.;
char tipkk[32];
int nomer=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  memset(fio,'\0',sizeof(fio));
  memset(kshet,'\0',sizeof(kshet));
  memset(tipkk,'\0',sizeof(tipkk));
  //Читем карточку
  sprintf(strsql,"select fio,bankshet,tipkk from Kartb where tabn=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
   {
    strncpy(fio,row1[0],sizeof(fio)-1);
    strncpy(kshet,row1[1],sizeof(kshet)-1);
    strncpy(tipkk,row1[2],sizeof(tipkk)-1);
   }
  suma=atof(row[4]);
  if(prn == 2)
   suma*=-1;
   
  itogo+=suma;
    
  fprintf(ff,"%-4d %-13s %-14s %-*.*s %10.2f\n",
  ++nomer,kshet,tipkk,iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,suma);

 }
fprintf(ff,"\
----------------------------------------------------------------------------\n");

fprintf(ff,"\
Загальна сума до виплати, грн.                                   %10.2f\n",itogo);

fprintf(ff,"\
Сумма коміссії Банку, грн.\n\
Загальна сума для перерахування в банк, грн.\n\
Дата виплати заробітної плати,\n\n");

fprintf(ff,"\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov,glavbuh);

fclose(ff);

class iceb_u_spisok imafr;
class iceb_u_spisok naimf;
imafr.plus(imaf);
naimf.plus(gettext("Ведомость для зачисления на карт-счёта"));

iceb_ustpeh(imaf,1,wpredok);

gdite.close();

iceb_rabfil(&imafr,&naimf,"",0,wpredok);

}

