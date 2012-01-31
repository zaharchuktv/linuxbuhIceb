/*$Id: eks_ko_v.c,v 1.5 2011-02-21 07:35:51 sasa Exp $*/
/*15.09.2009	20.10.2006	Белых А.И.	eks_ko_v.c
Ввод реквизитов для ескпорта кассовых ордеров в подсистему "Учёт кассовых ордеров".
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "../headers/buhg_g.h"
#include "zar_eks_ko.h"
enum
 {
  E_KASSA,
  E_DATA,
  E_KODOP,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class eks_ko_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *radiobutton1[2];
  GtkWidget *radiobutton2[2];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zar_eks_ko_rek *rk;
  
  eks_ko_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->kassa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KASSA]))));
    rk->data.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA]))));
    rk->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP]))));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[0])) == TRUE)
     rk->metka_r=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[1])) == TRUE)
     rk->metka_r=1;
   }
 };


gboolean   eks_ko_v_key_press(GtkWidget *widget,GdkEventKey *event,class eks_ko_v_data *data);
void  eks_ko_v_knopka(GtkWidget *widget,class eks_ko_v_data *data);
void    eks_ko_v_vvod(GtkWidget *widget,class eks_ko_v_data *data);
void  eks_ko_v_e_knopka(GtkWidget *widget,class eks_ko_v_data *data);

int eks_ko_v_vibor_pr();

int eks_ko_prov(class eks_ko_v_data *data);

extern SQL_baza  bd;
extern char      *name_system;
extern char *organ;

int eks_ko_v(class zar_eks_ko_rek *rek,GtkWidget *wpredok)
{
class eks_ko_v_data data;
char strsql[512];
data.rk=rek;
 



if((rek->prn=eks_ko_v_vibor_pr()) < 0)
 return(1);

if(rek->prn == 0)
  rek->prn=2;
else
  rek->prn=1;









data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",name_system,gettext("Экспорт кассовых оредров в подсистему \"Учёт кассовых ордеров\""));
label=gtk_label_new(gettext("Экспорт кассовых оредров в подсистему \"Учёт кассовых ордеров\""));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(eks_ko_v_key_press),&data);

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
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 1);
// gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_hseparator_new();
GtkWidget *separator3=gtk_hseparator_new();
gtk_box_pack_start (GTK_BOX (vbox), separator1, TRUE, TRUE, 1);
//gtk_container_add (GTK_CONTAINER (vbox), separator1);

data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Для каждого документа отдельный кассовый ордер"));
data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),gettext("Все документны в один кассовый ордер"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[1], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk->metka_r]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox), separator3, TRUE, TRUE, 1);
//gtk_container_add (GTK_CONTAINER (vbox), separator3);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 1);
//gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Код кассы"));
data.knopka_enter[E_KASSA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.knopka_enter[E_KASSA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KASSA]),"clicked",GTK_SIGNAL_FUNC(eks_ko_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KASSA]),(gpointer)E_KASSA);
tooltips_enter[E_KASSA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KASSA],data.knopka_enter[E_KASSA],gettext("Выбор кассы"),NULL);

data.entry[E_KASSA] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.entry[E_KASSA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KASSA]), "activate",GTK_SIGNAL_FUNC(eks_ko_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KASSA]),data.rk->kassa.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KASSA]),(gpointer)E_KASSA);

sprintf(strsql,"%s",gettext("Дата"));
data.knopka_enter[E_DATA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.knopka_enter[E_DATA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA]),"clicked",GTK_SIGNAL_FUNC(eks_ko_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA]),(gpointer)E_DATA);
tooltips_enter[E_DATA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA],data.knopka_enter[E_DATA],gettext("Выбор даты"),NULL);

data.entry[E_DATA] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA]), "activate",GTK_SIGNAL_FUNC(eks_ko_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.rk->data.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA]),(gpointer)E_DATA);


sprintf(strsql,"%s",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODOP]),"clicked",GTK_SIGNAL_FUNC(eks_ko_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODOP]),(gpointer)E_KODOP);
tooltips_enter[E_KODOP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODOP],data.knopka_enter[E_KODOP],gettext("Выбор операции"),NULL);

data.entry[E_KODOP] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODOP]), "activate",GTK_SIGNAL_FUNC(eks_ko_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODOP]),(gpointer)E_KODOP);

GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Экспорт"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Запись введеной в меню информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(eks_ko_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Завершение работы с меню без записи введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(eks_ko_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE, 0);

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
void  eks_ko_v_e_knopka(GtkWidget *widget,class eks_ko_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("eks_ko_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATA:

    if(iceb_calendar(&data->rk->data,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA]),data->rk->data.ravno());
      
    return;  

  case E_KASSA:

    if(l_spis_kas(1,&kod,&naim,data->window) == 0)
      data->rk->kassa.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KASSA]),data->rk->kassa.ravno());
      
    return;  

  case E_KODOP:
    if(data->rk->prn == 1)
     if(l_kasopp(1,&kod,&naim,data->window) == 0)
       data->rk->kodop.new_plus(kod.ravno());
    if(data->rk->prn == 2)
     if(l_kasopr(1,&kod,&naim,data->window) == 0)
       data->rk->kodop.new_plus(kod.ravno());

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno_toutf());
      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   eks_ko_v_key_press(GtkWidget *widget,GdkEventKey *event,class eks_ko_v_data *data)
{
//char  bros[512];

//printf("eks_ko_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK2]),"clicked");

    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK10]),"clicked");

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
void  eks_ko_v_knopka(GtkWidget *widget,class eks_ko_v_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("eks_ko_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
  
  if(eks_ko_prov(data) != 0)
   return;
  data->voz=0;
    gtk_widget_destroy(data->window);
          
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    eks_ko_v_vvod(GtkWidget *widget,class eks_ko_v_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("eks_ko_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KASSA:
    data->rk->kassa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATA:
    data->rk->data.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KODOP:
    data->rk->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/*******************************/
/*Проверка правильности ввода*/
/****************************/
int eks_ko_prov(class eks_ko_v_data *data)
{
char strsql[512];
//проверка кода кассы
sprintf(strsql,"select naik from Kas where kod=%d",data->rk->kassa.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код кассы"),data->rk->kassa.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
//проверка даты
if(data->rk->data.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата !"),data->window);
  return(1);
  
 }
SQL_str row;
class SQLCURSOR	cur;
//проверка кода операции
if(data->rk->prn == 1)
  sprintf(strsql,"select naik,shetkor from Kasop1 where kod='%s'",data->rk->kodop.ravno());
if(data->rk->prn == 2)
  sprintf(strsql,"select naik,shetkor from Kasop2 where kod='%s'",data->rk->kodop.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data->rk->kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
data->rk->naimop.new_plus(row[0]);
data->rk->shetk.new_plus(row[1]);
return(0);
}
/***********************/
/*Выбор типа документа*/
/***********************/
int eks_ko_v_vibor_pr()
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(organ,40);
zagolovok.ps_plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Экспортировать кассовые ордера для приходных документов"));//0
punkt_m.plus(gettext("Экспортировать кассовые ордера для расходных документов"));//1


int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL));

}
