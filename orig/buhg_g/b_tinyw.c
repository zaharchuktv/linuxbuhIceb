/*$Id: b_tinyw.c,v 1.14 2011-03-28 06:55:46 sasa Exp $*/
/*21.05.2010	16.07.2008	Белых А.И.	b_tinyw.c
Экспорт для подсистемы "Tiny" 
OrgDate    D8    - Дата платежа
Nom        C10   - Номер документа
Summa      N19.2 - Сумма
CmId       N3    - Числовой код валюты (основная 0)
DebAcc     C32   - Дебет счёт
CrdMfo     N6    - Кредит МФО
CrdAcc     C32   - Кредит счёт
CrdAccName C38   - Наименование получетеля
CrdCliCode C10   - Код ОКПО получателя
Note       C160  - Назначение платежа

Следующие поля заполняет только БО

Action     C4    - Тип операции (только БО)
Ctrl       C10   - Символ кассового плана (только БО)
Kind       C10   - Вид документа (только БО)
Notes      C160  - Коментарии          
*****************************/
#include <stdlib.h>
#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"
#include        "dbfhead.h"
#include <unistd.h>

void		b_tiny_h(char *imaf,long kolz,GtkWidget*);

extern struct REC rec;

void b_tinyw(const char *tabl,GtkWidget *wpredok)
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
class iceb_u_str oshet("");

//printw("b_tiny\n");
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
iceb_poldan("Окончание для счёта для системы Tiny",&oshet,"banki.alx",wpredok);

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
char nomer_ras_sh[32];

while(fgets(stt,sizeof(stt),ff) != NULL)
 {
//  printw("%s",stt);
  iceb_u_polen(stt,strsql,sizeof(strsql),1,'|');
  iceb_u_rsdat(&d,&m,&g,strsql,1);
  memset(nomerdok,'\0',sizeof(nomerdok));
  iceb_u_polen(stt,nomerdok,sizeof(nomerdok),2,'|');

  if(readpdokw(tabl,g,nomerdok,wpredok) != 0)
     continue;

  sprintf(nomer_ras_sh,"%s%s",rec.nsh.ravno(),oshet.ravno());
//  sprintf(nomer_ras_sh,"%s.980",rec.nsh.ravno());
  
  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);

  nomstr++;  

  fprintf(ff1," %04d%02d%02d\
%-*.*s\
%19.2f\
%3d\
%-*.*s\
%-*.*s\
%-*.*s\
%-*.*s\
%-*.*s\
%-*.*s\
%4s\
%10s\
%10s\
%160s",
  g,m,d,
  iceb_u_kolbait(10,nomerdok),iceb_u_kolbait(10,nomerdok),nomerdok,
  rec.sumd,
  0,
  iceb_u_kolbait(32,nomer_ras_sh),iceb_u_kolbait(32,nomer_ras_sh),nomer_ras_sh,
  iceb_u_kolbait(6,rec.mfo1.ravno()),iceb_u_kolbait(6,rec.mfo1.ravno()),rec.mfo1.ravno(),
  iceb_u_kolbait(32,rec.nsh1.ravno()),iceb_u_kolbait(32,rec.nsh1.ravno()),rec.nsh1.ravno(),
  iceb_u_kolbait(38,rec.naior1.ravno()),iceb_u_kolbait(38,rec.naior1.ravno()),rec.naior1.ravno(),
  iceb_u_kolbait(10,rec.kod1.ravno()),iceb_u_kolbait(10,rec.kod1.ravno()),rec.kod1.ravno(),
  iceb_u_kolbait(160,koment.ravno()),iceb_u_kolbait(160,koment.ravno()),koment.ravno(),
  "","","","");

 }
fputc(26, ff1);

fclose(ff1);
fclose(ff);

//Перекодировка
iceb_perecod(1,imaf1,wpredok);

char imafdbf[30];

sprintf(imafdbf,"plat.txt");
b_tiny_h(imafdbf,nomstr,wpredok);

/*Сливаем два файла*/
iceb_cat(imafdbf,imaf1,wpredok);  


unlink(imaf1);


}
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void b_tiny_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void		b_tiny_h(char *imaf,long kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
#define kolpol  14
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
b_tiny_f(&f[shetshik++],"OrgDate", 'D', 8, 0,&header_len,&rec_len);
b_tiny_f(&f[shetshik++],"Nom", 'C', 10, 0,&header_len,&rec_len);
b_tiny_f(&f[shetshik++],"Summa", 'N', 19, 2,&header_len,&rec_len);
b_tiny_f(&f[shetshik++],"CrnId", 'N', 3, 0,&header_len,&rec_len);
b_tiny_f(&f[shetshik++],"DebAcc", 'C',32, 0,&header_len,&rec_len);
b_tiny_f(&f[shetshik++],"CrdMfo", 'N',6, 0,&header_len,&rec_len);
b_tiny_f(&f[shetshik++],"CrdAcc", 'C', 32, 0,&header_len,&rec_len);
b_tiny_f(&f[shetshik++],"CrdAccName", 'C', 38, 0,&header_len,&rec_len);
b_tiny_f(&f[shetshik++],"CrdCliCode", 'C', 10, 4,&header_len,&rec_len);
b_tiny_f(&f[shetshik++],"Note", 'C', 160, 0, &header_len,&rec_len);

b_tiny_f(&f[shetshik++],"Action", 'C', 4, 0, &header_len,&rec_len);
b_tiny_f(&f[shetshik++],"Ctrl", 'C', 10, 0, &header_len,&rec_len);
b_tiny_f(&f[shetshik++],"Kind", 'C', 10, 0, &header_len,&rec_len);
b_tiny_f(&f[shetshik++],"Notes", 'C', 160, 0, &header_len,&rec_len);


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
