/*$Id: l_tk_saldo_p.c,v 1.8 2011-02-21 07:36:20 sasa Exp $*/
/*04.04.2005	04.04.2005	Белых А.И.	l_tk_saldo_p.c
Ввод и корректировка реквизитов поиска
*/
#include <unistd.h>
#include "taxi.h"
#include "l_tk_saldo.h"

enum
{
 FK2,
// FK3,
 FK4,
 FK10,
 KOL_F_KL
};

enum
{
 E_GOD,
 E_KOD,
 E_SALDO,
 KOLENTER
};

class tk_saldo_p_data 
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *entry[KOLENTER];
  GtkWidget *opt;
  short     kl_shift; //0-отжата 1-нажата  

  int       voz;
       
  class tk_saldo_rek_data *poi;
  iceb_u_str kodkl;
  iceb_u_spisok menu_kodgr;
  int           vib_kodgr;
      
  tk_saldo_p_data()
   {
    vib_kodgr=0;
    voz=0;
    kl_shift=0;
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
    poi->clear_data();
   }
 };


gboolean   tk_saldo_p_key_press(GtkWidget *widget,GdkEventKey *event,class tk_saldo_p_data *data);
void  tk_saldo_p_knopka(GtkWidget *widget,class tk_saldo_p_data *data);
void    tk_saldo_p_vvod(GtkWidget *widget,class tk_saldo_p_data *data);
void  tk_saldo_p_v_e_knopka(GtkWidget *widget,class tk_saldo_p_data *data);

extern SQL_baza	bd;
extern char *name_system;


int l_tk_saldo_p(class tk_saldo_rek_data *datap,GtkWidget *wpredok)
{
char strsql[300];

class tk_saldo_p_data data;
data.poi=datap;
printf("l_tk_saldo_p\n");
    
data.kl_shift=0;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));


gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(tk_saldo_p_key_press),&data);


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
for(int i=0 ; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0 ; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

GtkWidget *label=gtk_label_new(iceb_u_toutf("Год"));
data.entry[E_GOD] = gtk_entry_new_with_max_length (4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOD]), "activate",GTK_SIGNAL_FUNC(tk_saldo_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.poi->god.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOD]),(gpointer)E_GOD);

data.knopka_enter[E_KOD]=gtk_button_new_with_label(iceb_u_toutf("Код клиента (,,)"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_enter[E_KOD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD]),"clicked",GTK_SIGNAL_FUNC(tk_saldo_p_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD]),(gpointer)E_KOD);
tooltips_enter[E_KOD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD],data.knopka_enter[E_KOD],iceb_u_toutf("Выбор кода клиента в списке"),NULL);

data.entry[E_KOD] = gtk_entry_new_with_max_length (19);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(tk_saldo_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.poi->kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);

/*************
label=gtk_label_new(iceb_u_toutf("Код клиента (,,)"));
data.entry[E_KOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD]), "activate",GTK_SIGNAL_FUNC(tk_saldo_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.poi->kod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD]),(gpointer)E_KOD);
*****************/
label=gtk_label_new(iceb_u_toutf("Сальдо"));
data.entry[E_SALDO] = gtk_entry_new_with_max_length (99);
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SALDO]), data.entry[E_SALDO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SALDO]), "activate",GTK_SIGNAL_FUNC(tk_saldo_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SALDO]),data.poi->saldo.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SALDO]),(gpointer)E_SALDO);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(tk_saldo_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(tk_saldo_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без поиска нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(tk_saldo_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

printf("l_tk_saldo_p end\n");

return(data.voz);



}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  tk_saldo_p_v_e_knopka(GtkWidget *widget,class tk_saldo_p_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_KOD:

//    if(lklient(1,&kod,data->window) == 0)
    if(l_taxisk(1,&kod,&naim,data->window) == 0)
     {
      data->poi->kod.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->poi->kod.ravno());
     }      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   tk_saldo_p_key_press(GtkWidget *widget,GdkEventKey *event,class tk_saldo_p_data *data)
{
//char  bros[300];

//printf("tk_saldo_p_key_press\n");
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
    return(TRUE);
//    return(FALSE);

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
void  tk_saldo_p_knopka(GtkWidget *widget,class tk_saldo_p_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case FK2:
    data->read_rek();
    
    gtk_widget_destroy(data->window);
    data->voz=0;     
    
    return;  


  case FK4:
    data->clear_rek();
    return;


  case FK10:
    data->read_rek();
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    tk_saldo_p_vvod(GtkWidget *widget,class tk_saldo_p_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("tk_saldo_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->poi->kod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_GOD:
    data->poi->god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SALDO:
    data->poi->saldo.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
