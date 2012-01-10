/*$Id: l_uplsal_p.c,v 1.6 2011-02-21 07:35:54 sasa Exp $*/
/*07.03.2008	07.03.2008	Белых А.И.	l_uplsal_p.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_uplsal.h"
enum
 {
  E_GOD,
  E_KOD_AVT,
  E_KOD_VOD,
  E_KOD_TOP,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uplsal_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uplsal_rek *rk;
  
  
  l_uplsal_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GOD]))));
    rk->kod_avt.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_AVT]))));
    rk->kod_vod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_VOD]))));
    rk->kod_top.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_TOP]))));
   }

  void clear_data()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void l_uplsal_p_clear(class l_uplsal_p_data *data);
void    l_uplsal_p_vvod(GtkWidget *widget,class l_uplsal_p_data *data);
void  l_uplsal_p_knopka(GtkWidget *widget,class l_uplsal_p_data *data);
gboolean   l_uplsal_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplsal_p_data *data);

void  l_uplsal_p_v_knopka(GtkWidget *widget,class l_uplsal_p_data *data);

extern char      *name_system;

int l_uplsal_p(class l_uplsal_rek *rek_poi,GtkWidget *wpredok)
{
class l_uplsal_p_data data;
char strsql[512];
data.rk=rek_poi;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uplsal_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }



GtkWidget *vbox = gtk_vbox_new (FALSE,1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

GtkWidget *label=gtk_label_new(gettext("Год"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE,1);

data.entry[E_GOD] = gtk_entry_new_with_max_length (4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOD]), "activate",GTK_SIGNAL_FUNC(l_uplsal_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.rk->god.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOD]),(gpointer)E_GOD);



sprintf(strsql,"%s (,,)",gettext("Код автомобиля"));
data.knopka_enter[E_KOD_AVT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.knopka_enter[E_KOD_AVT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_AVT]),"clicked",GTK_SIGNAL_FUNC(l_uplsal_p_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_AVT]),(gpointer)E_KOD_AVT);
tooltips_enter[E_KOD_AVT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_AVT],data.knopka_enter[E_KOD_AVT],gettext("Выбор автомобиля"),NULL);

data.entry[E_KOD_AVT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_AVT]), data.entry[E_KOD_AVT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_AVT]), "activate",GTK_SIGNAL_FUNC(l_uplsal_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_AVT]),data.rk->kod_avt.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_AVT]),(gpointer)E_KOD_AVT);


sprintf(strsql,"%s (,,)",gettext("Код водителя"));
data.knopka_enter[E_KOD_VOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.knopka_enter[E_KOD_VOD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_VOD]),"clicked",GTK_SIGNAL_FUNC(l_uplsal_p_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_VOD]),(gpointer)E_KOD_VOD);
tooltips_enter[E_KOD_VOD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_VOD],data.knopka_enter[E_KOD_VOD],gettext("Выбор водителя"),NULL);

data.entry[E_KOD_VOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_VOD]), data.entry[E_KOD_VOD], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_VOD]), "activate",GTK_SIGNAL_FUNC(l_uplsal_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_VOD]),data.rk->kod_vod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_VOD]),(gpointer)E_KOD_VOD);


sprintf(strsql,"%s (,,)",gettext("Код топлива"));
data.knopka_enter[E_KOD_TOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.knopka_enter[E_KOD_TOP], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_TOP]),"clicked",GTK_SIGNAL_FUNC(l_uplsal_p_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_TOP]),(gpointer)E_KOD_TOP);
tooltips_enter[E_KOD_TOP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_TOP],data.knopka_enter[E_KOD_TOP],gettext("Выбор топлива"),NULL);

data.entry[E_KOD_TOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_TOP]), data.entry[E_KOD_TOP], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_TOP]), "activate",GTK_SIGNAL_FUNC(l_uplsal_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_TOP]),data.rk->kod_top.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_TOP]),(gpointer)E_KOD_TOP);


GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_uplsal_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(l_uplsal_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Поиск не выполнять"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_uplsal_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uplsal_p_v_knopka(GtkWidget *widget,class l_uplsal_p_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplsal_p_v_knopka knop=%d\n",knop);

switch (knop)
 {

  case E_KOD_AVT:
     if(l_uplavt(1,&kod,&naim,data->window) == 0)
      data->rk->kod_avt.z_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_AVT]),data->rk->kod_avt.ravno());
      
    return;  

  case E_KOD_VOD:
     if(l_uplout(1,&kod,&naim,1,data->window) == 0)
      data->rk->kod_vod.z_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_VOD]),data->rk->kod_vod.ravno());
      
    return;  

  case E_KOD_TOP:
     if(l_uplmt(1,&kod,&naim,data->window) == 0)
      data->rk->kod_top.z_plus(kod.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_TOP]),data->rk->kod_top.ravno_toutf());
      
    return;  

 }   
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uplsal_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_uplsal_p_data *data)
{
//char  bros[512];

//printf("vl_uplsal_key_press\n");
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
void  l_uplsal_p_knopka(GtkWidget *widget,class l_uplsal_p_data *data)
{
//char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplsal_p_knopka knop=%d\n",knop);

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

void    l_uplsal_p_vvod(GtkWidget *widget,class l_uplsal_p_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uplsal_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_GOD:
    data->rk->god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOD_AVT:
    data->rk->kod_avt.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
