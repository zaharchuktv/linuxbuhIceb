/*$Id: iceb_nalnaks_v.c,v 1.2 2011-08-29 07:13:46 sasa Exp $*/
/*12.02.2011	12.02.2011	Белых А.И.	iceb_nalnaks_v.c
Меню для ввода реквизитов 
*/
#include <stdlib.h>
#include "iceb_libbuh.h"

enum
 {
  E_VID_DOG,
  E_DAT_DOG,
  E_NOM_DOG,

  E_VID_POKUP,
  E_VKL,
  E_OST_U_PROD,
  E_TIP_PRIH,
  E_KOPIQ,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class iceb_nalnaks_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет
  class iceb_u_str vid_dog;
  class iceb_u_str dat_dog;
  class iceb_u_str nom_dog;
  
  class iceb_u_str vkl;
  class iceb_u_str vid_pokup;
  class iceb_u_str ost_u_prod;
  class iceb_u_str tip_prih;
  class iceb_u_str kopiq;
  
  iceb_nalnaks_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    vid_dog.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VID_DOG])));
    dat_dog.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DAT_DOG])));
    nom_dog.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DAT_DOG])));

    vkl.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VKL])));
    vid_pokup.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VID_POKUP])));
    ost_u_prod.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_OST_U_PROD])));
    tip_prih.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TIP_PRIH])));
    kopiq.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOPIQ])));
  
   }

  void clear_data()
   {
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void iceb_nalnaks_v_clear(class iceb_nalnaks_v_data *data);
void    iceb_nalnaks_v_vvod(GtkWidget *widget,class iceb_nalnaks_v_data *data);
void  iceb_nalnaks_v_knopka(GtkWidget *widget,class iceb_nalnaks_v_data *data);
gboolean   iceb_nalnaks_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_nalnaks_v_data *data);

extern char      *name_system;

int iceb_nalnaks_v(class iceb_u_str *vid_dog,
class iceb_u_str *dat_dog,
class iceb_u_str *nom_dog,
class iceb_u_str *vid_pokup,
class iceb_u_str *vkl,
class iceb_u_str *ost_u_prod,
class iceb_u_str *tip_prih,
class iceb_u_str *kopiq,
GtkWidget *wpredok)
{
class iceb_nalnaks_v_data data;
char strsql[512];

data.vid_dog.new_plus(vid_dog->ravno());
data.dat_dog.new_plus(dat_dog->ravno());
data.nom_dog.new_plus(nom_dog->ravno());

data.vid_pokup.new_plus(vid_pokup->ravno());
data.vkl.new_plus(vkl->ravno());
data.ost_u_prod.new_plus(ost_u_prod->ravno());
data.tip_prih.new_plus(tip_prih->ravno());
data.kopiq.new_plus(kopiq->ravno());

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_nalnaks_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);



GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
 gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 0);


GtkWidget *label=gtk_label_new(gettext("Вид договора"));
gtk_box_pack_start (GTK_BOX (hbox[E_VID_DOG]), label, FALSE, FALSE, 0);

data.entry[E_VID_DOG] = gtk_entry_new_with_max_length (64);
gtk_box_pack_start (GTK_BOX (hbox[E_VID_DOG]), data.entry[E_VID_DOG], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VID_DOG]), "activate",GTK_SIGNAL_FUNC(iceb_nalnaks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VID_DOG]),data.vid_dog.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VID_DOG]),(gpointer)E_VID_DOG);

label=gtk_label_new(gettext("Дата договора"));
gtk_box_pack_start (GTK_BOX (hbox[E_DAT_DOG]), label, FALSE, FALSE, 0);

data.entry[E_DAT_DOG] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DAT_DOG]), data.entry[E_DAT_DOG], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DAT_DOG]), "activate",GTK_SIGNAL_FUNC(iceb_nalnaks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DAT_DOG]),data.dat_dog.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DAT_DOG]),(gpointer)E_DAT_DOG);

label=gtk_label_new(gettext("Номер договора"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_DOG]), label, FALSE, FALSE, 0);

data.entry[E_NOM_DOG] = gtk_entry_new_with_max_length (60);
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_DOG]), data.entry[E_NOM_DOG], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOM_DOG]), "activate",GTK_SIGNAL_FUNC(iceb_nalnaks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOM_DOG]),data.nom_dog.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOM_DOG]),(gpointer)E_NOM_DOG);



