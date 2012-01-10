/*$Id: b_pervinvw.c,v 1.7 2011-03-28 06:55:45 sasa Exp $*/
/*18.10.2009	25.03.2005	Белых А.И.	b_pervinvw.c
Подготовка файла платёжек для Первого инвесиционного банка
Если вернули 0- всё впорядке

Формат записи:

1  - Номер строки в файле (Начиная с 1)
2  - Статус докумета (всегда 0)
3  - Дебет МФО
4  - Дебет счёт
5  - Дебет ОКПО
6  - Кредит МФО
7  - Кредит счёт
8  - Кредит ОКПО
9  - Дата документа (дд.мм.гггг)
10 - Номер документа
11 - Название плательщика
12 - Название получателя
13 - Сумма документа
14 - Дата валютирования
15 - Дата акцепта
16 - Назначение платежа
17 - Валюта (по умолчанию 980, если не заполнено)
18 - Информационный (значения:F,T. По умолчанию F, если не заполнено
19 - Код назначения платежа НБУ (по умолчанию 0, если не заполнено)

*/

#include        <errno.h>
#include "buhg_g.h"
#include        "dok4w.h"

extern class REC rec;

int b_pervinvw(const char *tabl,GtkWidget *wpredok)
{

char		imaf[32],imaf1[32];
FILE		*ff,*ff1;
char		stt[1024];
class iceb_u_str koment("");
char		strsql[512];
short		d,m,g;
short dv=rec.dv ,mv=rec.mv,gv=rec.gv;
char		nomerdok[32];
time_t          vrem;
struct tm *bf;

//printw("b_brokbiznes\n");
//refresh();
if(dv == 0)
 {
  iceb_u_poltekdat(&dv,&mv,&gv);
 }
time(&vrem);
bf=localtime(&vrem);

sprintf(imaf,"platp.txt");
if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }


sprintf(imaf1,"plat.txt");
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
  fprintf(ff1,"%d|0|%s|%s|%s|%s|%s|%s|%02d.%02d.%d|%s|%s|%s|%.2f|%02d.%02d.%04d||%s|980|\n",
  nomstr,
  rec.mfo.ravno(),
  rec.nsh.ravno(),
  rec.kod.ravno(),
  rec.mfo1.ravno(),
  rec.nsh1.ravno(),
  rec.kod1.ravno(),
  d,m,g,
  nomerdok,
  rec.naior.ravno(),
  rec.naior1.ravno(),
  rec.sumd,
  dv,mv,gv,
  koment.ravno());


 }

fclose(ff1);
fclose(ff);

return(0);

}
