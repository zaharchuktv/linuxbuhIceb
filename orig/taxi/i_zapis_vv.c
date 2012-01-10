/*$Id: i_zapis_vv.c,v 1.18 2011-02-21 07:36:20 sasa Exp $*/
/*29.09.2009	17.01.2006	Белых А.И.	i_zapis_vv.c
Запись входа/выхода в таблицу

Формат командной строки для запуска программы             
i_kontr_vv bx localhost parol 1 10 1234567890
           |      |        |  | |      |
           |      |        |  | |      -> номер карточки
           |      |        |  | -> номер турникета
           |      |        |  -> 1-если вход 2-если выход
           |      |        -> пароль для доступа к базе данных
           |      -> хост на базу
           -> имя базы
*/
#define         DVERSIQ "18.01.2007"
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include        <pwd.h>
#include        <locale.h>
#include        <sys/stat.h>
#include "../iceBw.h"
#include "i_zapis_vv.h"

void i_zapis_vihod(SQL_baza bd,int podr,char *kodkart);
void i_zapis_vhod(SQL_baza bd,int podr,char *kodkart);
int i_zapis_vv_z(SQL_baza bd,int podr,char *kodkart,int metka_vv);

extern char *putnansi;
extern char *imabaz;
extern char *host;
const char *name_system={NAME_SYSTEM};
const char *version={VERSION};
extern SQL_baza	bd;
uid_t kod_operatora;

int main(int argc,char **argv)
{
char		parol[100];
char strsql[500];
struct  passwd  *ktor; /*Кто работает*/
int nomer_tur=0;
int metka_vv=0; //1-вход 2-выход -1 отключить турникет
char nomer_kart[50];
memset(nomer_kart,'\0',sizeof(nomer_kart));
if(argc == 1 )
 goto vvvv;

if(strcmp("-v",argv[1]) == 0)
 {
  printf("Версия %s от %s\n",VERSION,DVERSIQ);
  return(1);
 }
if( strcmp("-h",argv[1]) == 0  )
 {
vvvv:;
  printf("Программа записи входа/выхода.\n");
  printf("Версия %s от %s\n",VERSION,DVERSIQ);
  printf("Формат командной строки:\n\
1 - имя базы\n\
2 - хост на базу\n\
3 - пароль для доступа к базе\n\
4 - 1 если вход\n\
    2 если выход\n\
   -1 eсли прочитали карточку для отключения турникета\n\
5 - номер турникета\n\
6 - номер карточки\n");
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
  sql_closebaz(&bd);
  printf("Не найден номер турникета %d в списке турникетов !\n",nomer_tur);
  return(1);
 }

int podr=atoi(row[0]);

if(metka_vv == 1)
   i_zapis_vhod(bd,podr,nomer_kart);

if(metka_vv == 2)
 if( i_zapis_vv_z(bd,podr,nomer_kart,metka_vv) == 0)
  printf("Записали выход.\n");

if(metka_vv == -1)
 if( i_zapis_vv_z(bd,podr,nomer_kart,metka_vv) == 0)
  printf("Записали отключение турникета.\n");

sql_closebaz(&bd);
    

return(0);
}
/******************/
/*Запись входа*/
/******************/
void i_zapis_vhod(SQL_baza bd,int podr,char *kodkart)
{
time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem); //Время должно определяться только вначале

if(i_zapis_vv_z(bd,podr,kodkart,1) == 0)
 printf("Записали вход.\n");
 
char imaf_nastr[312];
//sprintf(imaf_nastr,"%s%s%s%srestnast.alx",putnansi,G_DIR_SEPARATOR_S,imabaz,G_DIR_SEPARATOR_S);
sprintf(imaf_nastr,"restnast.alx");


char strsql[300];
memset(strsql,'\0',sizeof(strsql));

if(iceb_poldan("Код услуги оплаты за вход",strsql,imaf_nastr,NULL) != 0)
 {
  printf("Не нашли настройку \"Код услуги оплаты за вход\" !\n");
  return;
 }

int kodusl=atoi(strsql);
if(kodusl == 0)
 {
  printf("Не введен \"Код услуги оплаты за вход\" !\n");
  return;
 }

char grup_personal[500];
memset(grup_personal,'\0',sizeof(grup_personal));
if(iceb_poldan("Коды групп персонала предприятия",grup_personal,imaf_nastr,NULL) != 0)
 printf("Не нашли строку \"Коды групп персонала предприятия\" !\n");

SQL_str row;
SQLCURSOR cur;
float proc_sk=0.;
char dvk[30]; //Дата-время возврата карточки

char grupa_klienta[50];
memset(dvk,'\0',sizeof(dvk));

memset(grupa_klienta,'\0',sizeof(grupa_klienta));
int pol=0; //0-мужчина 1-женщина 
sprintf(strsql,"select ps,grup,dvk,pl from Taxiklient where kod='%s'",kodkart);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  printf("Не нашли код клиента %s в списке клиентов !\n",kodkart);
 }
else
 {
  proc_sk=atof(row[0]);
  strncpy(grupa_klienta,row[1],sizeof(grupa_klienta)-1);
  strncpy(dvk,row[2],sizeof(dvk)-1);
  pol=atoi(row[3]);
 }
 
if(iceb_u_proverka(grup_personal,grupa_klienta,0,1) == 0)
 {
  printf("Персонал предприятия. Счёт не выписываем.\n");
  return;
 } 


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
  if(cena == 0.)
   {
    printf("Цена за вход:%.2f\n",cena);
    return;
   }
  if(iceb_u_proverka(row[2],grupa_klienta,0,1) == 0) 
   {
    printf("Вход для группы %s бесплатный.\n",row[2]);
    return;
   }
 }
printf("Цена за вход:%.2f\n",cena);

if(cena < 0.01)
  return;
short d=0,m=0,g=0;

//Проверяем может счёт уже выписан от текущего времени минус 12 часов назад

sprintf(strsql,"select god from Restdok where kodkl='%s' and podr=%d and vremz >= %ld \
and vremz <= %ld and vremz > %ld limit 1",
kodkart,podr,vrem-(12*60*60),vrem,iceb_u_datetime_sec(dvk));

if(sql_readkey(&bd,strsql,&row,&cur) > 0)
 {
  printf("Счёт уже выписан.\n");
  return; 
 } 

printf("Выписываем счёт.\n");
//записываем шапку документа
iceb_u_poltekdat(&d,&m,&g);
iceb_u_str nomdok;

zap_rest_dok(d,m,g,&nomdok,"",kodkart,"","",0,podr,0,proc_sk);

zap_vrest_dok(d,m,g,nomdok.ravno(),podr,1,kodusl,"Пс",1.,cena,"",0,podr,0.);


}

/************************************/
/*Запись*/
/********************/
int i_zapis_vv_z(SQL_baza bd,int podr,char *kodkart,int metka_vv)
{
time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);

char strsql[300];
//делаем запись 
sprintf(strsql,"insert Restvv values('%d-%d-%d %02d:%02d:%02d','%s',%d,%d)",
bf->tm_year+1900,
bf->tm_mon+1,
bf->tm_mday,
bf->tm_hour,bf->tm_min,bf->tm_sec,
kodkart,
podr,metka_vv);

if(sql_zap(&bd,strsql) != 0)
 {
  t_msql_error(&bd,strsql);
  return(1);
 }
return(0);
}

