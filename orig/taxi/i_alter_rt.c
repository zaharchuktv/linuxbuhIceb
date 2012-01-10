/*$Id: i_alter_rt.c,v 1.23 2011-02-21 07:36:20 sasa Exp $*/
/*05.06.2008	29.04.2004	Белых А.И.	i_alter_rt.c
Программа модификации таблиц в базе данных
*/
#define   DVERSION "05.06.2008"
#include        <locale.h>
#include        <sys/stat.h>
#include <iceb_libbuh.h>
#include "../iceBw.h"
#define   NAME_SYSTEM "i_alter_rt"
#define   CONFIG_PATH "/etc/iceB"

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
  KOLENTER  
 };

 
class  alrter_rt
 {
  public:
  iceb_u_str  imabaz;
  iceb_u_str  host;
  iceb_u_str  parol;
  
  GtkWidget *entry[KOLENTER];
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  
 };

void    alrter_rt_vvod(GtkWidget *widget,class alrter_rt *data);
void  alrter_rt_knopka(GtkWidget *widget,class alrter_rt *data);
gboolean   alrter_rt_key_press(GtkWidget *widget,GdkEventKey *event,class alrter_rt *data);
void    alrter_rt_st(class alrter_rt *data);

void i_alter_rtr(const char *imabaz,const char *host,const char *parol,GtkWidget *wpredok);


extern SQL_baza	bd;
const char *name_system={NAME_SYSTEM};
const char *version={VERSION};


int main(int argc,char **argv)
{
alrter_rt data;
char bros[300];

(void)setlocale(LC_ALL,"");
umask(0117); /*Установка маски для записи и чтения группы*/

gtk_set_locale();
gtk_init( &argc, &argv );
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
//gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(bros,"%s %s",name_system,"Преобразовать базу");

gtk_window_set_title (GTK_WINDOW (data.window),iceb_u_toutf(bros));
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(alrter_rt_key_press),&data);

sprintf(bros,"Преобразование таблиц базы данных\n\
для версии:%s от %s",VERSION,DVERSION);

GtkWidget *label=gtk_label_new(iceb_u_toutf(bros));


GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

label=gtk_label_new(iceb_u_toutf("Имя базы"));
data.entry[E_IMABAZ] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_IMABAZ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_IMABAZ]), "activate",GTK_SIGNAL_FUNC(alrter_rt_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMABAZ]),"");
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_IMABAZ]),(gpointer)E_IMABAZ);

label=gtk_label_new(iceb_u_toutf("Хост"));
data.entry[E_HOST] = gtk_entry_new_with_max_length (100);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_HOST], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HOST]), "activate",GTK_SIGNAL_FUNC(alrter_rt_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HOST]),"localhost");
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HOST]),(gpointer)E_HOST);

label=gtk_label_new(iceb_u_toutf("Пароль администратора базы данных"));
data.entry[E_PAROL] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_PAROL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PAROL]), "activate",GTK_SIGNAL_FUNC(alrter_rt_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PAROL]),"");
gtk_entry_set_visibility(GTK_ENTRY(data.entry[E_PAROL]),FALSE);
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PAROL]),(gpointer)E_PAROL);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",iceb_u_toutf("Преобразовать"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],iceb_u_toutf("Начать преобразование таблиц базы данных"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(alrter_rt_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);

sprintf(bros,"F10 %s",iceb_u_toutf("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],iceb_u_toutf("Завершение работы с программой"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(alrter_rt_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   alrter_rt_key_press(GtkWidget *widget,GdkEventKey *event,class alrter_rt *data)
{
//char  bros[300];

//printf("alrter_rt_key_press\n");
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
void  alrter_rt_knopka(GtkWidget *widget,class alrter_rt *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case FK2:
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");
    alrter_rt_st(data);
    return;  


  case FK10:
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    alrter_rt_vvod(GtkWidget *widget,class alrter_rt *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("alrter_rt_vvod enter=%d\n",enter);

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

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************/
/*Создание таблиц*/
/**************************/

void    alrter_rt_st(class alrter_rt *data)
{

i_alter_rtr(data->imabaz.ravno(),data->host.ravno(),data->parol.ravno(),data->window);

}
