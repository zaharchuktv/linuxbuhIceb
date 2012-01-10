/*$Id: b_ukrsocw.c,v 1.8 2011-03-28 06:55:46 sasa Exp $*/
/*09.11.2009	09.06.2006	Белых А.И.	b_ukrsoc.c
Экспорт платёжки для "Укрсоц" банка.

Формат файлов: текстовый; Кодировка:DOS(CP-866); Разделитель полей: "|" (пайп);

Имя файла "c_epd.txt"
1 Номер строки в файле (Начиная с 1)
2 Статус документа (всегда 0)
3 Дебет МФО
4 Дебет счёт
5 Дебет ОКПО
6 Кредит МФО
7 Кредит счёт
8 Кредит МФО
9 Дата документа (ДД.ММ.ГГГГ) (разделитель "точка" ASCII 46)
10 Номер документа
11 Название плательщика (до 38 символов)
12 Название получателя  (до 38 символов)
13 Сумма документа (формат например 210.00)
14 Дата акцепта (не заполняется)
15 Время акчепта (не заполняется)
16 Назначение платежа (до 160 символов)
17 Валюта (по умолчанию 980, если не заполнено)
18 Информационный (значение:F,T. По умолчанию F, если не заполнено)
19 Код назначения платежа НБУ (по умолчанию 0, если не заполнено)
*/
#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern struct REC rec;

void b_ukrsocw(const char *tabl,GtkWidget *wpredok)
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
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return;
   }
 }
sprintf(imaf1,"plat.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }
int nom_str=0;
while(fgets(stt,sizeof(stt),ff) != NULL)
 {
  iceb_u_polen(stt,strsql,sizeof(strsql),1,'|');
  iceb_u_rsdat(&d,&m,&g,strsql,1);
  iceb_u_polen(stt,nomerdok,sizeof(nomerdok),2,'|');

  if(readpdokw(tabl,g,nomerdok,wpredok) != 0)
     continue;

  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);


  fprintf(ff1,"%d|0|%s|%s|%s|%s|%s|%s|%02d.%02d.%04d|%s|%.*s|%.*s|%.2f|||%.*s|980|\n",
  ++nom_str,
  rec.mfo.ravno(),
  rec.nsh.ravno(),
  rec.kod.ravno(),
  rec.mfo1.ravno(),
  rec.nsh1.ravno(),
  rec.kod1.ravno(),
  d,m,g,
  nomerdok, 
  iceb_u_kolbait(38,rec.naior.ravno()),rec.naior.ravno(),
  iceb_u_kolbait(38,rec.naior1.ravno()),rec.naior1.ravno(),
  rec.sumd,
  iceb_u_kolbait(160,koment.ravno()),koment.ravno());
  
  
 }

fclose(ff1);
fclose(ff);

}
