/*$Id: i_kontr_vv.c,v 1.33 2011-02-21 07:36:20 sasa Exp $*/
/*24.02.2009	15.12.2005	Белых А.И.	i_kontr_vv.c
Программа проверяет можно ли зайти или выйти клиенту в(из) подразделение(я)
Если вернули 0-можно входить-выходить
             1-нельзя
             2-прочитана карточка для выключения турникета
             
Формат командной строки для запуска программы             
i_kontr_vv bx localhost parol 1 10 1234567890
           |      |        |  | |      |
           |      |        |  | |      -> номер карточки
           |      |        |  | -> номер турникета
           |      |        |  -> 1-если проверка на вход 2-если проверка на выход
           |      |        -> пароль для доступа к базе данных
           |      -> хост на базу
           -> имя базы
*/
#define         DVERSIQ "09.10.2007"
//#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include        <pwd.h>
#include        <locale.h>
#include        <sys/stat.h>
#include        <string.h>
#include "i_kontr_vv.h"
//#include "../iceBw.h"
//#include "../libbuh_g/iceb_libbuh.h"
//#include "taxi.h"
//#include <iceb_libbuh.h>
int kontr_vv_prov_vhod(SQL_baza bd,int podr,char *kodkart);
int kontr_vv_prov_vihod(SQL_baza bd,int podr,char *kodkart);

extern char *putnansi;
extern char *imabaz;
extern char *host;
const char *name_system={NAME_SYSTEM};
const char *version={VERSION};
extern SQL_baza	bd;
short start_god_rest=0;
uid_t kod_operatora;
int  nomer_kas=0;  //номер кассы для списания безналичных денег по закрытым документам

int main(int argc,char **argv)
{
char		parol[112];
char strsql[512];
struct  passwd  *ktor; /*Кто работает*/
int nomer_tur=0;
int metka_vv=0; //1-вход 2-выход
char nomer_kart[50];
memset(nomer_kart,'\0',sizeof(nomer_kart));

if(argc == 1)
  goto vvvv;

if(strcmp("-v",argv[1]) == 0)
 {
  printf("Версия %s от %s\n",VERSION,DVERSIQ);
  return(1);
 }

if(strcmp("-h",argv[1]) == 0)
 {
vvvv:;
  printf("Программа проверки возможности входа/выхода\n");
  printf("Версия %s от %s\n",VERSION,DVERSIQ);
  printf("Формат командной строки:\n\
1 - имя базы\n\
2 - хост на базу\n\
3 - пароль для доступа к базе\n\
4 - 1 если вход\n\
    2 если выход\n\
5 - номер турникета\n\
6 - номер карточки\n\n\
Если вернули 0 - можно входить/выходить\n\
             1 - нельзя\n\
             2 - прочитана карточка для отключения турникета\n");
  return(1);
 }

for(int i=0; i < argc; i++)
 printf("%s ",argv[i]);
printf("\n");

if(argc != 7)
 {
  printf("i_kontr_vv-Количество аргументов в строке %d. А должно быть 6.\n",argc-1);
  return(1);
 }

//Определяем путь на файлы настройки
if((putnansi=getenv("PUTNANSI")) == NULL)
 {
  putnansi=new char[strlen(CONFIG_PATH)+1];
  strcpy(putnansi,CONFIG_PATH);
 }  



imabaz=new char[strlen(argv[1])+1];
strcpy(imabaz,argv[1]);

host=new char[strlen(argv[2])+1];
strcpy(host,argv[2]);

strncpy(parol,argv[3],sizeof(parol));
metka_vv=atoi(argv[4]);
nomer_tur=atoi(argv[5]);
strncpy(nomer_kart,argv[6],sizeof(nomer_kart));

kod_operatora=getuid();
ktor=getpwuid(kod_operatora);
    
umask(0117); /*Установка маски для записи и чтения группы*/


(void)setlocale(LC_ALL,"");
//gtk_init( &argc, &argv );

//Устанавливаем переменную обязательно после инициализации GTK
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа


//printf("imabaz=%s host=%s kto=%s parol=%s\n",imabaz,host,kto,parol);
if(sql_openbaz(&bd,imabaz,host,ktor->pw_name,parol) != 0)
 {
//    iceb_eropbaz(imabaz,ktor->pw_uid,ktor->pw_name,0);
  printf("Ошибка открытия базы данных\n%d %s\n",sql_nerror(&bd),sql_error(&bd));
  return(1); 
 }
i_vv_start(putnansi,imabaz);

SQL_str row;
SQLCURSOR cur;

//Читаем на входе в какое подразделение стоит турникет
sprintf(strsql,"select podr from Resst where nm=%d",nomer_tur);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  printf("Не найден номер турникета %d в списке турникетов !\n",nomer_tur);
  sql_closebaz(&bd);
  return(1); 
 }

int podr=atoi(row[0]);

int voz=0;

if(metka_vv == 1) //проверка входа
  voz=kontr_vv_prov_vhod(bd,podr,nomer_kart);
