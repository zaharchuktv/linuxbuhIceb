/*$Id: sklkar_m.c,v 1.13 2011-02-21 07:35:57 sasa Exp $*/
/*25.05.2004	25.05.2004	Белых А.И.	sklkar_m.c
Меню задания просмотра карточки материалла
*/
#include <stdlib.h>
#include "../headers/buhg_g.h"

enum
{
  FK2,
  FK3,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SKL,
  E_NKART,
  KOLENTER  
 };

class sklkar_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  static iceb_u_str skl;
  static iceb_u_str n_kar;

  sklkar_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear();
   }
  void clear()
   {
    skl.new_plus("");
    n_kar.new_plus("");
   }
 };

iceb_u_str sklkar_m_data::skl;
iceb_u_str sklkar_m_data::n_kar;

void    sklkar_m_v_vvod(GtkWidget *widget,class sklkar_m_data *data);
void  sklkar_m_v_knopka(GtkWidget *widget,class sklkar_m_data *data);
void   sklkar_m_rekviz(class sklkar_m_data *data);
gboolean   sklkar_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class sklkar_m_data *data);

extern char *name_system;
extern SQL_baza bd;

int sklkar_m(iceb_u_str *skl,iceb_u_str *n_kar,GtkWidget *wpredok)
{
sklkar_m_data data;


char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Просмотр карточки материалла"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(sklkar_m_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Просмотр карточки материалла"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

sprintf(strsql,"%s",gettext("Склад"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_SKL] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_SKL]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SKL]), data.entry[E_SKL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKL]), "activate",GTK_SIGNAL_FUNC(sklkar_m_v_vvod),&data);
if(data.skl.ravno() != NULL)
 gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKL]),data.skl.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKL]),(gpointer)E_SKL);

label=gtk_label_new(gettext("Номер карточки"));
data.entry[E_NKART] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_NKART]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NKART]), data.entry[E_NKART], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NKART]), "activate",GTK_SIGNAL_FUNC(sklkar_m_v_vvod),&data);
if(data.n_kar.ravno() != NULL)
 gtk_entry_set_text(GTK_ENTRY(data.entry[E_NKART]),data.n_kar.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NKART]),(gpointer)E_NKART);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Просмотр"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать просмотр"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(sklkar_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр списка складов"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(sklkar_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(sklkar_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(sklkar_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {
  skl->new_plus(data.skl.ravno());
  n_kar->new_plus(data.n_kar.ravno());
 }
 
return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   sklkar_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class sklkar_m_data *data)
{

//printf("sklkar_m_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
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
void  sklkar_m_v_knopka(GtkWidget *widget,class sklkar_m_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

  case FK3:
    iceb_vibrek(1,"Sklad",&data->skl,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKL]),data->skl.ravno());
    return;

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    sklkar_m_v_vvod(GtkWidget *widget,class sklkar_m_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("sklkar_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SKL:
    data->skl.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NKART:
    data->n_kar.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
