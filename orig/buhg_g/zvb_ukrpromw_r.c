/*$Id: zvb_ukrpromw_r.c,v 1.12 2011-02-21 07:36:00 sasa Exp $*/
/*29.09.2009	01.07.2005	Белых А.И.	zvb_ukrpromw_r.c
Выгрузка сумм для зачисления на карт-счета для Укрпромбанка
*/
#include <stdlib.h>
#include        <errno.h>
#include    "buhg_g.h"
#include <unistd.h>
#include        "dbfhead.h"
#include "l_nahud.h"
#include "zvb_ukrpromw.h"
void		ukrprom_h(char *imaf,long kolz);

extern char *organ;
extern SQL_baza bd;

void zvb_ukrpromw_r(short prn,const char *zapros,class zvb_ukrpromw_rek *rek,class l_nahud_rek *poisk,GtkWidget *wpredok)
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


time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);

sprintf(imaftmp,"ukrprom%d.tmp",getpid());
if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }
sprintf(imafr,"ukrprom%d.lst",getpid());
if((ffr = fopen(imafr,"w")) == NULL)
 {
  iceb_er_op_fil(imafr,"",errno,wpredok);
  return;
 }
iceb_u_startfil(ffr);

fprintf(ffr,"%s\n\n",organ);

fprintf(ffr,"\
          Відомість для зарахування на карт-рахунки N_____\n\
                від %d.%d.%dр.\n\n",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);

fprintf(ffr,"\
-----------------------------------------------------------------------\n");
fprintf(ffr,"\
 Н/п|Таб.номер| Карт-счёт |  Сумма    | Призвище,і'мя, по батькові\n");
fprintf(ffr,"\
-----------------------------------------------------------------------\n");




char fio[512];

float kolstr1=0.;
int nomer=0;

char kartshet[32];
double suma;
double sumai=0.;
char inn[32];

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
//  strzag(LINES-1,0,kolstr,++kolstr1);
  if(l_nahud_prov_str(row,poisk,wpredok) != 0)
   continue;

  memset(kartshet,'\0',sizeof(kartshet));
  memset(fio,'\0',sizeof(fio));
  memset(inn,'\0',sizeof(inn));
  
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

  suma=atof(row[4]);
  if(suma == 0.)
   continue;
  if(prn == 2)
    suma*=(-1);   
  

  fprintf(fftmp," %-10.10s%-20.20s%10.2f",inn,kartshet,suma);


  fprintf(ffr,"%4d %-9s %-14s %10.2f %s\n",
  ++nomer,row[1],kartshet,suma,fio);
  sumai+=suma;
    
 }
fputc(26, fftmp);
fprintf(ffr,"\
-----------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_u_kolbait(29,"Разом"),"Разом",sumai);


fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov,glavbuh);
iceb_podpis(ffr,wpredok); 
fclose(fftmp);
fclose(ffr);

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

sprintf(imafdbf,"%02d%02d%s%d.dbf",mt,dt,rek->kod_or.ravno(),rek->nomer_fil.ravno_atoi());

ukrprom_h(imafdbf,nomer);

/*Сливаем два файла*/
iceb_cat(imafdbf,imaftmp,wpredok);
unlink(imaftmp);
if(rek->ima_mah.getdlinna() > 1)
 {
  //Переписываем файл в нужный каталог
  sprintf(strsql,"bankcp %s %s %s %s %s",
  rek->ima_mah.ravno(),imafdbf,imafdbf,rek->ima_set_disk.ravno(),rek->parol.ravno());

  int koh=system(strsql);

  class iceb_u_str repl;
  if(koh != 0)
   {
    
    sprintf(strsql,gettext("Передача файла не произошла ! Код ошибки %d"),koh);
    repl.plus(strsql);
    
    sprintf(strsql,"bankcp %s %s %s %s %s",
    rek->ima_mah.ravno(),imafdbf,imafdbf,rek->ima_set_disk.ravno(),rek->parol.ravno());
    repl.ps_plus(imafdbf);
   }
  if(koh == 0)
   {
    repl.plus(gettext("Передача файла произошла успешно."));
    repl.ps_plus(imafdbf);
   }     
  iceb_menu_soob(&repl,wpredok);

 }
//printw("Имя файла - %s\n",imafdbf);
//OSTANOV();
class iceb_u_spisok imaf;
class iceb_u_spisok naimf;
imaf.plus(imafr);
imaf.plus(imafdbf);
naimf.plus(gettext("Ведомость для зачисления на карт-счёта."));
naimf.plus(gettext("Файл для передачи в банк."));

gdite.close();

iceb_rabfil(&imaf,&naimf,"",0,wpredok);
}

/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void ukrprom_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
int *header_len,int *rec_len)
{

strncpy(f->name, fn, 11);
f->type = ft;
f->length = l1;
f->dec_point = l2;
*header_len=*header_len+sizeof(DBASE_FIELD);
*rec_len=*rec_len+l1;
}

/***********************************************/
void		ukrprom_h(char *imaf,long kolz)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 3 //Количество колонок (полей) в dbf файле
DBASE_FIELD f[kolpol];
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
  return;
 }

memset(&h,'\0',sizeof(h));
memset(&f, '\0', sizeof(f));

h.version = 3;

time(&tmm);
bf=localtime(&tmm);

h.l_update[0] = bf->tm_year;       /* yymmdd for last update*/
h.l_update[1] = bf->tm_mon+1;       /* yymmdd for last update*/
h.l_update[2] = bf->tm_mday;       /* yymmdd for last update*/

h.count = kolz;              /* number of records in file*/

header_len = sizeof(h);
rec_len = 0;
int shetshik=0;
ukrprom_f(&f[shetshik++],"IDCODE", 'C', 10, 0,&header_len,&rec_len);
ukrprom_f(&f[shetshik++],"CARDACC", 'C', 20, 0,&header_len,&rec_len);
ukrprom_f(&f[shetshik++],"AMT", 'N', 10, 2,&header_len,&rec_len);


h.header = header_len + 1;/* length of the header
                           * includes the \r at end
                           */
h.lrecl= rec_len + 1;     /* length of a record
                           * includes the delete
                           * byte
                          */
/*
 printw("h.header=%d h.lrecl=%d\n",h.header,h.lrecl);
*/


fd = fileno(ff);

write(fd, &h, sizeof(h));
for(i=0; i < kolpol; i++) 
 {
  write(fd, &f[i], sizeof(DBASE_FIELD));
 }
fputc('\r', ff);

fclose(ff);

}
