/* $Id: rasnasw.c,v 1.7 2011-02-21 07:35:56 sasa Exp $ */
/*14.11.2009    09.07.1996      Белых А.И.      rasnasw.c
Распечатка истории настройки по заданному инвентарному номеру
и подразделению
*/
#include        <errno.h>
#include        <time.h>
#include        "buhg_g.h"
#include <unistd.h>

extern char	*organ;
extern SQL_baza bd;

void rasnasw(int in,GtkWidget *wpredok)
{
time_t 		tmm;
struct  tm      *bf;
long		kolstr;
SQL_str         row;
char		strsql[512];
FILE		*ff;
char		imaf[56];
char		naim[512];
SQLCURSOR cur;

//Читаем наименование инвентарного номера
memset(naim,'\0',sizeof(naim));
sprintf(strsql,"select naim from Uosin where innom=%d",in);
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
  strncpy(naim,row[0],sizeof(naim)-1); 

sprintf(strsql,"select * from Uosinp where innom=%d \
order by god,mes asc",in);
/*
printw("\n%s\n",strsql);
refresh();
*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),wpredok);
  return;
 }

sprintf(imaf,"uosnas%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

time(&tmm);
bf=localtime(&tmm);
iceb_u_zagolov(gettext("Список изменений по инвентарному номеру"),0,0,0,0,0,0,organ,ff);

fprintf(ff,"%s: %d %s\n",
gettext("Инвентарный номер"),in,naim);

fprintf(ff,"\
----------------------------------------------------------------------------------\n");

fprintf(ff,"\
  Дата | Счет  |Шифр  |Шифр анал.|  Группа |Коэффици.|Состояние|Номерной|Кто зап.\n\
       | учёта |затрат|  учёта   |Нал.|Бух.|Нал.|Бух.|объекта  | знак   |\n");
/*
1234567 1234567 123456 1234567890 1234 1234 1234 1234 123456789 12345678
*/

fprintf(ff,"\
----------------------------------------------------------------------------------\n");
char sostob[50];

while(cur.read_cursor(&row) != 0)
 {
  if(row[8][0] == '0')
   strcpy(sostob,"Бух+Нал+");
  if(row[8][0] == '1')
   strcpy(sostob,"Бух-Нал-");
  if(row[8][0] == '2')
   strcpy(sostob,"Бух+Нал-");
  if(row[8][0] == '3')
   strcpy(sostob,"Бух-Нал+");
   
  fprintf(ff,"%2s.%s %-*s %-*s %-*s %-*s %-*s %-4s %-4s %-*s %-*s %-8s\n",
  row[1],
  row[2],
  iceb_u_kolbait(7,row[3]),row[3],
  iceb_u_kolbait(6,row[4]),row[4],
  iceb_u_kolbait(10,row[5]),row[5],
  iceb_u_kolbait(4,row[6]),row[6],
  iceb_u_kolbait(4,row[12]),row[12],
  row[7],
  row[13],
  iceb_u_kolbait(9,sostob),sostob,
  iceb_u_kolbait(8,row[9]),row[9],
  row[10]);

 }
iceb_podpis(ff,wpredok);
fclose(ff);


class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf);
naimf.plus(gettext("Список изменений по инвентарному номеру"));

iceb_ustpeh(imaf,3,wpredok);

iceb_rabfil(&imafil,&naimf,"",0,wpredok);

}