if(metka_vv == 2) //проверка входа
  voz=kontr_vv_prov_vihod(bd,podr,nomer_kart);

sql_closebaz(&bd);
    

if(voz == 0)
 {
  if(metka_vv == 1)
    printf("i_kontr_vv вход разрешён\n");
  if(metka_vv == 2)
    printf("i_kontr_vv выход разрешён\n");
 }
else 
 {
  if(metka_vv == 1)
    printf("i_kontr_vv вход неразрешён\n");
  if(metka_vv == 2)
    printf("i_kontr_vv выход неразрешён\n");
 }
return(voz);
}

/****************************/
/*Проверка возможности входа*/
/****************************/

int kontr_vv_prov_vhod(SQL_baza bd,int podr,char *kodkart)
{
time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem); //Время должно определяться только вначале
char strsql[300];
SQL_str row;
SQLCURSOR cur;
int kodoh=0;
char dvk[40]; //Дата-время возврата карточки
memset(dvk,'\0',sizeof(dvk));
int pol=0; //0-мужчина 1-женщина

sprintf(strsql,"select sp,mk,grup,dvk,pl,mb from Taxiklient where kod='%s'",kodkart);
if((kodoh=sql_readkey(&bd,strsql,&row,&cur)) != 1)
 {
  if(kodoh < 0)
   printf("Код ошибки:%d %s\n",sql_nerror(&bd),sql_error(&bd));
  else
   printf("Нет кода клиента %s в списке клиентов !",kodkart);
  return(1);
 }
if(atoi(row[5]) == 1)
 {
  printf("Карточка %s заблокирована!\n",kodkart);
  return(1);
 }
strncpy(dvk,row[3],sizeof(dvk)-1);
pol=atoi(row[4]);

char grupa_klienta[50];
memset(grupa_klienta,'\0',sizeof(grupa_klienta));
strncpy(grupa_klienta,row[2],sizeof(grupa_klienta)-1);

if(row[0][0] == '*')
 {
  printf("Карточка для выключения турникета !!!\n");
  return(2);
 }

sprintf(strsql,"%d",podr);
if(iceb_u_proverka(row[0],strsql,0,1) == 0)
 {
  printf("Код карточки:%s Запрещён вход в подразделение %d !\n",kodkart,podr);
  return(1);
 }

if(atoi(row[1]) == 0)
 {
  int voz_zap=0;
  //контроль входа/выхода
  sprintf(strsql,"select tp from Restvv where kk='%s' and kp=%d  and dv > '%s' order by dv desc limit 1",kodkart,podr,dvk);
//  printf("strsql=%s\n",strsql);
  if((voz_zap=sql_readkey(&bd,strsql,&row,&cur)) < 0)
   {
    printf("strsql=%s\n",strsql);
    printf("Код ошибки:%d %s\n",sql_nerror(&bd),sql_error(&bd));
    return(1);
   }
  
  if(voz_zap > 0)
   if(atoi(row[0]) == 1)
    {
     printf("Вход уже был !!!!\n");
     return(1);
    }
 }

char imaf_nastr[312];
//sprintf(imaf_nastr,"%s%s%s%srestnast.alx",putnansi,G_DIR_SEPARATOR_S,imabaz,G_DIR_SEPARATOR_S);
sprintf(imaf_nastr,"restnast.alx");

char k_podr[1024];
memset(k_podr,'\0',sizeof(k_podr));


iceb_poldan("Коды подразделений для контроля сальдо при входе",k_podr,imaf_nastr,NULL);
sprintf(strsql,"%d",podr);
if(iceb_u_proverka(k_podr,strsql,0,1) != 0)
 {
  printf("Не нашли настройки \"Коды подразделений для контроля сальдо при входе\" !\n");
  return(0);
 }

if(k_podr[0] == '\0')
 {
  printf("Не введено ни одного подразделения для контроля сальдо при входе.\n");
  return(0);
 }

char grup_personal[512];
memset(grup_personal,'\0',sizeof(grup_personal));
if(iceb_poldan("Коды групп персонала предприятия",grup_personal,imaf_nastr,NULL) != 0)
 printf("Не нашли строку \"Коды групп персонала предприятия\" !\n");

if(iceb_u_proverka(grup_personal,grupa_klienta,0,1) == 0)
 {
  printf("Персонал предприятия. Счёт не выписываем.\n");
  return(0);
 } 


//Проверяем может счёт уже выписан от текущего времени минус 12 часов назад

sprintf(strsql,"select god from Restdok where kodkl='%s' and podr=%d and vremz >= %ld \
and vremz <= %ld and vremz > %ld limit 1",
kodkart,podr,vrem-(12*60*60),vrem,iceb_u_datetime_sec(dvk));

if(sql_readkey(&bd,strsql,&row,&cur) > 0)
 {
  printf("Счёт уже выписан\n");
  return(0); 
 } 



//Проверяем хватит ли денег для оплаты входа

