/*$Id: b_ukrbiznesw.c,v 1.1 2011-03-28 06:55:46 sasa Exp $*/
/*15.03.2011	15.03.2011	Белых А.И.	b_ukrbiznesw.c
Формирования файла с платёжным поручением в подсистему Клиент-Банк
для Укрбизнес банка.
телефон в банке 062 345 10 54
                062 345 10 51
                
; Описание импорта из DBF-файла
;
; Code = <кодировка>
;    где <кодировка>={Win|Dos|Ukr}
;
; далее следуют строки формата:
;
; <поле док-та>=<поле DBF>
;    Возможные типы полей DBF: N,C (числовые поля без десятичной точки)
;    Возможные значения <поле док-та>:
;	ACC1					- счет клиента
;	ACC2, MFO2, OKPO2, NAMEC2, NAMEB2	- параметры корреспондента
;	NDOC					- номер документа
;	SUMMA					- сумма в копейках
;	NP					- назначение платежа
;	DV					- дата валютирования
;
[ImportDbf]
Code	= Win
ACC1	= ACCCLI
ACC2	= ACCCOR
MFO2	= MFOCOR
OKPO2	= OKPOCOR
NAME2	= NAMECOR
BANK2	= BANKCOR
NDOC	= NDOC
SUMMA	= SUMMA
NP	= NAZN
DV	= DV
; -----------------------------------------------------------------------------
; Описание экспорта в DBF-файл
;
; Code = <кодировка>
;    где <кодировка>={Win|Dos|Ukr}
;
; далее следуют строки формата:
;
; <поле DBF>=<тип>,[<длинна>,]<поле док-та>
; <поле DBF>=<тип>,[<длинна>,]"<константа>"
;
;    Возможные типы полей DBF: N,C,D,L (числовые поля без десятичной точки)
;    Параметр <длинна> не указывается, если тип='D' или 'L'
;
;    Возможные значения <поле док-та>:
;
;	ACC1, MFO1, OKPO1, NAMEC1, NAMEB1	- параметры клиента
;	ACC2, MFO2, OKPO2, NAMEC2, NAMEB2	- параметры корреспондента
;	MFOD, ACCD, OKPOD, NAMECD, NAMEBD	- параметры счета по дебету
;	MFOC, ACCC, OKPOC, NAMECC, NAMEBC	- параметры счета по кредиту
;	NDOC					- номер документа
;	SUMMA					- сумма в копейках
;	NP					- назначение платежа
;	DATEV					- дата проводки
;	VAL					- код валюты
;	DK					- 0-дебет/1-кредит
;	VID					- тип документа
;	SUB					- субсчет (для коммунальных пл.)
;	DV					- дата валютирования
;
[ExportDbf]
Code	= Dos
ACCCLI	= N,14,	ACC1
MFOCLI	= N,6,	MFO1
OKPOCLI	= N,10,	OKPO1
NAMECLI	= C,40,	NAME1
BANKCLI	= C,40,	BANK1
ACCCOR	= N,14,	ACC2
MFOCOR	= N,6,	MFO2
OKPOCOR	= N,10,	OKPO2
NAMECOR	= C,40,	NAME2
BANKCOR	= C,40,	BANK2
NDOC	= C,10,	NDOC
SUMMA	= N,16,	SUMMA
NAZN	= C,160,NP
DT	= D,	DATEV
DK	= N,1,	DK
CVAL	= N,3,	VAL
TDOC	= N,2,	VID
DV	= D,	DV
IMPFLG	= L,	"T"
*/
#include <errno.h>
#include "buhg_g.h"
#include        "dok4w.h"
#include        "dbfhead.h"
#include <unistd.h>

void b_ukrbiznes_h(char *imaf,long kolz,GtkWidget *wpredok);
extern struct REC rec;
extern SQL_baza bd;

