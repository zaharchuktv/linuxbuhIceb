/*$Id: admin_l.c,v 1.11 2011-02-21 07:35:50 sasa Exp $*/
/*23.12.2010	05.02.2009	Белых А.И.	admin_l.c
Ввод логина и пароля для доступа к базе
*/
//zs
//#include  "buhg_g.h"
#include  "../headers/buhg_g.h"
//ze
enum
 {
  E_LOGIN,
  E_PAROL,
  E_HOST,
  E_KZKB,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK10,
  KOL_PFK
 };

class admin_l_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str parol;
  class iceb_u_str login;  
  class iceb_u_str host;  
  class iceb_u_str kzkb;
  
  admin_l_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    parol.plus("");
    login.plus("");
    host.plus("localhost");
//zs
    //kzkb.plus("koi8u");
    kzkb.plus("utf8");
//ze
  
   }

  void read_rek()
   {
    parol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PAROL]))));
    login.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_LOGIN]))));
    host.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_HOST]))));
    kzkb.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KZKB]))));
    
   }
 };

gboolean   admin_l_key_press(GtkWidget *widget,GdkEventKey *event,class admin_l_data *data);
void  admin_l_knopka(GtkWidget *widget,class admin_l_data *data);
void    admin_l_vvod(GtkWidget *widget,class admin_l_data *data);

extern SQL_baza  bd;

int admin_l(class iceb_u_str *parol,class iceb_u_str *login,class iceb_u_str *host,class iceb_u_str *kzkb)
{
class admin_l_data data;
char strsql[512];
if(kzkb->getdlinna() > 1)
 data.kzkb.new_plus(kzkb->ravno());

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s",NAME_SYSTEM);

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(admin_l_key_press),&data);


GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
//gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


label=gtk_label_new(gettext("Введите логин"));
data.entry[E_LOGIN] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_LOGIN]), data.entry[E_LOGIN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_LOGIN]), "activate",GTK_SIGNAL_FUNC(admin_l_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_LOGIN]),data.login.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_LOGIN]),(gpointer)E_LOGIN);

label=gtk_label_new(gettext("Введите пароль для доступа к базе данных"));
data.entry[E_PAROL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), data.entry[E_PAROL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PAROL]), "activate",GTK_SIGNAL_FUNC(admin_l_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PAROL]),data.parol.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PAROL]),(gpointer)E_PAROL);
gtk_entry_set_visibility(GTK_ENTRY(data.entry[E_PAROL]),FALSE); //невидимый ввод

label=gtk_label_new(gettext("Хост на базу"));
data.entry[E_HOST] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HOST]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_HOST]), data.entry[E_HOST], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HOST]), "activate",GTK_SIGNAL_FUNC(admin_l_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HOST]),data.host.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HOST]),(gpointer)E_HOST);

label=gtk_label_new(gettext("Кодировка запросов к базе"));
data.entry[E_KZKB] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KZKB]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KZKB]), data.entry[E_KZKB], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KZKB]), "activate",GTK_SIGNAL_FUNC(admin_l_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KZKB]),data.kzkb.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KZKB]),(gpointer)E_KZKB);


GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Продолжить работу с программой"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(admin_l_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Завершить работу с программой"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(admin_l_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  parol->new_plus(data.parol.ravno());
  login->new_plus(data.login.ravno());
  host->new_plus(data.host.ravno());
  kzkb->new_plus(data.kzkb.ravno());

  
 }
return(data.voz);

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_l_key_press(GtkWidget *widget,GdkEventKey *event,class admin_l_data *data)
{
//char  bros[512];

//printf("admin_l_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK2]),"clicked");

    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK10]),"clicked");

    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_l_knopka(GtkWidget *widget,class admin_l_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("admin_l_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    admin_l_vvod(GtkWidget *widget,class admin_l_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("admin_l_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_LOGIN:
    data->login.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PAROL:
    data->parol.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_HOST:
    data->host.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