sprintf(strsql,"select dv,cena,grup,cdg from Restvi where dv <= '%d-%d-%d %02d:%02d:%02d' \
and kp=%d order by dv desc limit 1",
bf->tm_year+1900,
bf->tm_mon+1,
bf->tm_mday,
bf->tm_hour,bf->tm_min,bf->tm_sec,
podr);

//printf("%s\n",strsql);

double cena=0.;
int kolzap=0;
if((kolzap=sql_readkey(&bd,strsql,&row,&cur)) > 0)
 {
  if(pol == 0)
   {
    printf("Пол:Мужчина\n");
    cena=atof(row[1]);  
   }
  else
   {
    printf("Пол:Женщина\n");
    cena=atof(row[3]);  
   }   
  if(cena <= 0.009)
    {
     printf("Цена за вход равна 0. Можно входить.\n");
     return(0);
    }
  if(iceb_u_proverka(row[2],grupa_klienta,0,1) == 0) 
   {
    printf("Вход для группы %s бесплатный.\n",row[2]);
    return(0);
   }
 }

printf("Цена за вход:%.2f\n",cena);

double saldo_kon=0.;
if(cena > 0.009)
 {
  iceb_poldan("Стартовый год",strsql,imaf_nastr,NULL);
  start_god_rest=atoi(strsql);

  //запуск проверки сальдо по клиенту

  double saldo_pered_1neopl=0.;
  saldo_kon=ras_sal_kl_t(kodkart,&saldo_pered_1neopl);

  if(saldo_kon < 0.01)
   {
    printf("Стартовый год:%d Код карточки:%s.\nНет денег для оплаты входа:%.2f\n",start_god_rest,kodkart,saldo_kon);
    return(1);
   }
 }
 
if(saldo_kon < cena)
 {
  printf("Сальдо меньше цены за вход %.2f < %.2f\n",saldo_kon,cena);
  return(1);
 }
 
return(0);
}
/************************/
/*Проверка возможности выхода*/
/*****************************/

int kontr_vv_prov_vihod(SQL_baza bd,int podr,char *kodkart)
{
SQL_str row;
SQLCURSOR cur;
char dvk[40]; //Дата-время возврата карточки
memset(dvk,'\0',sizeof(dvk));

char strsql[300];
int kodoh=0;
sprintf(strsql,"select sp,mk,dvk,mb from Taxiklient where kod='%s'",kodkart);
if((kodoh=sql_readkey(&bd,strsql,&row,&cur)) != 1)
 {
  if(kodoh < 0)
   printf("Код ошибки:%d %s\n",sql_nerror(&bd),sql_error(&bd));
  else
   printf("Нет кода клиента %s в списке клиентов !",kodkart);
  return(1);
 }

if(atoi(row[3]) == 1)
 {
  printf("Карточка %s заблокирована!\n",kodkart);
  return(1);
 }

strncpy(dvk,row[2],sizeof(dvk)-1);
if(row[0][0] == '*')
 {
  printf("Карточка для выключения турникета !!!\n");
  return(2);
 }

if(atoi(row[1]) == 0)
 {
  int voz_zap=0;
  //контроль входа/выхода

  sprintf(strsql,"select tp from Restvv where kk='%s' and kp=%d  and dv > '%s' order by dv desc limit 1",kodkart,podr,dvk);
  if((voz_zap=sql_readkey(&bd,strsql,&row,&cur)) < 0)
   {
    printf("strsql=%s\n",strsql);
    printf("Код ошибки:%d %s\n",sql_nerror(&bd),sql_error(&bd));
    return(1);
   }
  if(voz_zap > 0)
  if(atoi(row[0]) == 2)
   {
    printf("Выход уже был !!!!\n");
    return(1);
   }
  if(voz_zap == 0)
   {
    printf("После даты выдачи карточки %s небыло входа в подразделение !\nВыход невозможен !\n",dvk);
    return(1);
   }

 }


char imaf_nastr[312];
//sprintf(imaf_nastr,"%s%s%s%srestnast.alx",putnansi,G_DIR_SEPARATOR_S,imabaz,G_DIR_SEPARATOR_S);
sprintf(imaf_nastr,"restnast.alx");

char k_podr[1024];
memset(k_podr,'\0',sizeof(k_podr));

iceb_poldan("Коды подразделений для контроля выхода",k_podr,imaf_nastr,NULL);
sprintf(strsql,"%d",podr);
if(iceb_u_proverka(k_podr,strsql,0,1) != 0)
 return(0);

iceb_poldan("Стартовый год",strsql,imaf_nastr,NULL);
start_god_rest=atoi(strsql);

//запуск проверки сальдо по клиенту

double saldo_pered_1neopl=0.;
double saldo_kon=ras_sal_kl_t(kodkart,&saldo_pered_1neopl);
if(saldo_kon < -0.009)
 {
  printf("Стартовый год:%d Код карточки:%s. Отрицательное сальдо:%.2f\n",start_god_rest,kodkart,saldo_kon);
  return(1);
 }
else
  printf("Сальдо:%.2f\n",saldo_kon);
return(0);
}
