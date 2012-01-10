/*$Id: sel_hrift.c,v 1.12 2011-02-21 07:36:21 sasa Exp $*/
/*18.10.2003	18.10.2003	Белых А.И.	sel_hrift.c
Выбор шрифта с записью его в файл конфигурации
*/
#include <stdlib.h>
#include        <errno.h>
#include        <sys/stat.h>
#include        <locale.h>
#include         "taxi.h"

void  hrift_des(GtkWidget *,GtkWidget *);
void  hrift_ot(GtkWidget *,GtkWidget *);
const char *name_system={""};


int main( int   argc,char **argv)
{

umask(0117); /*Установка маски для записи и чтения группы*/
(void)setlocale(LC_ALL,"");
gtk_init( &argc, &argv );

//Устанавливаем переменную обязательно после инициализации GTK
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа

/*
GtkWidget *window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(window),ICEB_POS_CENTER);
*/

GtkWidget *hrift=gtk_font_selection_dialog_new(iceb_u_toutf("Выбор шрифта"));

gtk_signal_connect(GTK_OBJECT(hrift),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(hrift),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

//g_return_if_fail(GTK_IS_FONT_SELECTION_DIALOG(hrift));

gtk_signal_connect(GTK_OBJECT (GTK_FONT_SELECTION_DIALOG(hrift)->ok_button), "clicked",\
GTK_SIGNAL_FUNC(hrift_des),hrift);

gtk_signal_connect(GTK_OBJECT (GTK_FONT_SELECTION_DIALOG(hrift)->cancel_button), "clicked",\
GTK_SIGNAL_FUNC(hrift_ot),hrift);

gtk_widget_show(hrift);
//gtk_widget_show(GTK_FONT_SELECTION_DIALOG(hrift)->apply_button);
gtk_main();

}

/********************************/
/********************************/

void  hrift_des(GtkWidget *widget,GtkWidget *hrift)
{
printf("hrift\n");

gchar *name_font=gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(hrift));
printf("Имя шрифта=%s\n",name_font);
//if(gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(hrift),name_font) == TRUE)
// printf("Шрифт найден\n");

FILE *ff;
char imaf[40];
strcpy(imaf,".gtkrc-2.0");

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_u_str repl;
  char bros[312];
  
  sprintf(bros,"%s:%d !",gettext("Ошибка открытия файла"),errno);
  repl.plus_ps(bros);
  

  repl.plus(strerror(errno));

  iceb_menu_soob(&repl,NULL);
  return;
 }

/*************
fprintf(ff,"\
style \"gtk-default-koi8\"\n\
{\n\
 font_name = \"%s\"\n\
}\n\
class \"*\" style \"gtk-default-koi8\"\n",name_font);
*****************/
fprintf(ff,"\
style \"gtk-default\"\n\
{\n\
 font_name = \"%s\"\n\
}\n\
class \"*\" style \"gtk-default\"\n",name_font);

fclose(ff);

//gtk_widget_destroy(hrift);
//gtk_rc_parse (".gtkrc-2.0");
gtk_rc_reparse_all ();
}

/**************************/
/***************************/

void  hrift_ot(GtkWidget *widget,GtkWidget *hrift)
{
gtk_widget_destroy(hrift);
}
