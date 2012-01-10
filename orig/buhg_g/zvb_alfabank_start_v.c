/*$Id: zvb_alfabank_start_v.c,v 1.6 2011-02-21 07:36:00 sasa Exp $*/
/*16.10.2009	16.10.2009	Белых А.И.	zvb_alfabank_v.c
Ввод необходимых реквизитов для перечисления на картсчета
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
enum
 {
  E_NOMERB,
  E_NOMERPP,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class zvb_alfabank_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str nomerb;
  class iceb_u_str pornom;
    
  zvb_alfabank_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();    
   }

  void read_rek()
   {
    nomerb.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMERB]))));
    pornom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMERPP]))));
   }
  void clear()
   {
    nomerb.new_plus("");
    pornom.new_plus("");
   }
 };


gboolean   zvb_alfabank_v_key_press(GtkWidget *widget,GdkEventKey *event,class zvb_alfabank_v_data *data);
void  zvb_alfabank_v_knopka(GtkWidget *widget,class zvb_alfabank_v_data *data);
void    zvb_alfabank_v_vvod(GtkWidget *widget,class zvb_alfabank_v_data *data);
int zvb_alfabank_zap(class zvb_alfabank_v_data *data);

void  zvb_alfabank_v_e_knopka(GtkWidget *widget,class zvb_alfabank_v_data *data);


extern SQL_baza  bd;
extern char      *name_system;

int zvb_alfabank_v(class iceb_u_str *nomerb,class iceb_u_str *pornom,GtkWidget *wpredok)
{

class zvb_alfabank_v_data data;
char strsql[512];
iceb_u_str kikz;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",gettext("Перечисление на картсчета"),gettext("Альфабанк"));
label=gtk_label_new(iceb_u_toutf(strsql));

sprintf(strsql,"%s %s",name_system,gettext("Перечисление на картсчета"));
gtk_window_set_title(GTK_WINDOW(data.window),iceb_u_toutf(strsql));

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zvb_alfabank_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Номер заданный банком"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERB]), label, FALSE, FALSE, 0);


data.entry[E_NOMERB] = gtk_entry_new_with_max_length (5);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERB]), data.entry[E_NOMERB], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMERB]), "activate",GTK_SIGNAL_FUNC(zvb_alfabank_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMERB]),data.nomerb.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMERB]),(gpointer)E_NOMERB);

label=gtk_label_new(gettext("Порядковий номер в текущей дате"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERPP]), label, FALSE, FALSE, 0);

data.entry[E_NOMERPP] = gtk_entry_new_with_max_length (5);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMERPP]), data.entry[E_NOMERPP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMERPP]), "activate",GTK_SIGNAL_FUNC(zvb_alfabank_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMERPP]),data.pornom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMERPP]),(gpointer)E_NOMERPP);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(zvb_alfabank_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(zvb_alfabank_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
if(data.voz == 0)
 {
  nomerb->new_plus(data.nomerb.ravno());
  pornom->new_plus(data.pornom.ravno());  
 }

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/

void  zvb_alfabank_v_e_knopka(GtkWidget *widget,class zvb_alfabank_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zvb_alfabank_v_key_press(GtkWidget *widget,GdkEventKey *event,class zvb_alfabank_v_data *data)
{
//char  bros[512];

//printf("zvb_alfabank_v_key_press\n");
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
void  zvb_alfabank_v_knopka(GtkWidget *widget,class zvb_alfabank_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zvb_alfabank_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(zvb_alfabank_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void zvb_alfabank_v_vvod(GtkWidget *widget,class zvb_alfabank_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zvb_alfabank_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_NOMERB:
    data->nomerb.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NOMERPP:
    data->pornom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int zvb_alfabank_zap(class zvb_alfabank_v_data *data)
{


return(0);

}