label=gtk_label_new(gettext("Выдаётся покупателю"));
gtk_box_pack_start (GTK_BOX (hbox[E_VID_POKUP]), label, FALSE, FALSE, 0);

data.entry[E_VID_POKUP] = gtk_entry_new_with_max_length (1);
gtk_box_pack_start (GTK_BOX (hbox[E_VID_POKUP]), data.entry[E_VID_POKUP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VID_POKUP]), "activate",GTK_SIGNAL_FUNC(iceb_nalnaks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VID_POKUP]),data.vid_pokup.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VID_POKUP]),(gpointer)E_VID_POKUP);


label=gtk_label_new(gettext("Включено до ЕРНП"));
gtk_box_pack_start (GTK_BOX (hbox[E_VKL]), label, FALSE, FALSE, 0);

data.entry[E_VKL] = gtk_entry_new_with_max_length (1);
gtk_box_pack_start (GTK_BOX (hbox[E_VKL]), data.entry[E_VKL], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VKL]), "activate",GTK_SIGNAL_FUNC(iceb_nalnaks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VKL]),data.vkl.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VKL]),(gpointer)E_VKL);


label=gtk_label_new(gettext("Остаётся у продавца"));
gtk_box_pack_start (GTK_BOX (hbox[E_OST_U_PROD]), label, FALSE, FALSE, 0);

data.entry[E_OST_U_PROD] = gtk_entry_new_with_max_length (1);
gtk_box_pack_start (GTK_BOX (hbox[E_OST_U_PROD]), data.entry[E_OST_U_PROD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_OST_U_PROD]), "activate",GTK_SIGNAL_FUNC(iceb_nalnaks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OST_U_PROD]),data.ost_u_prod.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_OST_U_PROD]),(gpointer)E_OST_U_PROD);

label=gtk_label_new(gettext("Тип причины"));
gtk_box_pack_start (GTK_BOX (hbox[E_TIP_PRIH]), label, FALSE, FALSE, 0);

data.entry[E_TIP_PRIH] = gtk_entry_new_with_max_length (2);
gtk_box_pack_start (GTK_BOX (hbox[E_TIP_PRIH]), data.entry[E_TIP_PRIH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TIP_PRIH]), "activate",GTK_SIGNAL_FUNC(iceb_nalnaks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TIP_PRIH]),data.tip_prih.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TIP_PRIH]),(gpointer)E_TIP_PRIH);


label=gtk_label_new(gettext("Копия"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOPIQ]), label, FALSE, FALSE, 0);

data.entry[E_KOPIQ] = gtk_entry_new_with_max_length (1);
gtk_box_pack_start (GTK_BOX (hbox[E_KOPIQ]), data.entry[E_KOPIQ], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOPIQ]), "activate",GTK_SIGNAL_FUNC(iceb_nalnaks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOPIQ]),data.kopiq.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOPIQ]),(gpointer)E_KOPIQ);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Ввести информацию"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(iceb_nalnaks_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(iceb_nalnaks_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Выйти из меню"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(iceb_nalnaks_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.voz == 0)
 {
  vid_dog->new_plus(data.vid_dog.ravno());
  dat_dog->new_plus(data.dat_dog.ravno());
  nom_dog->new_plus(data.nom_dog.ravno());
  vid_pokup->new_plus(data.vid_pokup.ravno());
  vkl->new_plus(data.vkl.ravno());
  ost_u_prod->new_plus(data.ost_u_prod.ravno());
  tip_prih->new_plus(data.tip_prih.ravno());
  kopiq->new_plus(data.kopiq.ravno());
  
 }
return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_nalnaks_v_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_nalnaks_v_data *data)
{
//char  bros[512];

//printf("vl_zarkateg_key_press\n");
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
void  iceb_nalnaks_v_knopka(GtkWidget *widget,class iceb_nalnaks_v_data *data)
{
//char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("iceb_nalnaks_v_knopka knop=%d\n",knop);

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

void    iceb_nalnaks_v_vvod(GtkWidget *widget,class iceb_nalnaks_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (enter)
 {
  case E_VID_DOG:
    data->vid_dog.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_VID_DOG])));
    break;
  case E_DAT_DOG:
    data->dat_dog.new_plus((char*)gtk_entry_get_text(GTK_ENTRY(data->entry[E_DAT_DOG])));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
