/*$Id: iceb_start.c,v 1.19 2011-02-21 07:36:08 sasa Exp $*/
/*17.02.2011	28.03.2005	Белых А.И.	iceb_start.c
Начало работы подсистем
*/
#include        <stdlib.h>
#include        <pwd.h>
#include        <unistd.h>
#include        <locale.h>
#include        <sys/stat.h>
#include        "iceb_libbuh.h"
#include "../knopki/linuxbuh_icon.xpm"

// название файла с переводом для нашего приложения
#define PACKAGE "linuxbuh.ru"

// каталог, где лежат файлы с переводами
#ifdef WIN32 // для Windows подразумевается, что приложение лежит в папке bin, а рядом в папке share/locale находятся переводы
#define PACKAGE_LOCALE_DIR "../share/locale"
#else
#define PACKAGE_LOCALE_DIR "/usr/share/locale"
#endif




SQL_baza	bd;
char *putnansi=NULL;
char *organ=NULL;
char *host=NULL;
char *imabaz=NULL;
char *parol=NULL;
class iceb_u_str kodirovka_iceb;

void iceb_start(int argc,char **argv)
{
gchar *str=NULL;
struct  passwd  *ktor; /*Кто работает*/
int regim;
class iceb_u_str locale;
ktor=getpwuid(getuid());
setlocale(LC_ALL,"");/*Устанавливаются переменные локали из окружающей среды*/


locale.plus(setlocale(LC_MESSAGES,""));

if(iceb_u_polen(locale.ravno(),&kodirovka_iceb,2,'.') != 0)
 {
  if(iceb_u_SRAV(locale.ravno(),"ru",0) == 0)
   kodirovka_iceb.new_plus("koi8r");
  if(iceb_u_SRAV(locale.ravno(),"ru_UA",0) == 0)
   kodirovka_iceb.new_plus("koi8u");
 }

// настройка папок с переводами;
str = bindtextdomain( PACKAGE,PACKAGE_LOCALE_DIR );//установить каталог, содержащий переводы
// кодировка перевода;
str = bind_textdomain_codeset (PACKAGE,kodirovka_iceb.ravno());// Устанавливаем кодировку получаемых сообщений*/
str = textdomain(PACKAGE);// устанавливаем файл из которого будут браться строки для переводов

umask(0117); /*Установка маски для записи и чтения группы*/

gtk_set_locale();
gtk_init( &argc, &argv );

//Делаем обязятельно после gtk_ini
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа


//printf("parol=%s imabaz=%s\n",parol,imabaz);

/*Определяем путь на файлы настройки*/
if((putnansi=getenv("PUTNANSI")) == NULL)
 {
  class iceb_u_str nastr_path;
  if( G_DIR_SEPARATOR == '\\') /*Windous*/
   {
    nastr_path.plus("c:");
    nastr_path.plus(G_DIR_SEPARATOR_S);
    nastr_path.plus("iceB");
   }
  else /*Linux*/
   {
    nastr_path.plus(G_DIR_SEPARATOR_S);
    nastr_path.plus("etc");
    nastr_path.plus(G_DIR_SEPARATOR_S);
    nastr_path.plus("iceB");
   }
  putnansi=new char[nastr_path.getdlinna()];
  strcpy(putnansi,nastr_path.ravno());

 }  
GdkPixbuf *iconca=gdk_pixbuf_new_from_xpm_data(linuxbuh_icon_xpm);  
gtk_window_set_default_icon(iconca);

/*Устанавливаем иконку для всех меню из корневого каталога настроечных файлов*/
/**********
int metka_icon=0;
if(gtk_window_set_default_icon_from_file(iceb_imafnsi("iceB_icon.png"),NULL) == TRUE)
 metka_icon=1;
************/
/*Читаем ключи запуска программы*/
iceb_readkey(argc,argv,&parol,&imabaz,&host,&regim);

//printf("imabaz=%s host=%s kto=%s parol=%s\n",imabaz,host,kto,parol);
if(sql_openbaz(&bd,imabaz,host,ktor->pw_name,parol) != 0)
    iceb_eropbaz(imabaz,ktor->pw_uid,ktor->pw_name,0);

iceb_start_rf();/*читаем файл с командами и выполняем их*/


//printf("База открыта. %.2f\n",10.33);
SQLCURSOR cur;
SQL_str row;
/*Читаем наименование организации*/
char bros[512];
sprintf(bros,"select naikon from Kontragent where kodkon='00'");
if(sql_readkey(&bd,bros,&row,&cur) != 1)
 {
  iceb_vkk00(NULL);
/***************
  iceb_u_str SP;
  SP.plus(gettext("Не найден код контрагента 00 !"));  
  iceb_menu_soob(&SP,NULL);
  int i=1+strlen(gettext("Не найден код контрагента 00 !"));
  organ=new char[i];
  strcpy(organ,gettext("Не найден код контрагента 00 !"));
****************/
 }
else
 {
  int i=strlen(row[0])+1;
  organ=new char[i];
  strcpy(organ,row[0]);
 }

/*Устанавливаем иконку для всех меню из каталога конкретной базы данных*/
//if(metka_icon == 0)
// gtk_window_set_default_icon_from_file(iceb_imafnsi("iceBw_icon.png"),NULL);

}
