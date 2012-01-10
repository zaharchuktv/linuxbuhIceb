/*$Id: taxi_oth_v.c,v 1.28 2011-02-21 07:36:21 sasa Exp $*/
/*30.05.2009	08.12.2003	Белых А.И.	taxi_oth_v.c
Меню ввода данных для получения отчета
*/
#include        <time.h>
#include        <stdlib.h>
#include        <unistd.h>
#include "taxi.h"
#include "taxi_oth.h"

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };
 
void  taxi_oth_v_knopka(GtkWidget *widget,class taxi_oth_data *data);
gboolean   taxi_oth_v_key_press(GtkWidget *widget,GdkEventKey *event,class taxi_oth_data *data);
void    taxi_oth_v_vvod(GtkWidget *widget,class taxi_oth_data *data);
int    taxi_oth_prov(class taxi_oth_data *data);
void taxi_oth_clear(class taxi_oth_data *data);

void       taxi_oth_v_radio0(GtkWidget *,class taxi_oth_data *);
void       taxi_oth_v_radio1(GtkWidget *,class taxi_oth_data *);

void  taxi_oth_v_v_e_knopka(GtkWidget *widget,class taxi_oth_data *data);

//extern SQL_baza	bd;
extern char *name_system;

int taxi_oth_v(class taxi_oth_data *data)
{
char strsql[300];

data->window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data->window),ICEB_POS_CENTER);
//gtk_window_set_modal( GTK_WINDOW(data->window) ,TRUE ); 

sprintf(strsql,"%s %s",NAME_SYSTEM,iceb_u_toutf("Получение отчетов"));
gtk_window_set_title(GTK_WINDOW(data->window),strsql);
gtk_signal_connect(GTK_OBJECT(data->window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data->window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data->window),"key_press_event",GTK_SIGNAL_FUNC(taxi_oth_v_key_press),data);

GtkTooltips *tooltips_enter[KOLENTER];

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox1 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox2 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox4 = gtk_hbox_new (FALSE, 0);
GtkWidget *hbox5 = gtk_hbox_new (FALSE, 0);
GtkWidget *separator = gtk_hseparator_new ();
GtkWidget *hboxradio = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data->window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), separator);
gtk_container_add (GTK_CONTAINER (vbox), hboxradio);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

gtk_widget_set_usize(separator,0,10);


//Вставляем радиокнопки
GSList *group;

data->radiobutton0=gtk_radio_button_new_with_label(NULL,iceb_u_toutf("По времени записи заказа"));
gtk_box_pack_start (GTK_BOX (hboxradio),data->radiobutton0, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data->radiobutton0), "clicked",GTK_SIGNAL_FUNC(taxi_oth_v_radio0),data);
//gtk_object_set_user_data(GTK_OBJECT(data->radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data->radiobutton0));


data->radiobutton1=gtk_radio_button_new_with_label(group,iceb_u_toutf("По дате заказа"));
//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data->radiobutton3),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data->radiobutton1), "clicked",GTK_SIGNAL_FUNC(taxi_oth_v_radio1),data);
gtk_box_pack_start (GTK_BOX (hboxradio),data->radiobutton1, TRUE, TRUE, 0);

GtkWidget *label;

data->knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf("Дата начала периода"));
gtk_box_pack_start (GTK_BOX (hbox1), data->knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data->knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(taxi_oth_v_v_e_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data->knopka_enter[E_DATAN],iceb_u_toutf("Выбор даты в календаре"),NULL);

//GtkWidget *label=gtk_label_new(iceb_u_toutf("Дата начала периода"));
//gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);

data->entry[E_DATAN] = gtk_entry_new_with_max_length (11);
data->entry[E_VREMN] = gtk_entry_new_with_max_length (9);
gtk_box_pack_start (GTK_BOX (hbox1), data->entry[E_DATAN], TRUE, TRUE, 0);

label=gtk_label_new(iceb_u_toutf("Время начала периода"));
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data->entry[E_VREMN], TRUE, TRUE, 0);

gtk_signal_connect(GTK_OBJECT (data->entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->datn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_DATAN]),(gpointer)E_DATAN);

gtk_signal_connect(GTK_OBJECT (data->entry[E_VREMN]), "activate",GTK_SIGNAL_FUNC(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMN]),data->vremn.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_VREMN]),(gpointer)E_VREMN);

