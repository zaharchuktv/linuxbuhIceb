/*$Id: saldo_m.c,v 1.4 2011-02-21 07:35:57 sasa Exp $*/
/*12.10.2010	12.10.2010	Белых А.И.	saldo_m.c
Ввод и корректировка категорий работников
*/
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "saldo.h"
enum
 {
  E_DATAN,
  E_DATAK,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class saldo_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class saldo_m_rek *rk;
  
  saldo_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN]))));
    rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK]))));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   saldo_m_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_m_data *data);
void  saldo_m_knopka(GtkWidget *widget,class saldo_m_data *data);
void    saldo_m_vvod(GtkWidget *widget,class saldo_m_data *data);
int l_zarkateg_zap(class saldo_m_data *data);


extern SQL_baza  bd;
extern char      *name_system;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int saldo_m(class saldo_m_rek *rek,GtkWidget *wpredok)
{

class saldo_m_data data;
char strsql[512];
class iceb_u_str kikz;
data.rk=rek;
short god=0;
data.rk->clear_data();

if(iceb_prn(wpredok) != 0)

 return(1);
if(iceb_kod_podsystem == ICEB_PS_GK)
  if(iceb_poldan("Стартовый год",strsql,"nastrb.alx",wpredok) == 0)
    god=atoi(strsql);    

if(iceb_kod_podsystem == ICEB_PS_MU)
  if(iceb_poldan("Стартовый год",strsql,"matnast.alx",NULL) == 0)
    god=atoi(strsql);

if(iceb_kod_podsystem == ICEB_PS_UPL)
  if(iceb_poldan("Стартовый год",strsql,"uplnast.alx",NULL) == 0)
    god=atoi(strsql);

if(iceb_kod_podsystem == ICEB_PS_ZP)
  if(iceb_poldan("Стартовый год",strsql,"zarnast.alx",NULL) == 0)
    god=atoi(strsql);

if(god == 0)
 {
  iceb_menu_soob(gettext("Не введён стартовый год!"),wpredok);
  return(1);
 } 
data.rk->datan.new_plus(god);
data.rk->datak.new_plus(god+1);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(iceb_kod_podsystem == ICEB_PS_GK)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Перенос сальдо для подсистемы \"Главная книга\""));
  label=gtk_label_new(gettext("Перенос сальдо для подсистемы \"Главная книга\""));
 }
if(iceb_kod_podsystem == ICEB_PS_MU)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Перенос сальдо для подсистемы \"Материальный учёт\""));
  label=gtk_label_new(gettext("Перенос сальдо для подсистемы \"Материальный учёт\""));
 }
if(iceb_kod_podsystem == ICEB_PS_ZP)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Перенос сальдо для подсистемы \"Заработная плата\""));
  label=gtk_label_new(gettext("Перенос сальдо для подсистемы \"Заработная плата\""));
 }
if(iceb_kod_podsystem == ICEB_PS_UPL)
 {
  sprintf(strsql,"%s %s",name_system,gettext("Перенос сальдо для подсистемы \"Учёт путевых листов\""));
  label=gtk_label_new(gettext("Перенос сальдо для подсистемы \"Учёт путевых листов\""));
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(saldo_m_key_press),&data);

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
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("С какого года"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), label, FALSE, FALSE, 0);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(saldo_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

label=gtk_label_new(gettext("На какой год"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), label, FALSE, FALSE, 0);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(saldo_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Перенесення сальдо"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(saldo_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введённой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(saldo_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(saldo_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));


return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   saldo_m_key_press(GtkWidget *widget,GdkEventKey *event,class saldo_m_data *data)
{
//char  bros[512];

printf("saldo_m_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    printf("%s\n",__FUNCTION__);
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
  //  printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldo_m_knopka(GtkWidget *widget,class saldo_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("saldo_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    printf("%s\n",__FUNCTION__);
    data->read_rek();
    if(l_zarkateg_zap(data) == 0)
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

void    saldo_m_vvod(GtkWidget *widget,class saldo_m_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("saldo_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATAK:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
int l_zarkateg_zap(class saldo_m_data *data)
{

if(data->rk->datan.ravno_atoi() == 0 || data->rk->datan.ravno_atoi() < 1900 || data->rk->datan.ravno_atoi() > 3000)
 { 
  iceb_menu_soob("Неправильно введён год с которого нужно перенести сальдо!",data->window);
  return(1);
 } 


if(data->rk->datak.ravno_atoi() == 0 || data->rk->datak.ravno_atoi() < 1900 || data->rk->datak.ravno_atoi() > 3000)
 { 
  iceb_menu_soob("Не верно веден год на который нужно перенести !",data->window);
  return(1);
 } 

/*Проверяем заблокирована ли дата*/
if(iceb_pbpds(1,data->rk->datak.ravno_atoi(),data->window) != 0)
 return(1);
 
return(0);

}
