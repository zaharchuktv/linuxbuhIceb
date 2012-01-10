/*$Id: uosgetinn.c,v 1.8 2011-02-21 07:35:58 sasa Exp $*/
/*31.10.2007	31.10.2007	Белых А.И.	uosgetinn.c
Меню для ввода нового инвентарного номера
Если вернули 0-ввели информацию
             1-нет
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "iceb_libbuh.h"
enum
 {
  E_INN,
  KOLENTER  
 };

enum
 {
  FK2,
  FK5,
  FK10,
  KOL_FK
 };

class uosgetinn_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str innom;
  short metka_v;        
  uosgetinn_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    innom.plus("");
    metka_v=0;
   }

  void read_rek()
   {
    innom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_INN]))));
   }
  
 };


gboolean   uosgetinn_key_press(GtkWidget *widget,GdkEventKey *event,class uosgetinn_data *data);
void  uosgetinn_knopka(GtkWidget *widget,class uosgetinn_data *data);
void    uosgetinn_vvod(GtkWidget *widget,class uosgetinn_data *data);


void  uosgetinn_e_knopka(GtkWidget *widget,class uosgetinn_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int uosgetinn(class iceb_u_str *innom,
int metka_v, //0-новый инвентарный номер 1-старый инвентарный номер
GtkWidget *wpredok)
{
class uosgetinn_data data;
data.metka_v=metka_v;

char strsql[512];
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",name_system,gettext("Ввод инвентарного номера"));
label=gtk_label_new(gettext("Введите инвентарный номер"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosgetinn_key_press),&data);

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
 hbox[i] = gtk_hbox_new (FALSE, 2);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 2);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 2);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 2);



gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 2);

GtkTooltips *tooltips_enter[KOLENTER];


if(metka_v == 0)
 {
  sprintf(strsql,"%s",gettext("Инвентарный номер"));
  data.knopka_enter[E_INN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.knopka_enter[E_INN], FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_INN]),"clicked",GTK_SIGNAL_FUNC(uosgetinn_e_knopka),&data);
  gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_INN]),(gpointer)E_INN);
  tooltips_enter[E_INN]=gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips_enter[E_INN],data.knopka_enter[E_INN],gettext("Получение свободного инвентарного номера"),NULL);
 }
else
 {
  sprintf(strsql,"%s",gettext("Инвентарный номер"));
  label=gtk_label_new(iceb_u_toutf(strsql));
  gtk_box_pack_start (GTK_BOX (hbox[E_INN]), label, FALSE, FALSE, 0);
 }

data.entry[E_INN] = gtk_entry_new_with_max_length (15);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.entry[E_INN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_INN]), "activate",GTK_SIGNAL_FUNC(uosgetinn_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INN]),data.innom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_INN]),(gpointer)E_INN);




GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввод информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(uosgetinn_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(uosgetinn_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 innom->new_plus(data.innom.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uosgetinn_e_knopka(GtkWidget *widget,class uosgetinn_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_INN:
    data->innom.new_plus(iceb_invnomer(data->window));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_INN]),data->innom.ravno_toutf());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosgetinn_key_press(GtkWidget *widget,GdkEventKey *event,class uosgetinn_data *data)
{
//char  bros[512];

//printf("uosgetinn_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

    return(TRUE);

  case GDK_F5:
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
void  uosgetinn_knopka(GtkWidget *widget,class uosgetinn_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("uosgetinn_knopka knop=%d\n",knop);
char strsql[512];

switch (knop)
 {
  case FK2:

    data->read_rek();
    if(data->innom.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введён инвентарный номер!"),data->window);
      return;
     }
    
    if(data->innom.ravno_atoi() == 0)  /*Значит символ */
     {
      iceb_menu_soob(gettext("Инвентарный номер должен быть цифровым"),data->window);
      return;
     
     }
    if(data->metka_v == 0) /*Ввод нового инвентаного номера */
     {
      
      sprintf(strsql,"select innom from Uosin where innom=%d",data->innom.ravno_atoi());
      if(iceb_sql_readkey(strsql,data->window) >= 1)
       {
        iceb_menu_soob(gettext("Такой инвентарный номер уже есть!"),data->window);
        return;
       }      
     }
    else /*Ввод инвентарного номера который уже должен быть*/
     {
      sprintf(strsql,"select innom from Uosin where innom=%d",data->innom.ravno_atoi());
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        iceb_menu_soob(gettext("Нет такого инвентарного номера!"),data->window);
        return;
       }      
     }
    data->voz=0;
    gtk_widget_destroy(data->window);
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

void    uosgetinn_vvod(GtkWidget *widget,class uosgetinn_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("uosgetinn_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_INN:
    data->innom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    break;

 }
/***********
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
**************/ 
}