data->knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf("Дата конца периода"));
gtk_box_pack_start (GTK_BOX (hbox2), data->knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data->knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(taxi_oth_v_v_e_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data->knopka_enter[E_DATAK],iceb_u_toutf("Выбор даты в календаре"),NULL);
//label=gtk_label_new(iceb_u_toutf("Дата конца периода"));
//gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);

data->entry[E_DATAK] = gtk_entry_new_with_max_length (11);
data->entry[E_VREMK] = gtk_entry_new_with_max_length (9);
gtk_box_pack_start (GTK_BOX (hbox2), data->entry[E_DATAK], TRUE, TRUE, 0);
label=gtk_label_new(iceb_u_toutf("Время конца периода"));
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data->entry[E_VREMK], TRUE, TRUE, 0);

gtk_signal_connect(GTK_OBJECT (data->entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->datk.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_DATAK]),(gpointer)E_DATAK);

gtk_signal_connect(GTK_OBJECT (data->entry[E_VREMK]), "activate",GTK_SIGNAL_FUNC(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMK]),data->vremk.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_VREMK]),(gpointer)E_VREMK);

data->knopka_enter[E_KODVOD]=gtk_button_new_with_label(iceb_u_toutf("Код водителя (,,)"));
gtk_box_pack_start (GTK_BOX (hbox3), data->knopka_enter[E_KODVOD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data->knopka_enter[E_KODVOD]),"clicked",GTK_SIGNAL_FUNC(taxi_oth_v_v_e_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_enter[E_KODVOD]),(gpointer)E_KODVOD);
tooltips_enter[E_KODVOD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODVOD],data->knopka_enter[E_KODVOD],iceb_u_toutf("Выбор кода водителя в списке"),NULL);

//label=gtk_label_new(iceb_u_toutf("Код водителя (,,)"));
//gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);

data->entry[E_KODVOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox3), data->entry[E_KODVOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data->entry[E_KODVOD]), "activate",GTK_SIGNAL_FUNC(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODVOD]),data->kodvod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_KODVOD]),(gpointer)E_KODVOD);

label=gtk_label_new(iceb_u_toutf("Код оператора (,,)"));
data->entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data->entry[E_KODOP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data->entry[E_KODOP]), "activate",GTK_SIGNAL_FUNC(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->kodop.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_KODOP]),(gpointer)E_KODOP);

data->knopka_enter[E_KODZAV]=gtk_button_new_with_label(iceb_u_toutf("Код завершения (,,)"));
gtk_box_pack_start (GTK_BOX (hbox5), data->knopka_enter[E_KODZAV], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data->knopka_enter[E_KODZAV]),"clicked",GTK_SIGNAL_FUNC(taxi_oth_v_v_e_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_enter[E_KODZAV]),(gpointer)E_KODZAV);
tooltips_enter[E_KODZAV]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODZAV],data->knopka_enter[E_KODZAV],iceb_u_toutf("Выбор кода завершения в списке"),NULL);

data->entry[E_KODZAV] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox5), data->entry[E_KODZAV], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data->entry[E_KODZAV]), "activate",GTK_SIGNAL_FUNC(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODZAV]),data->kodzav.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_KODZAV]),(gpointer)E_KODZAV);


data->knopka_enter[E_KODKLIENTA]=gtk_button_new_with_label(iceb_u_toutf("Код клиента (,,)"));
gtk_box_pack_start (GTK_BOX (hbox5), data->knopka_enter[E_KODKLIENTA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data->knopka_enter[E_KODKLIENTA]),"clicked",GTK_SIGNAL_FUNC(taxi_oth_v_v_e_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(data->knopka_enter[E_KODKLIENTA]),(gpointer)E_KODKLIENTA);
tooltips_enter[E_KODKLIENTA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODKLIENTA],data->knopka_enter[E_KODKLIENTA],iceb_u_toutf("Выбор клиента в списке клиентов"),NULL);


data->entry[E_KODKLIENTA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox5), data->entry[E_KODKLIENTA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data->entry[E_KODKLIENTA]), "activate",GTK_SIGNAL_FUNC(taxi_oth_v_vvod),data);
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKLIENTA]),data->kodklienta.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data->entry[E_KODKLIENTA]),(gpointer)E_KODKLIENTA);


