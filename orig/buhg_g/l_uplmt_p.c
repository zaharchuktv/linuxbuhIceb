/*$Id: l_uplmt_p.c,v 1.7 2011-02-21 07:35:54 sasa Exp $*/
/*05.03.2008	05.03.2008	Белых А.И.	l_uplmt_p.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_uplmt.h"
enum
 {
  E_KOD_TOP,
  E_KOD_MAT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uplmt_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uplmt_rek *rk;
  
  
  l_uplmt_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->kod_top.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_TOP]))));
    rk->kod_mat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_MAT]))));
   }

  void clear_data()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void l_uplmt_p_clear(class l_uplmt_p_data *data);
void    l_uplmt_p_vvod(GtkWidget *widget,class l_uplmt_p_data *data);
void  l_uplmt_p_knopka(GtkWidget *widget,class l_uplmt_p_data *data);
gboolean   l_uplmt_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplmt_p_data *data);

extern char      *name_system;

int l_uplmt_p(class l_uplmt_rek *rek_poi,GtkWidget *wpredok)
{
class l_uplmt_p_data data;
char strsql[512];
data.rk=rek_poi;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uplmt_p_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Код палива"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), label, FALSE, FALSE, 0);

data.entry[E_KOD_TOP] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.entry[E_KOD_TOP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_TOP]), "activate",GTK_SIGNAL_FUNC(l_uplmt_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_TOP]),data.rk->kod_top.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_TOP]),(gpointer)E_KOD_TOP);


label=gtk_label_new(gettext("Код материалла"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_MAT]), label, FALSE, FALSE, 0);

data.entry[E_KOD_MAT] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_MAT]), data.entry[E_KOD_MAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_MAT]), "activate",GTK_SIGNAL_FUNC(l_uplmt_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_MAT]),data.rk->kod_mat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_MAT]),(gpointer)E_KOD_MAT);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_uplmt_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(l_uplmt_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Поиск не выполнять"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_uplmt_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplmt_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplmt_p_data *data)
{
//char  bros[512];

//printf("vl_uplmt_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(FALSE);

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
void  l_uplmt_p_knopka(GtkWidget *widget,class l_uplmt_p_data *data)
{
//char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplmt_p_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_uplmt_p_vvod(GtkWidget *widget,class l_uplmt_p_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplmt_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD_TOP:
    data->rk->kod_top.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOD_MAT:
    data->rk->kod_mat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
