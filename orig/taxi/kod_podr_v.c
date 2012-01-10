/*$Id: kod_podr_v.c,v 1.9 2011-02-21 07:36:20 sasa Exp $*/
/*09.01.2007	01.03.2004	Белых А.И.	kod_podr_v.c
Ввод кода подразделения для подсистемы выписки счетов
*/
#include <time.h>
#include <unistd.h>
#include "i_rest.h"
//#include "klient.h"

enum
{
 FK2,
// FK4,
 FK10,
 KOL_F_KL
};

enum
{
 E_PODR,
 KOLENTER
};


class kod_podr_v_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *opt;
  short     kl_shift; //0-отжата 1-нажата  

  int       voz;
  iceb_u_str podr;
       
  kod_podr_v_data()
   {
    voz=0;
    kl_shift=0;
   }
  void read_rek()
   {
    podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PODR]))));
   }
  
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };


gboolean   kod_podr_v_key_press(GtkWidget *widget,GdkEventKey *event,class kod_podr_v_data *data);
void  kod_podr_v_knopka(GtkWidget *widget,class kod_podr_v_data *data);
void    kod_podr_v_vvod(GtkWidget *widget,class kod_podr_v_data *data);
int l_vrint_zap(class kod_podr_v_data *data);
void l_vrint_kodgr(class kod_podr_v_data *data);
void  kod_podr_v_e_knopka(GtkWidget *widget,class kod_podr_v_data *data);

extern SQL_baza	bd;
extern char *name_system;


int kod_podr_v(class iceb_u_str *podr,GtkWidget *wpredok)
{
char strsql[300];
class kod_podr_v_data data;
data.podr.new_plus(podr->ravno());
    
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kod_podr_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label;
sprintf(strsql,"%s %s",name_system,gettext("Выбор подразделения."));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

label=gtk_label_new(gettext("Введите код подразделения."));


GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER+1];
for(int i=0 ; i < KOLENTER+1; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0 ; i < KOLENTER+1; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

data.knopka_enter[E_PODR]=gtk_button_new_with_label(gettext("Код подразделения"));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(kod_podr_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
GtkTooltips *tooltips_k=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_k,data.knopka_enter[E_PODR],gettext("Выбор подразделения."),NULL);


data.entry[E_PODR] = gtk_entry_new_with_max_length (11);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(kod_podr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),"");
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.podr.ravno_toutf());



GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(kod_podr_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(kod_podr_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.voz == 0)
 podr->new_plus(data.podr.ravno());

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  kod_podr_v_e_knopka(GtkWidget *widget,class kod_podr_v_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_PODR:

//    gtk_signal_emit_by_name(GTK_OBJECT(data->entry[E_NOMST]),"activate");

    if(rpod_l(1,&kod,&naim,data->window) == 0)
     {
      data->podr.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->podr.ravno_toutf());
     }
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kod_podr_v_key_press(GtkWidget *widget,GdkEventKey *event,class kod_podr_v_data *data)
{

//printf("kod_podr_v_key_press\n");
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

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kod_podr_v_knopka(GtkWidget *widget,class kod_podr_v_data *data)
{
char strsql[300];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek();
    //Проверяем код подразделения
    sprintf(strsql,"select kod from Restpod where kod=%d",data->podr.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),data->podr.ravno_atoi());
      iceb_menu_soob(strsql,data->window);
      return;
     }
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    kod_podr_v_vvod(GtkWidget *widget,class kod_podr_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kod_podr_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_PODR:
    data->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    break;
 }
/********
enter+=1;
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
***********/ 
}
