/*$Id: b_iBank_2_UAw.c,v 1.10 2011-03-28 06:55:45 sasa Exp $*/
/*09.06.2006	09.06.2006	Белых А.И.	b_iBank_2_UA.c

Экспорт платёжек для системы iBank 2 UA
Кодировка Сp1251
---------------------------------------------------------------
Наименование    Описание                        Тип        
поля
---------------------------------------------------------------
DATE_DOC        Дата документа                  date     10
NUM_DOC         Номер документа                 string   10
AMOUNT          Сумма документа                 decimal  11.2
CLN_NAME        Наименование плательщика        string   40
CLN_OKPO        ЕГРПОУ плательщика              string   10
CLN_ACCOUNT     Счёт плательщика                string   14
CLN_BANK_NAME   Наименование банка плательщика  string   45
CLN_BANK_MFO    МФО банка плательщика           string   6
RCPT_NAME       Наименование получателя         string   40
RCPT_OKPO       ЕГРПОУ получателя               string   10
RCPT_ACCOUNT    Счёт получателя                 string   14
RCPT_BANK_NAME  Наименование банка получателя   string   45
RCPT_BANK_MFO   МФО банка получателя            string   6
PAYMENT_DETAILS Назначение платежа              string   160
VALUE_DATE      Дата валютирования              date     10


*/

#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern struct REC rec;

void b_iBank_2_UAw(const char *tabl,GtkWidget *wpredok)
{
char		imaf[32],imaf1[32];
FILE		*ff,*ff1;
char		stt[1024];
class iceb_u_str koment("");
char		strsql[512];
short		d,m,g;
char		nomerdok[32];

sprintf(imaf,"platp.txt");
if((ff = fopen(imaf,"r")) == NULL)
 {
 if(errno == ENOENT)
  {
   return;
  }
 else
   {
    return;
   }
 }
sprintf(imaf1,"plat.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }
fprintf(ff1,"Content-Type=doc/ua_payment\n\n");
short dv,mv,gv;
while(fgets(stt,sizeof(stt),ff) != NULL)
 {
  iceb_u_polen(stt,strsql,sizeof(strsql),1,'|');
  iceb_u_rsdat(&d,&m,&g,strsql,1);
  iceb_u_polen(stt,nomerdok,sizeof(nomerdok),2,'|');

  if(readpdokw(tabl,g,nomerdok,wpredok) != 0)
     continue;

  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);

  dv=rec.dv;
  mv=rec.mv;
  gv=rec.gv;
  if(dv == 0)
   {
    dv=d;
    mv=m;
    gv=g;
   }
  fprintf(ff1,"\
DATE_DOC=%02d.%02d.%04d\n\
NUM_DOC=%s\n\
AMOUNT=%.2f\n\
CLN_NAME=%.40s\n\
CLN_OKPO=%s\n\
CLN_ACCOUNT=%s\n\
CLN_BANK_NAME=%.*s\n\
CLN_BANK_MFO=%s\n\
RCPT_NAME=%.40s\n\
RCPT_OKPO=%s\n\
RCPT_ACCOUNT=%s\n\
RCPT_BANK_NAME=%.*s\n\
RCPT_BANK_MFO=%.6s\n\
PAYMENT_DETAILS=%.*s\n\
VALUE_DATE=%02d.%02d.%04d\n\n",
  d,m,g,
  nomerdok,
  rec.sumd,
  rec.naior.ravno(),
  rec.kod.ravno(),
  rec.nsh.ravno(),
  iceb_u_kolbait(45,rec.naiban.ravno()),rec.naiban.ravno(),
  rec.mfo.ravno(),
  rec.naior1.ravno(),
  rec.kod1.ravno(),
  rec.nsh1.ravno(),
  iceb_u_kolbait(45,rec.naiban1.ravno()),rec.naiban1.ravno(),
  rec.mfo1.ravno(),
  iceb_u_kolbait(160,koment.ravno()),koment.ravno(),
  dv,mv,gv);
  
  
 }

fclose(ff1);
fclose(ff);
iceb_perecod(2,imaf1,wpredok);
}
