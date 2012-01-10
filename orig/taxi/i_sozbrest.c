/*$Id: i_sozbrest.c,v 1.34 2011-02-21 07:36:20 sasa Exp $*/
/*05.09.2008	19.02.2004	Белых А.И.	i_sozbrest.c
Прогорамма создания базы для ресторана
*/
#define         DVERSIQ "05.09.2008"
#include   <stdlib.h>
#include   <errno.h>
#include <time.h>
#include        <sys/stat.h>
#include        <locale.h>
#include        <unistd.h>
#include        "../iceBw.h"
#include "i_rest.h"
enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_IMABAZ,
  E_HOST,
  E_PAROL,
  E_PUTNANSI,
  E_KODIR_ZAP,
  KOLENTER  
 };

 
class  i_sozbrest
 {
  public:
  iceb_u_str  imabaz;
  iceb_u_str  host;
  iceb_u_str  parol;
  class iceb_u_str putnansi;
  class iceb_u_str kodir_zap;
      
  GtkWidget *entry[KOLENTER];
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];

  i_sozbrest()
   {
    imabaz.new_plus("");
    host.new_plus("localhost");
    parol.new_plus("");
    putnansi.new_plus("./bx");  
    kodir_zap.new_plus("utf8");
   }

 };

void    i_sozbrest_vvod(GtkWidget *widget,class i_sozbrest *data);
void  i_sozbrest_knopka(GtkWidget *widget,class i_sozbrest *data);
gboolean   i_sozbrest_key_press(GtkWidget *widget,GdkEventKey *event,class i_sozbrest *data);
void    i_sozbrest_st(class i_sozbrest *data);
void i_sozbrest_r(const char *imabaz,const char *host,const char *parol,const char *putnansi,const char *kodir_zap,GtkWidget *wpredok);

extern SQL_baza	bd;
const char *name_system={"i_sozbrest"};
const char *version={"VIRSION"};


int main(int argc,char **argv)
{
i_sozbrest data;
char bros[300];

(void)setlocale(LC_ALL,"");
umask(0117); /*Установка маски для записи и чтения группы*/

gtk_set_locale();
gtk_init( &argc, &argv );
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,"Создать базу");

gtk_window_set_title (GTK_WINDOW (data.window),iceb_u_toutf(bros));
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(i_sozbrest_key_press),&data);

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox4 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox5 = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

iceb_u_str repl;
repl.new_plus("Создание базы данных для программы выписки счетов.");
repl.ps_plus("Версия:");
repl.plus(VERSION);
repl.plus(" от ");
repl.plus(DVERSIQ);

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

label=gtk_label_new(iceb_u_toutf("Имя базы"));
data.entry[E_IMABAZ] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_IMABAZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_IMABAZ]), "activate",GTK_SIGNAL_FUNC(i_sozbrest_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMABAZ]),"");
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_IMABAZ]),(gpointer)E_IMABAZ);

label=gtk_label_new(iceb_u_toutf("Хост"));
data.entry[E_HOST] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_HOST], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HOST]), "activate",GTK_SIGNAL_FUNC(i_sozbrest_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HOST]),"localhost");
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HOST]),(gpointer)E_HOST);

label=gtk_label_new(iceb_u_toutf("Пароль адмистратора базы данных"));
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);

data.entry[E_PAROL] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_PAROL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PAROL]), "activate",GTK_SIGNAL_FUNC(i_sozbrest_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PAROL]),"");
gtk_entry_set_visibility(GTK_ENTRY(data.entry[E_PAROL]),FALSE);
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PAROL]),(gpointer)E_PAROL);

label=gtk_label_new(iceb_u_toutf("Путь на НСИ"));
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);

data.entry[E_PUTNANSI] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_PUTNANSI], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PUTNANSI]), "activate",GTK_SIGNAL_FUNC(i_sozbrest_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PUTNANSI]),data.putnansi.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PUTNANSI]),(gpointer)E_PUTNANSI);

label=gtk_label_new(iceb_u_toutf("Кодировка запросов к базе данных"));
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE, 0);

data.entry[E_KODIR_ZAP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox5), data.entry[E_KODIR_ZAP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODIR_ZAP]), "activate",GTK_SIGNAL_FUNC(i_sozbrest_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODIR_ZAP]),data.kodir_zap.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODIR_ZAP]),(gpointer)E_KODIR_ZAP);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",iceb_u_toutf("Создать базу"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],iceb_u_toutf("Начать созданиу базы данных."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(i_sozbrest_knopka),&data);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);

sprintf(bros,"F10 %s",iceb_u_toutf("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],iceb_u_toutf("Завершение работы с программой."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(i_sozbrest_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   i_sozbrest_key_press(GtkWidget *widget,GdkEventKey *event,class i_sozbrest *data)
{
//char  bros[300];

//printf("i_sozbrest_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");


    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  i_sozbrest_knopka(GtkWidget *widget,class i_sozbrest *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case FK2:
    i_sozbrest_st(data);
    return;  


  case FK10:
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    i_sozbrest_vvod(GtkWidget *widget,class i_sozbrest *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("i_sozbrest_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_IMABAZ:
    data->imabaz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_HOST:
    data->host.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PAROL:
    data->parol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PUTNANSI:
    data->putnansi.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODIR_ZAP:
    data->kodir_zap.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************/
/*Создание таблиц*/
/**************************/

void    i_sozbrest_st(class i_sozbrest *data)
{

for(int i=0; i < KOLENTER; i++)
  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");


/*Проверяем возможность открытия настроечного файла*/   
class iceb_u_str imafilprov(data->putnansi.ravno());
imafilprov.plus(G_DIR_SEPARATOR_S,"restnast.alx");

FILE *ff;
if((ff = fopen(imafilprov.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imafilprov.ravno(),gettext("Неправильно указан путь откуда брать настроечные файлы!"),errno,data->window);
  return;
 }
fclose(ff);

if(data->imabaz.getdlinna() <= 1)
 {
  iceb_u_str SP;
  SP.plus("Не введено имя базы данных !");
  iceb_menu_soob(&SP,NULL);
  return;
 }
/**********
if(data->parol.getdlinna() <= 1)
 {
  iceb_u_str SP;
  SP.plus("Не введен пароль !");
  iceb_menu_soob(&SP,NULL);
  return;
 }
***************/
if(data->host.getdlinna() <= 1)
 {
  iceb_u_str SP;
  SP.plus("Не введен хост !");
  iceb_menu_soob(&SP,NULL);
  return;
 }

i_sozbrest_r(data->imabaz.ravno(),data->host.ravno(),data->parol.ravno(),data->putnansi.ravno(),data->kodir_zap.ravno(),data->window);

}

