/*$Id: rasf1df_m.c,v 1.9 2011-02-21 07:35:56 sasa Exp $*/
/*23.04.2007	23.04.2007	Белых А.И.	rasf1df_m.c
Меню для ввода количества работников
*/

#include "buhg_g.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOLIH_V_HTATE,
  E_KOLIH_PO_SOVM,
  KOLENTER  
 };

class rasf1df_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  class iceb_u_str kolih_v_htate;
  class iceb_u_str kolih_po_sovm;


  rasf1df_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    kolih_v_htate.plus("");
    kolih_po_sovm.plus("");
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
    kolih_v_htate.new_plus("");
    kolih_po_sovm.new_plus("");
   }
 };


gboolean   rasf1df_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class rasf1df_m_data *data);
void    rasf1df_m_v_vvod(GtkWidget *widget,class rasf1df_m_data *data);
void  rasf1df_m_v_knopka(GtkWidget *widget,class rasf1df_m_data *data);
void rasf1df_m_r(class rasf1df_m_rr *datap);



extern char *name_system;

int  rasf1df_m(class iceb_u_str *kolih_v_htate,
class iceb_u_str *kolih_po_sovm,
GtkWidget *wpredok)
{
class rasf1df_m_data data;

 
char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Форма 1ДФ"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rasf1df_m_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=gtk_label_new(gettext("Форма 1ДФ"));

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

//GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Работало в штате"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH_V_HTATE]), label, FALSE, FALSE, 0);


data.entry[E_KOLIH_V_HTATE] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH_V_HTATE]), data.entry[E_KOLIH_V_HTATE], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOLIH_V_HTATE]), "activate",GTK_SIGNAL_FUNC(rasf1df_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH_V_HTATE]),data.kolih_v_htate.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOLIH_V_HTATE]),(gpointer)E_KOLIH_V_HTATE);

sprintf(strsql,"%s",gettext("Работало по совместительству"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH_PO_SOVM]), label, FALSE, FALSE, 0);


data.entry[E_KOLIH_PO_SOVM] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH_PO_SOVM]), data.entry[E_KOLIH_PO_SOVM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOLIH_PO_SOVM]), "activate",GTK_SIGNAL_FUNC(rasf1df_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH_PO_SOVM]),data.kolih_po_sovm.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOLIH_PO_SOVM]),(gpointer)E_KOLIH_PO_SOVM);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(rasf1df_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(rasf1df_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(rasf1df_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();


if(data.voz == 0)
 {
  kolih_v_htate->new_plus(data.kolih_v_htate.ravno());
  kolih_po_sovm->new_plus(data.kolih_po_sovm.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
return(data.voz);

}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasf1df_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class rasf1df_m_data *data)
{

//printf("rasf1df_m_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
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
void  rasf1df_m_v_knopka(GtkWidget *widget,class rasf1df_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK4:
    data->clear_rek();
    return;

  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    rasf1df_m_v_vvod(GtkWidget *widget,class rasf1df_m_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("rasf1df_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOLIH_V_HTATE:
    data->kolih_v_htate.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOLIH_PO_SOVM:
    data->kolih_po_sovm.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