int b_ukrbiznesw(const char *tabl,GtkWidget *wpredok)
{
char strsql[1024];
char iceb_dbf_tmp[64];
FILE *ff_dbf_tmp;
class iceb_u_str nomerdok("");
class iceb_u_str koment("");
int kolstr=0;
class SQLCURSOR cur;
SQL_str row;

sprintf(strsql,"select datd,nomd from %s where vidpl='1'",tabl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного документа для передачи в банк!"),wpredok);
  return(1);
 }


sprintf(iceb_dbf_tmp,"plat_b.txt");
if((ff_dbf_tmp = fopen(iceb_dbf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(iceb_dbf_tmp,"",errno,wpredok);
  return(1);
 }

int nomstr=0;
short d,m,g;
//while(fgets(stroka,sizeof(stroka),ff) != NULL)
// {
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s",stroka);
//  polen(stroka,strsql,sizeof(strsql),1,'|');
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  nomerdok.new_plus(row[1]);
//  polen(stroka,&nomerdok,2,'|');

  if(readpdokw(tabl,g,nomerdok.ravno(),wpredok) != 0)
     continue;

  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);

  int suma_int=rec.sumd*100.;

  nomstr++;  

  fprintf(ff_dbf_tmp," %-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%16d%-*.*s00000000",
  iceb_u_kolbait(14,rec.nsh.ravno()),
  iceb_u_kolbait(14,rec.nsh.ravno()),
  rec.nsh.ravno(),
  iceb_u_kolbait(14,rec.nsh1.ravno()),
  iceb_u_kolbait(14,rec.nsh1.ravno()),
  rec.nsh1.ravno(),
  iceb_u_kolbait(6,rec.mfo1.ravno()),
  iceb_u_kolbait(6,rec.mfo1.ravno()),
  rec.mfo1.ravno(),
  iceb_u_kolbait(10,rec.kod1.ravno()),
  iceb_u_kolbait(10,rec.kod1.ravno()),
  rec.kod1.ravno(),
  iceb_u_kolbait(40,rec.naior1.ravno()),
  iceb_u_kolbait(40,rec.naior1.ravno()),
  rec.naior1.ravno(),
  iceb_u_kolbait(40,rec.naiban1.ravno()),
  iceb_u_kolbait(40,rec.naiban1.ravno()),
  rec.naiban1.ravno(),
  iceb_u_kolbait(10,nomerdok.ravno()),
  iceb_u_kolbait(10,nomerdok.ravno()),
  nomerdok.ravno(),
  suma_int,
  iceb_u_kolbait(160,koment.ravno()),
  iceb_u_kolbait(160,koment.ravno()),
  koment.ravno());


 }
fputc(26, ff_dbf_tmp);

fclose(ff_dbf_tmp);
//fclose(ff);

iceb_perecod(2,iceb_dbf_tmp,wpredok);

char imafdbf[64];

sprintf(imafdbf,"plat.txt");
b_ukrbiznes_h(imafdbf,nomstr,wpredok);

/*Сливаем два файла*/
iceb_cat(imafdbf,iceb_dbf_tmp,wpredok);
unlink(iceb_dbf_tmp);
return(0);

}
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void b_ukrbiznes_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void b_ukrbiznes_h(char *imaf,long kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
#define kolpol  10
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
b_ukrbiznes_f(&f[shetshik++],"ACCCLI", 'N', 14, 0,&header_len,&rec_len);
b_ukrbiznes_f(&f[shetshik++],"ACCCOR", 'N', 14, 0,&header_len,&rec_len);
b_ukrbiznes_f(&f[shetshik++],"MFOCOR", 'N', 6, 0,&header_len,&rec_len);
b_ukrbiznes_f(&f[shetshik++],"OKPOCOR", 'N', 10, 0,&header_len,&rec_len);
b_ukrbiznes_f(&f[shetshik++],"NAMECOR", 'C',40, 0,&header_len,&rec_len);
b_ukrbiznes_f(&f[shetshik++],"BANKCOR", 'C',40, 0,&header_len,&rec_len);
b_ukrbiznes_f(&f[shetshik++],"NDOC", 'C', 10, 0,&header_len,&rec_len);
b_ukrbiznes_f(&f[shetshik++],"SUMMA", 'N', 16, 0,&header_len,&rec_len);
b_ukrbiznes_f(&f[shetshik++],"NAZN", 'C', 160, 0,&header_len,&rec_len);
b_ukrbiznes_f(&f[shetshik++],"DV", 'D', 8, 0, &header_len,&rec_len);

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
