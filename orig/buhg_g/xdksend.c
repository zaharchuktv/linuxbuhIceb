/*$Id: xdksend.c,v 1.21 2011-06-07 08:52:27 sasa Exp $*/
/*17.03.2011	17.05.2006	Белых А.И.	xdksend.c
Формирование файла для передачи в подсистемы Банк-Клиент
*/
#include <stdlib.h>
#include <errno.h>
#include "buhg_g.h"
#include <unistd.h>

int l_banks(int *nomb,char *imamah,char *katalog,char *imafil,char *parol,class iceb_u_str*,class iceb_u_str*,GtkWidget *wpredok);
int b_prominvest(const char *tabl,GtkWidget *wpredok);
int b_nadra(const char *tabl,GtkWidget *wpredok);
int b_aval(const char *tabl,GtkWidget *wpredok);
int b_ukrinbank(const char *tabl,GtkWidget *wpredok);
int b_privatinvestw(const char *tabl,GtkWidget *wpredok);
int b_brokbiznesw(const char *tabl,GtkWidget *wpredok);
int b_pervinvw(const char *tabl,GtkWidget *wpredok);
int b_ukrpromw(const char *tabl,GtkWidget *wpredok);
int b_ukreximw(const char *tabl,GtkWidget *wpredok);
int b_b2_bank_on_linew(const char *tabl,GtkWidget *wpredok);
int b_ukrsocw(const char *tabl,GtkWidget *wpredok);
int b_iBank_2_UAw(const char *tabl,GtkWidget *wpredok);
int b_privatw(const char *tabl,GtkWidget *wpredok);
int b_tinyw(const char *tabl,GtkWidget *wpredok);
int b_soft_revieww(const char *tabl,GtkWidget *wpredok);
int b_skarbw(const char *tabl,GtkWidget *wpredok);
int b_ukrbiznesw(const char *tabl,GtkWidget *wpredok);

extern SQL_baza bd;

void xdksend(int metka, //0-передача в подситему Банк-Клиент 1-запись на DOS дискету
const char *tablica,GtkWidget *wpredok)
{
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
char bros[512];
char strsql[512];
FILE *ff;
short d,m,g;

printf("%s-metka=%d tablica=%s\n",__FUNCTION__,metka,tablica);

sprintf(strsql,"select datd,nomd from %s where vidpl='1'",tablica);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного документа отмеченного для передчи в банк"),wpredok);
  return;
 }

/*Выгрузка в файл сделана чтобы не переписывать все подпрограммы которые формируют файл для конкретного банка*/
const char *imaf_tmp={"platp.txt"};

