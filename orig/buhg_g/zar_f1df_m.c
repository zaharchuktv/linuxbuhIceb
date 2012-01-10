/*$Id: zar_f1df_m.c,v 1.10 2011-02-21 07:35:59 sasa Exp $*/
/*20.12.2006	20.12.2006	Белых А.И.	zar_f1df_m.c
Меню для расчёта формы 1ДФ
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "zar_f1df.h"
enum
 {
  E_KVART,
  E_DATAN,
  E_DATAK,
  E_TABNOM,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK5,
  FK10,
  KOL_FK
 };

class zar_f1df_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *spin_enter_mesn;
  GtkWidget *knopka_enter[KOLENTER];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zar_f1df_rek *rk;
  
  zar_f1df_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->kvart.new_plus(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_enter_mesn)));
    rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN]))));
    rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK]))));
    rk->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM]))));
   }
  
  void clear_rek()
   {

    rk->clear_data();
    for(int i=1; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    //gtk_widget_grab_focus(entry[0]);

   }
 };


gboolean   zar_f1df_m_key_press(GtkWidget *widget,GdkEventKey *event,class zar_f1df_m_data *data);
void  zar_f1df_m_knopka(GtkWidget *widget,class zar_f1df_m_data *data);
void    zar_f1df_m_vvod(GtkWidget *widget,class zar_f1df_m_data *data);


void  zar_f1df_m_e_knopka(GtkWidget *widget,class zar_f1df_m_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int zar_f1df_m(class zar_f1df_rek *rek,GtkWidget *wpredok)
{

class zar_f1df_m_data data;
char strsql[512];
iceb_u_str kikz;
data.rk=rek;
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",name_system,gettext("Распечатать форму 1ДФ"));
label=gtk_label_new(gettext("Распечатать форму 1ДФ"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_f1df_m_key_press),&data);

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

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Квартал"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KVART]), label, FALSE, FALSE, 0);

data.spin_enter_mesn=gtk_spin_button_new_with_range(1,4,1);
if(data.rk->kvart.ravno_atoi() != 0)
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(data.spin_enter_mesn),(gfloat)data.rk->kvart.ravno_atof());
gtk_box_pack_start (GTK_BOX (hbox[E_KVART]),data.spin_enter_mesn,FALSE, FALSE, 0);


sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(zar_f1df_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(zar_f1df_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);


sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(zar_f1df_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(zar_f1df_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_TABNOM]),"clicked",GTK_SIGNAL_FUNC(zar_f1df_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_TABNOM]),(gpointer)E_TABNOM);
tooltips_enter[E_TABNOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_TABNOM],data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"),NULL);

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TABNOM]), "activate",GTK_SIGNAL_FUNC(zar_f1df_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk->tabnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TABNOM]),(gpointer)E_TABNOM);




GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчёт"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(zar_f1df_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(zar_f1df_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Настройка расчёта"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]),"clicked",GTK_SIGNAL_FUNC(zar_f1df_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK5], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(zar_f1df_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

//gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  zar_f1df_m_e_knopka(GtkWidget *widget,class zar_f1df_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_DATAN:
    iceb_calendar(&data->rk->datan,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno_toutf());
    return;

  case E_DATAK:
    iceb_calendar(&data->rk->datak,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno_toutf());
    return;

  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->rk->tabnom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk->tabnom.ravno_toutf());
    return;
  
    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_f1df_m_key_press(GtkWidget *widget,GdkEventKey *event,class zar_f1df_m_data *data)
{
//char  bros[512];

//printf("zar_f1df_m_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");

    return(TRUE);



  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");

    return(TRUE);

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");

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
void  zar_f1df_m_knopka(GtkWidget *widget,class zar_f1df_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zar_f1df_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:

    data->read_rek();
    if(data->rk->datan.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата начала !"),data->window);
      return;
     }
    if(data->rk->datak.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата конца !"),data->window);
      return;
     }
    if(iceb_rsdatp(data->rk->datan.ravno(),data->rk->datak.ravno(),data->window) != 0)
     return;
     
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

    
  case FK4:
    data->clear_rek();
    return;

  case FK5:
    iceb_f_redfil("zarsdf1df.alx",0,data->window);
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

void    zar_f1df_m_vvod(GtkWidget *widget,class zar_f1df_m_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zar_f1df_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KVART:
    data->rk->kvart.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATAN:
    data->rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATAK:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_TABNOM:
    data->rk->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=1;
gtk_widget_grab_focus(data->entry[enter]);
 
}
