/*$Id: l_f1dfz_p.c,v 1.9 2011-02-21 07:35:52 sasa Exp $*/
/*09.01.2007	09.01.2007	Белых А.И.	l_f1dfz_p.c
Ввод и корректировка записи по постороннему человеку в документ формы 1ДФ
Если вернули 0-искать 1- не искать
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "l_f1dfz.h"

enum
 {
  E_FIO,
  E_INN,
  E_OZ_DOH,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_f1dfz_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  
  class l_f1dfz_rek *rk;
        
    
  l_f1dfz_p_data() //Конструктор
   {
    kl_shift=0;
   }

  void read_rek()
   {
    rk->fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_FIO]))));
    rk->inn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_INN]))));
    rk->kod_doh.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_OZ_DOH]))));
   }
 };

gboolean   l_f1dfz_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfz_p_data *data);
void  l_f1dfz_p_knopka(GtkWidget *widget,class l_f1dfz_p_data *data);
void    l_f1dfz_p_vvod(GtkWidget *widget,class l_f1dfz_p_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int l_f1dfz_p(class l_f1dfz_rek *rek,
GtkWidget *wpredok)
{

class l_f1dfz_p_data data;
char strsql[512];
data.rk=rek;

 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",name_system,gettext("Поиск нужных записей"));
label=gtk_label_new(gettext("Поиск нужных записей"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_f1dfz_p_key_press),&data);

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
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 1);


label=gtk_label_new(gettext("Фамилия"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);

data.entry[E_FIO] = gtk_entry_new_with_max_length (59);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FIO]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.rk->fio.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FIO]),(gpointer)E_FIO);



label=gtk_label_new(gettext("Инд. нал. номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), label, FALSE, FALSE, 0);

data.entry[E_INN] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.entry[E_INN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_INN]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INN]),data.rk->inn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_INN]),(gpointer)E_INN);



label=gtk_label_new(gettext("Признак дохода"));
gtk_box_pack_start (GTK_BOX (hbox[E_OZ_DOH]), label, FALSE, FALSE, 0);

data.entry[E_OZ_DOH] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_OZ_DOH]), data.entry[E_OZ_DOH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_OZ_DOH]), "activate",GTK_SIGNAL_FUNC(l_f1dfz_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OZ_DOH]),data.rk->kod_doh.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_OZ_DOH]),(gpointer)E_OZ_DOH);



GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_f1dfz_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_f1dfz_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.rk->metka_poi);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_f1dfz_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfz_p_data *data)
{
//char  bros[512];

//printf("l_f1dfz_p_key_press\n");
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
void  l_f1dfz_p_knopka(GtkWidget *widget,class l_f1dfz_p_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_f1dfz_p_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->rk->metka_poi=0;
     
    gtk_widget_destroy(data->window);
    return;  


  case FK10:
    data->rk->metka_poi=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_f1dfz_p_vvod(GtkWidget *widget,class l_f1dfz_p_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_f1dfz_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_INN:
    data->rk->inn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_FIO:
    data->rk->fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    break;

  case E_OZ_DOH:
    data->rk->kod_doh.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
