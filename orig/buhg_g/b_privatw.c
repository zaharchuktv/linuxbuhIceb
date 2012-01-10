/*$Id: b_privatw.c,v 1.7 2011-03-28 06:55:45 sasa Exp $*/
/*27.03.2007	23.03.2007	Белых А.И.	b_privatw.c
Экспорт платёжек для Приват Банка
В базе данных должна быть создана таблица с именем JBKL_SND
Колонки таблицы

TIP     Numeric    1 0  - тип документа 0- дебетовый информационный, платёжное требование, иначе - реальный кредитовый, платёжное поручение
N_D     Numeric   10 0  - Номер документа
DATE    Numeric    8    - Клиентская дата
SUMMA   Numeric   15 2  - Сумма документа
NAME_A  Character 40    - Наименование счёта плательщика (Думаю что это наименование банка)
COUNT_A Numeric   16 0  - Счёт плательщика
MFO_A   Numeric    9 0  - Не используется 
KSCH_A  Numeric   16 0  - Не используется
BANK_A  Character 45    - Не используется
NAME_B  Character 40    - Наименование счёта получателя (Думаю что этог наименование банка)
MFO_B   Numeric    9 0  - МФО банка получателя
COUNT_B Numeric   16 0  - Счёт получателя
KSCH_B  Numeric   16    - Наименование банка получателя (Думаю что это не заполняется)
BANK_B  Character 45    - Не используется
N_P     Character 160   - Назначение платежа
VAL     Character  4    - Валюта документа
PACKET  Character  1    - Не используется
USER    Character 20    - Не используется
K_S     Character  6    - Не используется
OKPO_A  Character 10 0  - Код ОКПО плательщика
OKP0_B  Character 10 0  - Код ОКПО получателя
SKP     Character  2    - Не используется
SOURSE  Character 12    - Не используется
OI      Character  3    - Не используется
                        
*/
#include        <errno.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern struct REC rec;

int b_privatw(const char *tabl,GtkWidget *wpredok)
{
char		imaf[32];
FILE		*ff;
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
   iceb_menu_soob("Нет подготовленых платёжек для передачи !",wpredok);
   return(1);
  }
 else
   {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return(1);
   }
 }
//Удаляем все записи в таблице
iceb_sql_zapis("DELETE FROM JBKL_SND",0,0,wpredok);
                        
while(fgets(stt,sizeof(stt),ff) != NULL)
 {
//  printw("%s",stt);
//  refresh();
  iceb_u_polen(stt,strsql,sizeof(strsql),1,'|');
  iceb_u_rsdat(&d,&m,&g,strsql,1);
  iceb_u_polen(stt,nomerdok,sizeof(nomerdok),2,'|');

  if(readpdokw(tabl,g,nomerdok,wpredok) != 0)
     continue;

  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);

  sprintf(strsql,"insert into JBKL_SND (TIP,N_D,DATE,SUMMA,NAME_A,COUNT_A,NAME_B,MFO_B,COUNT_B,\
N_P,VAL,OKPO_A,OKPO_B) values (%d,'%s','%d-%d-%d',%.2f,'%s',%s,'%s',%s,%s,'%s','%s','%s','%s')",
  1,nomerdok,g,m,d,rec.sumd,rec.naior.ravno(),rec.nsh.ravno(),rec.naior1.ravno(),rec.mfo1.ravno(),rec.nsh1.ravno(),
  koment.ravno(),"UAH",rec.kod.ravno(),rec.kod1.ravno());
  
  iceb_sql_zapis(strsql,1,0,wpredok);
    
 }

fclose(ff);
return(0);
}
                        
