/*$Id: b_soft_revieww.c,v 1.10 2011-03-28 06:55:46 sasa Exp $*/
/*29.10.2009	14.08.2007	Белых А.И. 	b_soft_revieww.c
Формирование файла для подсистемы Клиент-банк для банка "Финатсы и кредит".
Вадим Машталлер тел. 611458
Структура записи dbf файла
Структура файлов импорта-экспорта для системы Клиент-банк

Имя файла для импорта WPAYDOC.DBF из каталога IN\
--------------------------------------------------

Record#  FIELD_NAME FIELD_TYPE FIELD_LEN FIELD_DEC
      1  VIDOP      N                003       000    -Вид операции /=11/
      2  DOC_NO     C                010       000    -Номер документа
      3  DOC_DATE   D                008       000    -Дата документа
      4  SUM        N                020       002    -Сумма
      5  P_ACC_NO   N                014       000    -Номер счета
      6  R_NAME     C                038       000    -Наименование получателя
      7  R_ACC_NO   N                014       000    -Счет получателя
      8  R_MFO      N                009       000    -МФО получателя
      9  R_ZIP      N                010       000    -ОКПО получателя
     10  COMMENT    C                160       000    -Назначение платежа
     11  DATEC      C                022       000    
     12  IDOPER     N                002       000    
     13  DATESEND   D                008       000    
     14  TIMESEND   N                010       000    
     15  CART       L                001       000    
     16  STATUS     N                001       000    
     17  ERRCODE    N                004       000    
     18  SIGN_BUH   C                103       000    
     19  SIGN_DIR   C                103       000    
     20  NUMPCKG    N                009       000    
     21  PL_HASH    C                032       000    

*** Примечание !!!
  Все не описанные поля должны передаваться пустыми


--------------------------------------------------------------
Экспорт данных
Имя файла для экспорта OUTDOCS.DBF каталог INP_OUT\

   Структура файла


Field  Field Name  Type       Width    Dec    
-----------------------------------------------
    1  VIDOP       Numeric        3            --      
    2  DOC_NO      Character     10            Номер документа      
    3  DATE        Date           8            Дата проводки      
    4  ACC_A       Numeric       14            Счет клиента A      
    5  ACC_B       Numeric       14            Счет клиента B      
    6  SUM         Numeric       16      2     Сумма      
    7  KURS        Numeric       16      2     Курс      
    8  FORC        Numeric       16      2     За      
    9  CURRENCY    Numeric        3            Код валюты      
   10  FL_DK       Numeric        1            Флаг дебет/кредет      
   11  R_NAME      Character     38            Наименование корреспондента      
   12  R_MFO       Numeric        9            МФО корреспондента            
   13  COMMENT     Character    160            Назначение платежа      
   14  R_ZIP       Numeric       14            ОКПО корреспондента      
   15  IDOPER      Numeric        2            ---
   16  DOC_DATE    Date           8            Дата документа      
   17  FL_LIST     Logical        1            ---      

  Примечание 
  ----------------
  ACC_A - Свой счет
  Флаг дебет/кредит если = 1, то по счету ACC_A оборот по дебету,
  = 0 соотвественно по кредиту.
  

*/
#include <stdlib.h>
#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"
#include        "dbfhead.h"
#include <unistd.h>

void		b_fintakred_h(char *imaf,long kolz,GtkWidget*);

extern struct REC rec;

void b_soft_revieww(const char *tabl,GtkWidget *wpredok)
{
char		imaf[32],imaf1[32];
FILE		*ff,*ff1;
char		stt[1024];
int		koldok=0;
class iceb_u_str koment("");
char		strsql[512];
short		d,m,g;
char		nomerdok[32];
time_t          vrem;
struct tm *bf;

//printw("b_brokbiznes\n");
//refresh();

time(&vrem);
bf=localtime(&vrem);

sprintf(imaf,"platp.txt");
if((ff = fopen(imaf,"r")) == NULL)
 {
 if(errno == ENOENT)
  {
   return;
  }
  else
   {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return;
   }
 }

while(fgets(stt,sizeof(stt),ff) != NULL)
  koldok++;
rewind(ff);

sprintf(imaf1,"plat_b.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

int nomstr=0;

while(fgets(stt,sizeof(stt),ff) != NULL)
 {
//  printw("%s",stt);
  iceb_u_polen(stt,strsql,sizeof(strsql),1,'|');
  iceb_u_rsdat(&d,&m,&g,strsql,1);
  memset(nomerdok,'\0',sizeof(nomerdok));
  iceb_u_polen(stt,nomerdok,sizeof(nomerdok),2,'|');

  if(readpdokw(tabl,g,nomerdok,wpredok) != 0)
     continue;

  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);

  nomstr++;  

  fprintf(ff1," 011%-10.10s%04d%02d%02d%20.2f%-14.14s%-*.*s%-14.14s%-9.9s%-10.10s%-*.*s\
%22s%2s%8s%10s%1s%1s%4s%103s%103s%9s%32s",
  nomerdok,
  g,m,d,
  rec.sumd,
  rec.nsh.ravno(),
  iceb_u_kolbait(38,rec.naior1.ravno()),iceb_u_kolbait(38,rec.naior1.ravno()),rec.naior1.ravno(),
  rec.nsh1.ravno(),
  rec.mfo1.ravno(),
  rec.kod1.ravno(),
  iceb_u_kolbait(160,koment.ravno()),iceb_u_kolbait(160,koment.ravno()),koment.ravno(),
  " "," "," "," "," "," "," "," "," "," "," ");

 }
fputc(26, ff1);

fclose(ff1);
fclose(ff);

//Перекодировка
iceb_perecod(1,imaf1,wpredok);


char imafdbf[30];

sprintf(imafdbf,"plat.txt");
b_fintakred_h(imafdbf,nomstr,wpredok);

/*Сливаем два файла*/
iceb_cat(imafdbf,imaf1,wpredok);  


unlink(imaf1);


}
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void b_fintakred_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void b_fintakred_h(char *imaf,long kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
#define kolpol  21
DBASE_FIELD f[kolpol];
memset(&f, '\0', sizeof(f));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

memset(&h,'\0',sizeof(h));

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

b_fintakred_f(&f[shetshik++],"VIDOP", 'N', 3, 0,&header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"DOC_NO", 'C', 10, 0,&header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"DOC_DATE", 'D', 8, 0,&header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"SUM", 'N', 20, 2,&header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"P_ACC_NO", 'N',14, 0,&header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"R_NAME", 'C',38, 0,&header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"R_ACC_NO", 'N', 14, 0,&header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"R_MFO", 'N', 9, 0,&header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"R_ZIP", 'N', 10, 0,&header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"COMMENT", 'C', 160, 0, &header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"DATEC", 'C', 22, 0, &header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"IDOPER", 'N', 2, 0, &header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"DATESEND", 'D', 8, 0, &header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"TIMESEND", 'N', 10, 0, &header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"CART", 'L', 1, 0, &header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"STATUS", 'N', 1, 0, &header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"ERRCODE", 'N', 4, 0, &header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"SIGN_BUH", 'C', 103, 0, &header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"SIGN_DIR", 'C', 103, 0, &header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"NUMPCKG", 'N', 9, 0, &header_len,&rec_len);
b_fintakred_f(&f[shetshik++],"PL_HASH", 'C', 32, 0, &header_len,&rec_len);


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