GtkWidget *knopka[KOL_PFK];
GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",iceb_u_toutf("Расчет"));
knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],knopka[PFK2],iceb_u_toutf("Начать расчет отчета"),NULL);
gtk_signal_connect(GTK_OBJECT(knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(taxi_oth_v_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F4 %s",iceb_u_toutf("Очистить"));
knopka[PFK4]=gtk_button_new_with_label(strsql);
tooltips[PFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK4],knopka[PFK4],iceb_u_toutf("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(knopka[PFK4]),"clicked",GTK_SIGNAL_FUNC(taxi_oth_v_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(knopka[PFK4]),(gpointer)PFK4);

sprintf(strsql,"F10 %s",iceb_u_toutf("Выход"));
knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],knopka[PFK10],iceb_u_toutf("Выйти из меню без получения отчета"),NULL);
gtk_signal_connect(GTK_OBJECT(knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(taxi_oth_v_knopka),data);
gtk_object_set_user_data(GTK_OBJECT(knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), knopka[PFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), knopka[PFK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), knopka[PFK10], TRUE, TRUE, 0);


gtk_widget_grab_focus(data->entry[0]);

gtk_widget_show_all(data->window);
gtk_main();

return(data->metka_voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  taxi_oth_v_v_e_knopka(GtkWidget *widget,class taxi_oth_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->datn,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->datn.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->datk,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->datk.ravno());
      
    return;  

  case E_KODKLIENTA:
    if(l_taxisk(1,&kod,&naim,data->window) == 0)
     {
      data->kodklienta.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKLIENTA]),data->kodklienta.ravno_toutf());
     }
    return;  

  case E_KODVOD:
    if(l_vod(1,&kod,&naim,data->window) == 0)
     {
      data->kodvod.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODVOD]),data->kodvod.ravno_toutf());
     }
    return;  

  case E_KODZAV:
    if(l_taxikz(1,&kod,&naim,data->window) == 0)
     {
      data->kodzav.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODZAV]),data->kodzav.ravno_toutf());
     }
    return;  

 }
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  taxi_oth_v_knopka(GtkWidget *widget,class taxi_oth_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case PFK2:
    taxi_oth_prov(data);
    data->metka_voz=1;
    return;  

  case PFK4:
    taxi_oth_clear(data);
    return;
   
  case PFK10:
    gtk_widget_destroy(data->window);
    data->metka_voz=0;
    return;
 }
}
 
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   taxi_oth_v_key_press(GtkWidget *widget,GdkEventKey *event,class taxi_oth_data *data)
{
//char  bros[300];

//printf("vzaktaxi_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    taxi_oth_prov(data);
    data->metka_voz=1;
    return(TRUE);
    
  case GDK_F4:
    taxi_oth_clear(data);
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_widget_destroy(data->window);
    data->metka_voz=0;

    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

//    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    taxi_oth_v_vvod(GtkWidget *widget,class taxi_oth_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kontr_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->datn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VREMN:
    data->vremn.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->datk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VREMK:
    data->vremk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODVOD:
    data->kodvod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODOP:
    data->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODZAV:
    data->kodzav.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODKLIENTA:
    data->kodklienta.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/************************************/
/*Чтение введенной информации и проверка на ошибки*/
/**************************************************/
//Если вернули 0 - все впорядке

int    taxi_oth_prov(class taxi_oth_data *data)
{
for(int i=0; i < KOLENTER; i++)
  gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");

short d,m,g;

if(iceb_u_rsdat(&d,&m,&g,data->datn.ravno(),1) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не верно введена дата начала !"));
  iceb_menu_soob(&repl,NULL);
  return(1);
 }

if(iceb_u_rsdat(&d,&m,&g,data->datk.ravno(),1) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не верно введена дата конца !"));
  iceb_menu_soob(&repl,NULL);
  return(1);
 }

if(iceb_u_rstime(&d,&m,&g,data->vremn.ravno()) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не верно ввели время начала !"));
  iceb_menu_soob(&repl,NULL);
  return(1);
 }

if(iceb_u_rstime(&d,&m,&g,data->vremk.ravno()) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не верно ввели время конца !"));
  iceb_menu_soob(&repl,NULL);
  return(1);
 }

gtk_widget_destroy(data->window);

return(0);

}
/****************************/
/*Очистка меню              */
/*****************************/

void taxi_oth_clear(class taxi_oth_data *data)
{

data->clear_zero();

gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->datn.ravno_toutf());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->datk.ravno_toutf());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMN]),data->vremn.ravno_toutf());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_VREMK]),data->vremk.ravno_toutf());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->kodop.ravno_toutf());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODVOD]),data->kodvod.ravno_toutf());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODKLIENTA]),data->kodklienta.ravno_toutf());
gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODZAV]),data->kodzav.ravno_toutf());

gtk_widget_grab_focus(data->entry[0]);

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  taxi_oth_v_radio0(GtkWidget *widget,class taxi_oth_data *data)
{
//g_print("taxi_oth_v_radio0\n");
/*
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("taxi_oth_v_radio knop=%s\n",knop);

data->metkarr=atoi(knop);
*/
if(data->metkarr == 0)
  return;
data->metkarr=0;

}
void  taxi_oth_v_radio1(GtkWidget *widget,class taxi_oth_data *data)
{
//g_print("taxi_oth_v_radio1\n");
if(data->metkarr == 1)
  return;
data->metkarr=1;
}
