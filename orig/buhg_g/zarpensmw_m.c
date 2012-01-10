/*$Id: zarpensmw_m.c,v 1.5 2011-02-21 07:36:00 sasa Exp $*/
/*31.05.2009	21.06.2006	Белых А.И.	zarpensmw_m.c
Ввод и корректировка категорий работников
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "zarpensmw.h"
enum
 {
  E_DATAR,
  E_TABNOM,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class zarpensmw_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zarpensmw_rr *rk;
  
  zarpensmw_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datar.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAR]))));
    rk->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM]))));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   zarpensmw_m_key_press(GtkWidget *widget,GdkEventKey *event,class zarpensmw_m_data *data);
void  zarpensmw_m_knopka(GtkWidget *widget,class zarpensmw_m_data *data);
void    zarpensmw_m_vvod(GtkWidget *widget,class zarpensmw_m_data *data);
int zarpensmw_m_zap(class zarpensmw_m_data *data);
void  zarpensmw_m_e_knopka(GtkWidget *widget,class zarpensmw_m_data *data);


extern SQL_baza  bd;
extern char      *name_system;

int zarpensmw_m(class zarpensmw_rr *rekr,GtkWidget *wpredok)
{

class zarpensmw_m_data data;
char strsql[512];
iceb_u_str kikz;

data.rk=rekr;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",name_system,gettext("Расчёт месячного отчёта для пенсионного персонификованого учёта"));
label=gtk_label_new(gettext("Расчёт месячного отчёта для пенсионного персонификованого учёта"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zarpensmw_m_key_press),&data);

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

GtkTooltips *tooltips_enter[KOLENTER];


sprintf(strsql,"%s (%s)",gettext("Месяц расчёта"),gettext("м.г"));
data.knopka_enter[E_DATAR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAR]), data.knopka_enter[E_DATAR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAR]),"clicked",GTK_SIGNAL_FUNC(zarpensmw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAR]),(gpointer)E_DATAR);
tooltips_enter[E_DATAR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAR],data.knopka_enter[E_DATAR],gettext("Выбор даты"),NULL);

data.entry[E_DATAR] = gtk_entry_new_with_max_length (7);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAR]), data.entry[E_DATAR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAR]), "activate",GTK_SIGNAL_FUNC(zarpensmw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAR]),data.rk->datar.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAR]),(gpointer)E_DATAR);

sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_TABNOM]),"clicked",GTK_SIGNAL_FUNC(zarpensmw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_TABNOM]),(gpointer)E_TABNOM);
tooltips_enter[E_TABNOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_TABNOM],data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"),NULL);

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TABNOM]), "activate",GTK_SIGNAL_FUNC(zarpensmw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk->tabnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TABNOM]),(gpointer)E_TABNOM);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(zarpensmw_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введённой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(zarpensmw_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(zarpensmw_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

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
void  zarpensmw_m_e_knopka(GtkWidget *widget,class zarpensmw_m_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zarpensmw_m_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATAR:

    iceb_calendar1(&data->rk->datar,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAR]),data->rk->datar.ravno());
      
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

gboolean   zarpensmw_m_key_press(GtkWidget *widget,GdkEventKey *event,class zarpensmw_m_data *data)
{
//char  bros[512];

//printf("zarpensmw_m_key_press\n");
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
void  zarpensmw_m_knopka(GtkWidget *widget,class zarpensmw_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zarpensmw_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(zarpensmw_m_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK4:
    data->clear_rek();
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

void    zarpensmw_m_vvod(GtkWidget *widget,class zarpensmw_m_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("zarpensmw_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAR:
    data->rk->datar.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_TABNOM:
    data->rk->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int zarpensmw_m_zap(class zarpensmw_m_data *data)
{


if(data->rk->datar.prov_dat1() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
  return(1);
 }


return(0);

}
