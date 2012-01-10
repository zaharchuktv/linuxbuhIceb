/*$Id: zvb_alfabank_endw.c,v 1.4 2011-01-13 13:49:56 sasa Exp $*/
/*20.11.2009	15.10.2009	Белых А.И.	zvb_alfabank_endw.c
Концовки файлов
*/
#include <errno.h>
#include "buhg_g.h"
#include <unistd.h>
#include "dbfhead.h"

void alfabank_h(const char *imaf,long kolz,GtkWidget *wpredok);

void zvb_alfabank_endw(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp,
GtkWidget *wpredok)
{
class iceb_fioruk_rk ruk;
class iceb_fioruk_rk glbuh;

iceb_fioruk(1,&ruk,wpredok);
iceb_fioruk(2,&glbuh,wpredok);



fprintf(ffr,"\
------------------------------------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_u_kolbait(62,"Разом"),"Разом",sumai);


fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",ruk.fio.ravno(),glbuh.fio.ravno());

iceb_podpis(ffr,wpredok); 

fclose(ffr);

fclose(ff_dbf_tmp);

iceb_perecod(1,imaf_dbf_tmp,wpredok); /*Перекодировка*/

alfabank_h(imaf_dbf,kolzap,wpredok);
iceb_cat(imaf_dbf,imaf_dbf_tmp,wpredok);

unlink(imaf_dbf_tmp);

}

/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void alfabank_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void alfabank_h(const char *imaf,long kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 8 //Количество колонок (полей) в dbf файле
DBASE_FIELD f[kolpol];
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
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
alfabank_f(&f[shetshik++],"NPP", 'N', 6, 0,&header_len,&rec_len);
alfabank_f(&f[shetshik++],"FAMIL", 'C', 30, 0,&header_len,&rec_len);
alfabank_f(&f[shetshik++],"IMA", 'C', 30, 0,&header_len,&rec_len);
alfabank_f(&f[shetshik++],"OTH", 'C', 30, 0,&header_len,&rec_len);
alfabank_f(&f[shetshik++],"NOMERKS", 'C', 14, 0,&header_len,&rec_len);
alfabank_f(&f[shetshik++],"SUMA", 'N', 10, 2,&header_len,&rec_len);
alfabank_f(&f[shetshik++],"INN", 'C', 14, 0,&header_len,&rec_len);
alfabank_f(&f[shetshik++],"KEDRPOU", 'C', 14, 0,&header_len,&rec_len);


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
