/*$Id: i_admin.c,v 1.20 2011-08-29 07:13:43 sasa Exp $*/
/*18.02.2011	04.02.2009	Белых А.И.	i_admin.c
программа администрирования баз данных
*/
#define DVERSIQ "31.01.2012"
#include        <stdlib.h>
#include        <sys/stat.h>
#include        <locale.h>
//zs
//#include  "buhg_g.h"
#include  "../headers/buhg_g.h"
//#include "../knopki/iceBw_icon.xpm"
#include "../../knopki/linuxbuh_icon.xpm"
//ze

enum
 {
  MAKEBAZ,
  DOSTUP,
  SPISLOGINBAZ,
  SPISBAZ,
  VIHOD,
  KOLKNOP
 };

class glmenu_data
 {
  public:
  

  GtkWidget *window;
  GtkWidget *knopka[KOLKNOP];
  short     nomkn;

  glmenu_data() // Конструктор
   {
    nomkn=-1;
   }
 };

int admin_l(class iceb_u_str *parol,class iceb_u_str *login,class iceb_u_str *host,class iceb_u_str *kzkb);


int admin_dostup();
void admin_user(GtkWidget *wpredok);
void admin_makebaz(const char *host,const char *parol);


extern SQL_baza bd;
const char *name_system={NAME_SYSTEM};
class iceb_u_str kzkb; /*кодировка запросов к базе*/
extern class iceb_u_str kodirovka_iceb; 
const char            *version={VERSION};
extern int iceb_kod_podsystem;

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  glmenu_knopka(GtkWidget *widget,class glmenu_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("glkni_v_knopka knop=%d\n",knop);
data->nomkn=knop;
gtk_widget_destroy(data->window);
data->window=NULL;

}



/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   glmenu_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data)
{

switch(event->keyval)
 {

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[VIHOD]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

//    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/********************************/
/*Главное меню*/
/********************************/

void	glmenu(int *kom,const char *login,const char *host)
{
iceb_kod_podsystem=ICEB_PS_ABD;
iceb_u_str NADPIS;
class glmenu_data data;
char       bros[1024];


data.window=gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,gettext("Администрирование баз данных"));
gtk_window_set_title(GTK_WINDOW(data.window),iceb_u_toutf(bros));

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(glmenu_key_press),&data);

//GtkWidget *hbox=gtk_hbox_new(FALSE,0);
//gtk_container_add(GTK_CONTAINER(data.window),hbox);

GtkWidget *vbox=gtk_vbox_new(FALSE,2);
gtk_container_add(GTK_CONTAINER(data.window),vbox);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
//gtk_widget_realize(data.window);

sprintf(bros,"%s\n%s %s %s %s\n%s:%s\n%s:%s",
gettext("Администрирование баз данных"),
gettext("Версия"),
VERSION,
gettext("от"),
DVERSIQ,
gettext("Логин оператора"),
login,
gettext("Хост на базу"),
host);

GtkWidget *label=gtk_label_new(iceb_u_toutf(bros));
gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);



data.knopka[MAKEBAZ]=gtk_button_new_with_label(gettext("Разметить новую базу данных для программы"));
gtk_signal_connect(GTK_OBJECT(data.knopka[MAKEBAZ]),"clicked",GTK_SIGNAL_FUNC(glmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[MAKEBAZ]),(gpointer)MAKEBAZ);
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[MAKEBAZ], TRUE, TRUE, 0);

data.knopka[DOSTUP]=gtk_button_new_with_label(gettext("Рaзрешить доступ к базе данных"));
gtk_signal_connect(GTK_OBJECT(data.knopka[DOSTUP]),"clicked",GTK_SIGNAL_FUNC(glmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[DOSTUP]),(gpointer)DOSTUP);
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[DOSTUP], TRUE, TRUE, 0);