if((ff = fopen(imaf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tmp,"",errno,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
  fprintf(ff,"%d.%d.%d|%s\n",d,m,g,row[1]);
 }

fclose(ff);

//iceb_cp("platp.txt","plat.txt",0,wpredok);

int nomerb=0;
char imamah[512];
char katalog[512];
char imafil[32];
char parol[32];
class iceb_u_str comanda("");
class iceb_u_str script("");

if(l_banks(&nomerb,imamah,katalog,imafil,parol,&comanda,&script,wpredok) != 0)
  return;

if(script.getdlinna() <= 1)
 script.new_plus("bankcp");
 
/*
    printw("\nnomerb=%d imamah=%s katalog=%s imafil=%s\n",
    nomerb,imamah,katalog,imafil);
    OSTANOV();
*/
if(nomerb == 1) //Проминвест банк
  b_prominvest(tablica,wpredok);
if(nomerb == 2) //Банк Надра
  b_nadra(tablica,wpredok);
if(nomerb == 3) //Банк Аваль
  b_aval(tablica,wpredok);
if(nomerb == 4) //Укринбанк
  b_ukrinbank(tablica,wpredok);
if(nomerb == 5) //Приватинвест
  b_privatinvestw(tablica,wpredok);
if(nomerb == 6) //Брокбизнесбанк
  b_brokbiznesw(tablica,wpredok);
if(nomerb == 7) //Первый инвестиционный
  b_pervinvw(tablica,wpredok);

if(nomerb == 8) //Укрпромбанк
 {
  
  b_ukrpromw(tablica,wpredok);

  //Узнаём номер дня недели
  struct tm *bf;
  time_t vrem;
  time(&vrem);
  bf=localtime(&vrem);
  sprintf(bros,"%d",bf->tm_wday+1); //Нумерация дней недели начинаестся с ноля
  //Третий знак в имени файла номер дня недели
  imafil[2]=bros[0]; 

  if(iceb_menu_vvod1(gettext("Введите имя файла."),imafil,sizeof(imafil),wpredok) != 0)
     return;

 }


if(nomerb == 9) //УкрЕсим банк
  b_ukreximw(tablica,wpredok);
if(nomerb == 10) //Б2 bank on line
  b_b2_bank_on_linew(tablica,wpredok);
if(nomerb == 11) //Укрсоцбанк
  b_ukrsocw(tablica,wpredok);
if(nomerb == 12) //iBank 2 UA
  b_iBank_2_UAw(tablica,wpredok);

if(nomerb == 13) //Приват Банк
 {
  if(b_privatw(tablica,wpredok) == 0)
   {
    iceb_menu_soob("Платёжки записаны в таблицу JBKL_SND !",wpredok);
    unlink("plat.txt");
    unlink(imaf_tmp);
    /*Снимаем метку передачи в банк*/
    sprintf(strsql,"update %s set vidpl='' where vidpl='1'",tablica);
    iceb_sql_zapis(strsql,1,0,wpredok);
   }
  return;
 }

if(nomerb == 14) //финансы и кредит
  b_soft_revieww(tablica,wpredok);

if(nomerb == 15) //система клиент-банк "Tiny"
  b_tinyw(tablica,wpredok);

if(nomerb == 16) //система клиент-банк "СКАРБ"
  b_skarbw(tablica,wpredok);

if(nomerb == 17) //система клиент-банк для бана Укрбизнесбанк
  b_ukrbiznesw(tablica,wpredok);


if(metka == 0)
 {
  int i=0;
  if(comanda.getdlinna() <= 1)
   {
    sprintf(bros,"%s %s %s %s %s %s",script.ravno(),imamah,"plat.txt",imafil,katalog,parol);

    i=system(bros);
   }
  else 
    i=system(comanda.ravno());
  
  iceb_u_str repl;
  if(i != 0)
   {
    
    sprintf(bros,gettext("Передача файла не произошла ! Код ошибки %d"),i);

    repl.plus(bros);
    if(comanda.getdlinna() <= 1)
     {
      sprintf(bros,"%d %s %s %s %s %s %s",nomerb,imamah,"plat.txt",imafil,katalog,parol,script.ravno());
      repl.ps_plus(bros);
     }
    else
     repl.ps_plus(comanda.ravno());
    iceb_menu_soob(&repl,wpredok);
   }
  if(i == 0)
   {
    
    repl.plus(gettext("Передача файла произошла успешно."));
    if(comanda.getdlinna() <= 1)
      repl.ps_plus(imafil);
    
    iceb_menu_soob(&repl,wpredok);
    
    unlink("plat.txt");
//    unlink(imaf_tmp);
    /*Снимаем метку передачи в банк*/
    sprintf(strsql,"update %s set vidpl='' where vidpl='1'",tablica);
    iceb_sql_zapis(strsql,1,0,wpredok);
   }     

 }

if(metka == 1)
 {
  rename("plat.txt",imafil);
  
  if(iceb_mydoscopy(imafil,wpredok) == 0)
   {
    /*Снимаем метку передачи в банк*/
    sprintf(strsql,"update %s set vidpl='' where vidpl='1'",tablica);
    iceb_sql_zapis(strsql,1,0,wpredok);
//    unlink(imaf_tmp);
   }
  unlink(imafil);
 }

unlink(imaf_tmp);

}
