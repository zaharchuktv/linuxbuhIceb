/*$Id: i_start.c,v 1.44 2011-02-21 07:35:51 sasa Exp $*/
/*16.04.2010	10.05.2004	Белых А.И.	i_start.c
Стартовая программа для запуска всех подсистем
*/

#define DVERSIQ "31.01.2012"
#include        <stdlib.h>
#include        <sys/stat.h>
#include        <pwd.h>
#include        <unistd.h>
#include        <locale.h>
#include        "../headers/buhg_g.h"


void i_start_obd(int argc,char *argv[]);
void razrab(void);
void i_start_sb();

extern char		*imabaz;
extern SQL_baza	bd;
extern char		*organ;
extern char		*host;
extern char		*parol;
const char *name_system={NAME_SYSTEM};
const char *version={VERSION};
extern class iceb_u_str kodirovka_iceb;

int main(int argc,char **argv)
{
char strsql[512];
int		kom=0;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

if(setenv("PAROLICEB",parol,1) != 0)
  printf("\nОшибка установки переменной паpоля!\n");

iceb_nastsys();//читаем шрифты
iceb_perzap(1,NULL);
nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));
sql_closebaz(&bd);

//printf("Уровень вхождения циклов=%d\n",gtk_main_level());

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Главная бухгалтерская книга"));
menu_str.plus(gettext("Материальный учет"));
menu_str.plus(gettext("Платёжные документы"));
menu_str.plus(gettext("Зарaботная плата"));
menu_str.plus(gettext("Учёт основных средств"));
menu_str.plus(gettext("Учёт услуг"));
menu_str.plus(gettext("Учёт кассовых ордеров"));
menu_str.plus(gettext("Реестр счет-фактур"));
menu_str.plus(gettext("Учёт командировочных расходов"));
menu_str.plus(gettext("Учёт путевых листов"));
menu_str.plus(gettext("Учёт доверенностей"));
while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Бухгалтерский учёт"),fioop.ravno(),nom_op,&menu_str,"#0080C9");
  switch(kom)
   {
    case 0:
      i_start_sb();
      break;

    case 1:
      sprintf(strsql,"i_xbu -h %s -b %s",host,imabaz);
      system(strsql);
      break;

    case 2:
      sprintf(strsql,"i_xmu -h %s -b %s",host,imabaz);
      system(strsql);
      break;

    case 3:
      sprintf(strsql,"i_xdk -h %s -b %s",host,imabaz);
      system(strsql);
      break;

    case 4:
      sprintf(strsql,"i_xzp -h %s -b %s",host,imabaz);
      system(strsql);
      break;

    case 5:
      sprintf(strsql,"i_xuos -h %s -b %s",host,imabaz);
      system(strsql);
      break;

    case 6:
      sprintf(strsql,"i_xusl -h %s -b %s",host,imabaz);
      system(strsql);
      break;

    case 7:
      sprintf(strsql,"i_xkas -h %s -b %s",host,imabaz);
      system(strsql);
      break;

    case 8:
      sprintf(strsql,"i_xrnn -h %s -b %s",host,imabaz);
      system(strsql);
      break;


    case 9:
      sprintf(strsql,"i_xukr -h %s -b %s",host,imabaz);
      system(strsql);
      break;

    case 10:
      sprintf(strsql,"i_xupl -h %s -b %s",host,imabaz);
      system(strsql);
      break;

    case 11:
      sprintf(strsql,"i_xud -h %s -b %s",host,imabaz);
      system(strsql);
      break;

   }
 }


//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());
//kon:;

    
   
return( 0 );
}

/**********************/
/*Смена базы*/
/************************/
void i_start_sb()
{
struct  passwd  *ktor; /*Кто работает*/

ktor=getpwuid(getuid());

if(sql_openbaz(&bd,imabaz,host,ktor->pw_name,parol) != 0)
    iceb_eropbaz(imabaz,ktor->pw_uid,ktor->pw_name,0);

iceb_start_rf();/*читаем файл с командами и выполняем их*/

if(iceb_smenabaz(1,NULL) != 0)
 return;

sql_closebaz(&bd);

}