data.knopka[SPISLOGINBAZ]=gtk_button_new_with_label(gettext("Просмотр списка разрешённых доступов к базам данных"));
gtk_signal_connect(GTK_OBJECT(data.knopka[SPISLOGINBAZ]),"clicked",GTK_SIGNAL_FUNC(glmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SPISLOGINBAZ]),(gpointer)SPISLOGINBAZ);
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[SPISLOGINBAZ], TRUE, TRUE, 0);

data.knopka[SPISBAZ]=gtk_button_new_with_label(gettext("Просмотр списка размеченных баз"));
gtk_signal_connect(GTK_OBJECT(data.knopka[SPISBAZ]),"clicked",GTK_SIGNAL_FUNC(glmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SPISBAZ]),(gpointer)SPISBAZ);
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[SPISBAZ], TRUE, TRUE, 0);

data.knopka[VIHOD]=gtk_button_new_with_label(gettext("Выход"));
gtk_signal_connect(GTK_OBJECT(data.knopka[VIHOD]),"clicked",GTK_SIGNAL_FUNC(glmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[VIHOD]),(gpointer)VIHOD);
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[VIHOD], TRUE, TRUE,0);


gtk_widget_grab_focus(data.knopka[*kom]);

gtk_widget_show_all (data.window);


gtk_main();

*kom=data.nomkn;

}
/*******************/
/*Главная программа*/
/*******************/
int main(int argc,char **argv)
{
char strsql[512];
int		kom=0;

if(iceb_u_strstrm(setlocale(LC_ALL,""),"utf") == 1)
 {
  kzkb.new_plus("utf8");
  kodirovka_iceb.new_plus("utf8");
 }  
umask(0117); /*Установка маски для записи и чтения группы*/

gtk_set_locale();
gtk_init( &argc, &argv );
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа

/*Устанавливаем иконку для всех меню*/
GdkPixbuf *iconca=gdk_pixbuf_new_from_xpm_data(linuxbuh_icon_xpm);  
gtk_window_set_default_icon(iconca);


class iceb_u_str parol("");
class iceb_u_str login("");
class iceb_u_str host("");


if(admin_l(&parol,&login,&host,&kzkb) != 0)
 return(0);

if(mysql_init(&bd) == NULL)
 {
  class iceb_u_str repl;
  repl.plus(gettext("Ошибка инициализации базы данных"));
  repl.plus((int)sql_nerror(&bd));
  repl.plus(" ",sql_error(&bd));
  
  iceb_menu_soob(&repl,NULL);
  return(1);  
 }

if(mysql_real_connect(&bd,host.ravno(),login.ravno(),parol.ravno(),"mysql",0,NULL,0) == NULL)
 { 
  class iceb_u_str repl(gettext("Ошибка соединения с базой данных"));
  repl.plus((int)sql_nerror(&bd));
  repl.plus(" ");
  repl.plus(sql_error(&bd));
  
  iceb_menu_soob(&repl,NULL);
  return(1);  

 } 
sprintf(strsql,"SET NAMES %s",kzkb.ravno());
sql_zap(&bd,strsql);

while( kom >= 0 && kom < VIHOD )   
 {
  glmenu(&kom,login.ravno(),host.ravno());

  class iceb_u_str imabaz("");
  class iceb_u_str kodirovka("");
  class iceb_u_str fromnsi("");
  class iceb_u_str tonsi("");
  switch(kom)
   {
    case MAKEBAZ:
      admin_makebaz(host.ravno(),parol.ravno());
/**************
      if(admin_makebaz_m(&imabaz,&kodirovka,&fromnsi,&tonsi,NULL) != 0)
       break;

      admin_makebaz_r(imabaz.ravno(),host.ravno(),parol.ravno(),kodirovka.ravno(),fromnsi.ravno(),tonsi.ravno(),NULL);
***************/       
      break;

    case DOSTUP:
      admin_dostup();
      break;

    case SPISLOGINBAZ:
      admin_user(NULL);
      break;

    case SPISBAZ:
      admin_bases(0,&imabaz,NULL);
      break;


    case VIHOD:
      break;

   }


 }    

sql_closebaz(&bd);

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

return( 0 );
}
