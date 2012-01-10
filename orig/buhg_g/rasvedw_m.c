/*$Id: rasvedw_m.c,v 1.16 2011-02-21 07:35:57 sasa Exp $*/
/*23.11.2006	31.06.2006	Белых А.И.	rasvedw_m.c
Меню для распечатки ведомости на заработную плату
*/
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"
#include "rasvedw.h"
enum
 {
  E_DATA,
  E_TABNOM,
  E_SHET,
  E_KOD_NAH,
  E_PODR,
  E_KOD_KAT,
  E_KOD_GRUP_POD,  
  E_PROC_VIP,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class rasvedw_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton2[2];
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class rasvedw_rek *rk;
  
  rasvedw_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->data.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA]))));
    rk->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PODR]))));
    rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    rk->kod_nah.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_NAH]))));
    rk->kod_kat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_KAT]))));
    rk->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM]))));
    rk->proc_vip.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PROC_VIP]))));
    rk->kod_grup_pod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_GRUP_POD]))));
   }
  
  void clear_rek()
   {

    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
    gtk_entry_set_text(GTK_ENTRY(entry[E_PROC_VIP]),rk->proc_vip.ravno_toutf());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2[rk->metka_saldo]),TRUE); //Устанавливем активной кнопку

   }
 };


gboolean   rasvedw_m_key_press(GtkWidget *widget,GdkEventKey *event,class rasvedw_m_data *data);
void  rasvedw_m_knopka(GtkWidget *widget,class rasvedw_m_data *data);
void    rasvedw_m_vvod(GtkWidget *widget,class rasvedw_m_data *data);

void  rasvedw_radio2_0(GtkWidget *widget,class rasvedw_m_data *data);
void  rasvedw_radio2_1(GtkWidget *widget,class rasvedw_m_data *data);

void  rasvedw_m_e_knopka(GtkWidget *widget,class rasvedw_m_data *data);

extern SQL_baza  bd;
extern char      *name_system;

int rasvedw_m(class rasvedw_rek *rek,GtkWidget *wpredok)
{

class rasvedw_m_data data;
char strsql[512];
iceb_u_str kikz;
data.rk=rek;
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",name_system,gettext("Распечатать ведомость заработной платы"));//0
label=gtk_label_new(gettext("Распечатать ведомость заработной платы"));//0

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rasvedw_m_key_press),&data);

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
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_hseparator_new();
GtkWidget *separator3=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator1);
//Вставляем радиокнопки
GSList *group;

data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("С учётом сальдо"));
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[0]), "clicked",GTK_SIGNAL_FUNC(rasvedw_radio2_0),&data);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));

data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Без учёта сальдо"));
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[1]), "clicked",GTK_SIGNAL_FUNC(rasvedw_radio2_1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk->metka_saldo]),TRUE); //Устанавливем активной кнопку

gtk_container_add (GTK_CONTAINER (vbox), separator3);


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];


sprintf(strsql,"%s %s",gettext("Дата"),gettext("(м.г)"));
data.knopka_enter[E_DATA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.knopka_enter[E_DATA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA]),"clicked",GTK_SIGNAL_FUNC(rasvedw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA]),(gpointer)E_DATA);
tooltips_enter[E_DATA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA],data.knopka_enter[E_DATA],gettext("Выбор даты"),NULL);

data.entry[E_DATA] = gtk_entry_new_with_max_length (7);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA]), "activate",GTK_SIGNAL_FUNC(rasvedw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.rk->data.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA]),(gpointer)E_DATA);


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_TABNOM]),"clicked",GTK_SIGNAL_FUNC(rasvedw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_TABNOM]),(gpointer)E_TABNOM);
tooltips_enter[E_TABNOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_TABNOM],data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"),NULL);

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TABNOM]), "activate",GTK_SIGNAL_FUNC(rasvedw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.rk->tabnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TABNOM]),(gpointer)E_TABNOM);


sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(rasvedw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(rasvedw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);


sprintf(strsql,"%s (,,)",gettext("Код начисления"));
data.knopka_enter[E_KOD_NAH]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NAH]), data.knopka_enter[E_KOD_NAH], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_NAH]),"clicked",GTK_SIGNAL_FUNC(rasvedw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_NAH]),(gpointer)E_KOD_NAH);
tooltips_enter[E_KOD_NAH]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_NAH],data.knopka_enter[E_KOD_NAH],gettext("Выбор кода начисления"),NULL);

data.entry[E_KOD_NAH] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NAH]), data.entry[E_KOD_NAH], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_NAH]), "activate",GTK_SIGNAL_FUNC(rasvedw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_NAH]),data.rk->kod_nah.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_NAH]),(gpointer)E_KOD_NAH);


sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(rasvedw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(rasvedw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);


sprintf(strsql,"%s (,,)",gettext("Код группы подразделения"));
data.knopka_enter[E_KOD_GRUP_POD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GRUP_POD]), data.knopka_enter[E_KOD_GRUP_POD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_GRUP_POD]),"clicked",GTK_SIGNAL_FUNC(rasvedw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_GRUP_POD]),(gpointer)E_KOD_GRUP_POD);
tooltips_enter[E_KOD_GRUP_POD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_GRUP_POD],data.knopka_enter[E_KOD_GRUP_POD],gettext("Выбор группы"),NULL);

data.entry[E_KOD_GRUP_POD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GRUP_POD]), data.entry[E_KOD_GRUP_POD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_GRUP_POD]), "activate",GTK_SIGNAL_FUNC(rasvedw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_GRUP_POD]),data.rk->kod_grup_pod.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_GRUP_POD]),(gpointer)E_KOD_GRUP_POD);


sprintf(strsql,"%s (,,)",gettext("Код категории"));
data.knopka_enter[E_KOD_KAT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KAT]), data.knopka_enter[E_KOD_KAT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_KAT]),"clicked",GTK_SIGNAL_FUNC(rasvedw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_KAT]),(gpointer)E_KOD_KAT);
tooltips_enter[E_KOD_KAT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_KAT],data.knopka_enter[E_KOD_KAT],gettext("Выбор категории"),NULL);

data.entry[E_KOD_KAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KAT]), data.entry[E_KOD_KAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_KAT]), "activate",GTK_SIGNAL_FUNC(rasvedw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_KAT]),data.rk->kod_kat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_KAT]),(gpointer)E_KOD_KAT);


label=gtk_label_new(gettext("Процент выплаты"));
gtk_box_pack_start (GTK_BOX (hbox[E_PROC_VIP]), label, FALSE, FALSE, 0);

data.entry[E_PROC_VIP] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC_VIP]), data.entry[E_PROC_VIP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PROC_VIP]), "activate",GTK_SIGNAL_FUNC(rasvedw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROC_VIP]),data.rk->proc_vip.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PROC_VIP]),(gpointer)E_PROC_VIP);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчёт"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(rasvedw_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(rasvedw_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(rasvedw_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/**************************/

void  rasvedw_radio2_0(GtkWidget *widget,class rasvedw_m_data *data)
{
//g_print("rasvedw_radio0\n");

data->rk->metka_saldo=0;
//printf("prov=0\n");

}
void  rasvedw_radio2_1(GtkWidget *widget,class rasvedw_m_data *data)
{
//g_print("rasvedw_radio1\n");
data->rk->metka_saldo=1;
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  rasvedw_m_e_knopka(GtkWidget *widget,class rasvedw_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {

  case E_DATA:
    iceb_calendar1(&data->rk->data,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA]),data->rk->data.ravno_toutf());
    return;
   
  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->rk->tabnom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->rk->tabnom.ravno_toutf());
    return;
  
  case E_KOD_NAH:
    if(l_zarnah(1,&kod,&naim,data->window) == 0)
     data->rk->kod_nah.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_NAH]),data->rk->kod_nah.ravno_toutf());
    return;
  
  case E_PODR:
    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     data->rk->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno_toutf());
    return;
  
  case E_KOD_GRUP_POD:
    if(l_gruppod(1,&kod,&naim,data->window) == 0)
     data->rk->kod_grup_pod.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_GRUP_POD]),data->rk->kod_grup_pod.ravno_toutf());
    return;

  case E_KOD_KAT:
    if(l_zarkateg(1,&kod,&naim,data->window) == 0)
     data->rk->kod_kat.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_KAT]),data->rk->kod_kat.ravno_toutf());
    return;
    
  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno_toutf());

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasvedw_m_key_press(GtkWidget *widget,GdkEventKey *event,class rasvedw_m_data *data)
{
//char  bros[512];

//printf("rasvedw_m_key_press\n");
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
void  rasvedw_m_knopka(GtkWidget *widget,class rasvedw_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("rasvedw_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(data->rk->data.prov_dat1() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата !"),data->window);
      return;
     }
    data->voz=0;
    gtk_widget_destroy(data->window);
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

void    rasvedw_m_vvod(GtkWidget *widget,class rasvedw_m_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("rasvedw_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATA:
    data->rk->data.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_TABNOM:
    data->rk->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_SHET:
    data->rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PODR:
    data->rk->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOD_NAH:
    data->rk->kod_nah.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_KOD_GRUP_POD:
    data->rk->kod_grup_pod.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PROC_VIP:
    data->rk->proc_vip.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
