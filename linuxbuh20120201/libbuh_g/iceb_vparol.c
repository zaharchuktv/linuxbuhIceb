/*$Id: iceb_vparol.c,v 1.13 2011-02-21 07:36:08 sasa Exp $*/
/*30.12.2003	18.08.2003	Белых А.И.	iceb_vparol.c
Программа для ввода пароля и имени базы данных
*/
#include <stdio.h>
#include <stdlib.h>
//#include <gdk/gdkkeysyms.h>
#include "iceb_libbuh.h"
#define  DLINNA 50
enum
 {
  E_PAROL,
  E_IMABAZ,
  KOLENTER
 };

void       iceb_vparol_vvod(GtkWidget *widget,class parol_imabaz *data);

extern char             *name_system;

class parol_imabaz
 {    
  public:
  
  GtkWidget *window;
  GtkWidget *button;
  iceb_u_str p1; //пароль
  iceb_u_str p2; //имя базы
  GtkWidget *entry[KOLENTER];

  //Конструктор

  parol_imabaz()
   {
    p1.plus("");
    p2.plus("");
   }
 };

/***************************/
gboolean parol_klav(GtkWidget *widget,GdkEventKey *event,struct parol_imabaz *data)
{
//printf("parol_klav\n");
if(event->keyval == GDK_F10)
 {
  data->p1.new_plus("aaa");
  data->p2.new_plus("bx");
  gtk_widget_destroy(widget);
  return(FALSE);
 }
return(TRUE);
}


/*******************************/

void vvodparol( GtkWidget *entry,
struct parol_imabaz *data)
{
for(int i=0; i < 2; i++)
  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

if(data->p1.ravno()[0] == '\0' || data->p2.ravno()[0] == '\0')
 {
  iceb_u_str SP;
  SP.plus(gettext("Не введены все обязательные реквизиты !"));
  iceb_menu_soob(&SP,NULL);
 }
else
  gtk_widget_destroy(data->window);

}

void iceb_vparol(char **parol,char **imabaz)
{

GtkWidget *vbox, *hbox, *hbox1;
GtkWidget *label,*label1;
parol_imabaz PP;
memset(&PP,'\0',sizeof(PP));

if(*parol != NULL)
 PP.p1.new_plus(*parol);
if(*imabaz != NULL)
 PP.p2.new_plus(*imabaz);

/* create a new window */
PP.window = gtk_window_new( GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(PP.window),GTK_WIN_POS_CENTER);

gtk_window_set_title(GTK_WINDOW (PP.window),name_system);
gtk_signal_connect(GTK_OBJECT (PP.window), "delete_event",(GtkSignalFunc) gtk_exit, NULL);
gtk_signal_connect(GTK_OBJECT(PP.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(PP.window),"key_press_event",GTK_SIGNAL_FUNC(parol_klav),&PP);
//gtk_container_set_border_width(GTK_CONTAINER(PP.window),10);
//gtk_window_set_default_size( GTK_WINDOW( PP.window ),
//                                500,   /* width  */
//                                200 ); /* height */

vbox = gtk_vbox_new (FALSE, 0);
//vbox = gtk_vbox_new (TRUE, 0);
gtk_container_add (GTK_CONTAINER (PP.window), vbox);
gtk_widget_show (vbox);

hbox = gtk_hbox_new (TRUE, 0);
//hbox = gtk_hbox_new (FALSE, 0);
gtk_container_add (GTK_CONTAINER (vbox), hbox);
gtk_widget_show (hbox);

label=gtk_label_new(iceb_u_toutf(gettext("Введите пароль для доступа к базе данных")));

gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
gtk_widget_show (label);

PP.entry[0] = gtk_entry_new_with_max_length (DLINNA);

//gtk_signal_connect(GTK_OBJECT(PP.entry[0]), "activate",GTK_SIGNAL_FUNC(enter_callback1),PP.p1);
gtk_signal_connect(GTK_OBJECT(PP.entry[0]), "activate",GTK_SIGNAL_FUNC(iceb_vparol_vvod),&PP);
gtk_object_set_user_data(GTK_OBJECT(PP.entry[E_PAROL]),(gpointer)E_PAROL);

if(*parol != NULL)
 gtk_entry_set_text (GTK_ENTRY (PP.entry[0]),*parol);

gtk_box_pack_start (GTK_BOX (hbox), PP.entry[0], TRUE, TRUE, 0);
gtk_entry_set_visibility(GTK_ENTRY(PP.entry[0]),FALSE);

//if(*parol == NULL)
// gtk_widget_grab_focus(PP.entry[0]);

gtk_widget_show (PP.entry[0]);

hbox1 = gtk_hbox_new (TRUE, 0);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_widget_show (hbox1);

label1=gtk_label_new(iceb_u_toutf(gettext("Введите имя базы данных")));
//label1=gtk_label_new("Baza");

gtk_box_pack_start (GTK_BOX (hbox1), label1, TRUE, TRUE, 0);
gtk_widget_show (label1);

PP.entry[1] = gtk_entry_new_with_max_length (DLINNA);
gtk_signal_connect(GTK_OBJECT(PP.entry[1]), "activate",GTK_SIGNAL_FUNC(iceb_vparol_vvod),&PP);
gtk_object_set_user_data(GTK_OBJECT(PP.entry[E_IMABAZ]),(gpointer)E_IMABAZ);

if(*imabaz != NULL)
 gtk_entry_set_text (GTK_ENTRY (PP.entry[1]), *imabaz);

gtk_box_pack_start (GTK_BOX (hbox1), PP.entry[1], TRUE, TRUE, 0);

//if(*parol != NULL && *imabaz == NULL)
// gtk_widget_grab_focus(PP.entry[1]);

gtk_widget_show (PP.entry[1]);

PP.button = gtk_button_new_with_label (iceb_u_toutf(gettext("Ввести")));

gtk_signal_connect(GTK_OBJECT (PP.button), "clicked",
      GTK_SIGNAL_FUNC(vvodparol),
      &PP);

gtk_box_pack_start (GTK_BOX (vbox), PP.button, TRUE, TRUE, 0);
GTK_WIDGET_SET_FLAGS (PP.button, GTK_CAN_DEFAULT);
gtk_widget_grab_default (PP.button);
gtk_widget_show (PP.button);

//gtk_widget_show(PP.window);
gtk_widget_show_all(PP.window);

gtk_main();

//printf("Введенный пароль=%s\n",PP.p1.ravno());
//printf("Имя базы=%s\n",PP.p2.ravno());


if((*imabaz) != NULL)
  delete(*imabaz);
if((*parol) != NULL)
  delete(*parol);

if(PP.p2.ravno()[0] != '\0')
 {
  (*imabaz)=new char[strlen(PP.p2.ravno())+1];
  strcpy(*imabaz,PP.p2.ravno());
 }
if(PP.p1.ravno()[0] != '\0') 
 {
  (*parol)=new char[strlen(PP.p1.ravno())+1];
  strcpy(*parol,PP.p1.ravno());
 }

}

/*********************************/
/*Чтение строки*/
/******************/

void    iceb_vparol_vvod(GtkWidget *widget,class parol_imabaz *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kontr_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_PAROL:
    data->p1.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_IMABAZ:
    data->p2.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }
enter++;
if(enter >= KOLENTER)
 {
  gtk_widget_grab_focus(data->button);
 }
else
  gtk_widget_grab_focus(data->entry[enter]);
}
