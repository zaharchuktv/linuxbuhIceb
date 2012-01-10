/*$Id: prov_vv.c,v 1.8 2011-02-21 07:36:21 sasa Exp $*/
/*07.01.2007	07.01.2007	Белых А.И.	prov_vv.c
Проверка возможности входа-выхода в подразделение.
*/
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "i_rest.h"
enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KOD_KART,
  E_PODR,
  KOLENTER  
 };

class prov_vv_data
 {
  public:
  //Реквизиты записи
  class iceb_u_str kod_kart;
  class iceb_u_str podr;
  int vhod_vihod; //0-проверка входа 1-проверка выхода


  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_naimpodr;
  GtkWidget *radiobutton2[2];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  
  prov_vv_data() //Конструктор
   {
    kl_shift=0;
    voz=0;
    clear_rek_zap();
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[0])) == TRUE)
     vhod_vihod=0;
    else
     vhod_vihod=1;
   }
  void clear_rek_zap()
   {
    kod_kart.new_plus("");
    podr.new_plus("");
    vhod_vihod=0;
   }
 };

gboolean   prov_vv_v_key_press(GtkWidget *widget,GdkEventKey *event,class prov_vv_data *data);
void    prov_vv_v_vvod(GtkWidget *widget,class prov_vv_data *data);
void  prov_vv_v_knopka(GtkWidget *widget,class prov_vv_data *data);
void uslopp_kkvt(char *skod,char *nkod,GtkWidget *wpredok);

void  prov_vv_v_e_knopka(GtkWidget *widget,class prov_vv_data *data);

void  prov_vv_r(int podr,int nomer_tur,const char *kod_kart,int vhod_vihod,GtkWidget *wpredok);
int prov_vv_zr(class prov_vv_data *data);

extern char *name_system;
extern SQL_baza bd;
extern char	*shrt;    /*Счета розничной торговли*/
extern double	okrcn;  /*Округление цены*/

void prov_vv(const char *kod_kart,GtkWidget *wpredok)
{
prov_vv_data data;
iceb_u_str naim_podr;
naim_podr.plus("");
char strsql[300];

data.kod_kart.new_plus(kod_kart);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Проверка возможности входа/выхода в подразделение"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(prov_vv_v_key_press),&data);

iceb_u_str zagolov;
zagolov.new_plus(gettext("Проверка возможности входа/выхода в подразделение"));

GtkWidget *label=gtk_label_new(zagolov.ravno_toutf());

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 1);

GtkWidget *separator1=gtk_hseparator_new();
GtkWidget *separator3=gtk_hseparator_new();
gtk_box_pack_start (GTK_BOX (vbox),separator1, TRUE, TRUE, 2);

//Вставляем радиокнопки

data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Проверка возможности входа"));
data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),gettext("Проверка возможности выхода"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[1], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.vhod_vihod]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox),separator3, TRUE, TRUE, 2);


gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 1);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Код клиента"));
data.knopka_enter[E_KOD_KART]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KART]), data.knopka_enter[E_KOD_KART], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_KART]),"clicked",GTK_SIGNAL_FUNC(prov_vv_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_KART]),(gpointer)E_KOD_KART);
tooltips_enter[E_KOD_KART]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_KART],data.knopka_enter[E_KOD_KART],gettext("Выбор подразделения"),NULL);

data.entry[E_KOD_KART] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KART]), data.entry[E_KOD_KART],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_KART]), "activate",GTK_SIGNAL_FUNC(prov_vv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_KART]),data.kod_kart.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_KART]),(gpointer)E_KOD_KART);

sprintf(strsql,"%s",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(prov_vv_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения."),NULL);

data.entry[E_PODR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR],TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(prov_vv_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);

data.label_naimpodr=gtk_label_new(naim_podr.ravno_toutf());
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.label_naimpodr,TRUE, TRUE, 0);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Проверка"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Проверка возможности входа/выхода в подразделение"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(prov_vv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне."),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(prov_vv_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

if(data.kod_kart.getdlinna() <= 1)
  gtk_widget_grab_focus(data.entry[0]);
else
  gtk_widget_grab_focus(data.entry[1]);
gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
 
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  prov_vv_v_e_knopka(GtkWidget *widget,class prov_vv_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.plus("");
naim.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("prov_vv_v_e_knopka knop=%d\n",knop);

switch ((gint)knop)
 {

  case E_KOD_KART:
    iceb_mous_klav(gettext("Введите код клиента"),&data->kod_kart,30,1,0,1,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_KART]),data->kod_kart.ravno_toutf());
    return;
    
  case E_PODR:
    if(rpod_l(1,&kod,&naim,data->window) == 0)
     {
      data->podr.new_plus(kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naimpodr),naim.ravno_toutf(20));
     }    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->podr.ravno_toutf());
    return;

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   prov_vv_v_key_press(GtkWidget *widget,GdkEventKey *event,class prov_vv_data *data)
{

//printf("prov_vv_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
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
void  prov_vv_v_knopka(GtkWidget *widget,class prov_vv_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("prov_vv_v_knopka knop=%d\n",knop);

switch ((gint)knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    prov_vv_zr(data);

    return;  

    
  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    prov_vv_v_vvod(GtkWidget *widget,class prov_vv_data *data)
{
char strsql[300];
SQL_str row;
SQLCURSOR cur;
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("prov_vv_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD_KART:
    data->kod_kart.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_PODR:
    data->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

    sprintf(strsql,"select naik from Restpod where kod=%d",data->podr.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.20s",row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naimpodr),iceb_u_toutf(strsql));
     }    
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**********************/
/*Проверка*/
/************************/

int prov_vv_zr(class prov_vv_data *data)
{

if(data->kod_kart.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели код клиента !"),data->window);
  return(1);
 }
if(data->podr.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не ввели код подразделения !"),data->window);
  return(1);
 }

//проверяем код карточки
char strsql[300];
sprintf(strsql,"select kod from Taxiklient where kod='%s'",data->kod_kart.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не нашли код клиента !"),data->kod_kart.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

//проверяем код подразделения
sprintf(strsql,"select kod from Restpod where kod=%d",data->podr.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не нашли код подразделения !"),data->podr.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

SQL_str row;
class SQLCURSOR cur;

//Читаем номер любого турникета для этого подразделения
sprintf(strsql,"select nm from Resst where podr=%d",data->podr.ravno_atoi());

if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 0)
 {
  iceb_menu_soob(gettext("Для этого подразделения не введено ни одного турникета !"),data->window);
  return(1);
 }
int nomer_tur=atoi(row[0]);


prov_vv_r(data->podr.ravno_atoi(),nomer_tur,data->kod_kart.ravno(),data->vhod_vihod+1,data->window);

return(0);
}

