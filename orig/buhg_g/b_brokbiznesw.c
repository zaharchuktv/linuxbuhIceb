/*$Id: b_brokbiznesw.c,v 1.11 2011-03-28 06:55:45 sasa Exp $*/
/*29.10.2009	21.07.2004	Белых А.И. 	b_brokbiznesw.c
Формирование файла для подсистемы Клиент-банк для Врокбизнес банка.
Структура записи dbf файла

dtpd     DATE   8    Дата платёжного документа
npd      char   10   Номер платёжного документа
dmfo     int    11   Дебет МФО (ПЛАТЕЛЬШИКА)
cmfo     int    11   Кредит МФО (ПОЛУЧАТЕЛЯ)
dacc     char   17   Дебет Р/С (ПЛАТЕЛЬЩИКА)
cacc     char   17   Кредит Р/С (ПОЛУЧАТЕЛЯ)
dokpo    char   17   Дебет ОКПО (ПЛАТУЛЬЩИКА)
cokpo    char   17   Кредит ОКПО (ПОЛУЧАТУЛЯ)
amount   double 19.4 Сумма
currency short  6    Код валюты (980)
remark   char   160  Назначение платежа
dname    char   50   Название плательщика
cname    char   50   Название получателя

Телефон 520827 Андрей Александрович
Адрес - Киевская 14  


*/
#include <stdlib.h>
#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"
#include <unistd.h>
#include        "dbfhead.h"

int		b_brokbiznes_h(char *imaf,long kolz,GtkWidget *wpredok);

extern class REC rec;

int b_brokbiznesw(const char *tabl,GtkWidget *wpredok)
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
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }


while(fgets(stt,sizeof(stt),ff) != NULL)
  koldok++;
rewind(ff);

sprintf(imaf1,"plat_b.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return(1);
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


  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);
  
  nomstr++;  
  
  fprintf(ff1," %04d%02d%02d%-10.10s%11.11s%11.11s%-17.17s%-17.17s%-17.17s%-17.17s%19.4f\
%6d%-*.*s%-*.*s%-*.*s",
  g,m,d,
  nomerdok,
  rec.mfo.ravno(),
  rec.mfo1.ravno(),
  rec.nsh.ravno(),
  rec.nsh1.ravno(),
  rec.kod.ravno(),
  rec.kod1.ravno(),
  rec.sumd,
  980,
  iceb_u_kolbait(160,koment.ravno()),iceb_u_kolbait(160,koment.ravno()),koment.ravno(),
  iceb_u_kolbait(50,rec.naior.ravno()),iceb_u_kolbait(50,rec.naior.ravno()),rec.naior.ravno(),
  iceb_u_kolbait(50,rec.naior1.ravno()),iceb_u_kolbait(50,rec.naior1.ravno()),rec.naior1.ravno());


 }
fputc(26, ff1);

fclose(ff1);
fclose(ff);

//Перекодировка
iceb_perecod(2,imaf1,wpredok);
char imafdbf[30];

sprintf(imafdbf,"plat.txt");
b_brokbiznes_h(imafdbf,nomstr,wpredok);

/*Сливаем два файла*/
iceb_cat(imafdbf,imaf1,wpredok);  


unlink(imaf1);

return(0);

}
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void b_brokbiznes_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
int		b_brokbiznes_h(char *imaf,long kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
#define kolpol  13
DBASE_FIELD f[kolpol];
memset(&f, '\0', sizeof(f));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
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
b_brokbiznes_f(&f[shetshik++],"dtpd", 'D', 8, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"npd", 'C', 10, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"dmfo", 'N', 11, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"cmfo", 'N', 11, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"dacc", 'C',17, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"cacc", 'C',17, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"dokpo", 'C', 17, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"cokpo", 'C', 17, 0,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"amount", 'N', 19, 4,&header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"currency", 'N', 6, 0, &header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"remark", 'C', 160, 0, &header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"dname", 'C', 50, 0, &header_len,&rec_len);
b_brokbiznes_f(&f[shetshik++],"cname", 'C', 50, 0, &header_len,&rec_len);


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
return(0);
}
