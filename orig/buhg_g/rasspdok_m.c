/*$Id: rasspdok_m.c,v 1.18 2011-02-21 07:35:56 sasa Exp $*/
/*23.02.2009	31.01.2005	Белых А.И.	rasspdok_m.c
Меню для получения отчета - список документов
*/
#include <stdlib.h>
#include "buhg_g.h"
#include "rasspdok.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAN,
  E_DATAK,
  E_KODOP,
  E_KONTR,
  E_SKLAD,
  E_GRUPK,
  KOLENTER  
 };

class rasspdok_data
 {
  public:
  rasspdok_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет
  GtkWidget *radiobutton[2];
  GtkWidget *radiobutton1[3];
  GtkWidget *radiobutton2[3];

  rasspdok_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
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
   }
 };


void       rasspdok_radio0(GtkWidget *,class rasspdok_data *);
void       rasspdok_radio1(GtkWidget *,class rasspdok_data *);

void  rasspdok_radio01(GtkWidget *widget,class rasspdok_data *data);
void  rasspdok_radio11(GtkWidget *widget,class rasspdok_data *data);
void  rasspdok_radio21(GtkWidget *widget,class rasspdok_data *data);

void  rasspdok_radio02(GtkWidget *widget,class rasspdok_data *data);
void  rasspdok_radio12(GtkWidget *widget,class rasspdok_data *data);
void  rasspdok_radio22(GtkWidget *widget,class rasspdok_data *data);

gboolean   rasspdok_v_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdok_data *data);
void    rasspdok_v_vvod(GtkWidget *widget,class rasspdok_data *data);
void  rasspdok_v_knopka(GtkWidget *widget,class rasspdok_data *data);
void  rasspdok_v_e_knopka(GtkWidget *widget,class rasspdok_data *data);


extern char *name_system;
extern SQL_baza bd;

int   rasspdok_m(class rasspdok_rr *rek_ras)
{
rasspdok_data data;

data.rk=rek_ras;

if(data.rk->datan.getdlinna() == 0)
  data.rk->clear_data();
  
char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать список документов"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(rasspdok_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Распечатать список документов"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

//GtkWidget *hboxradio = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator1);

//Вставляем радиокнопки сортировки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Сортировка по датам"));
if(data.rk->metka_sort == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[0]), "clicked",GTK_SIGNAL_FUNC(rasspdok_radio0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

data.radiobutton[1]=gtk_radio_button_new_with_label(group,gettext("Сортировка по номерам документов"));
if(data.rk->metka_sort == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[1]), "clicked",GTK_SIGNAL_FUNC(rasspdok_radio1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[1], TRUE, TRUE, 0);

GtkWidget *separator2=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator2);

//Вставляем радиокнопки видов документов

data.radiobutton1[0]=gtk_radio_button_new_with_label(NULL,gettext("Только подтверждённые документы"));
if(data.rk->metka_dok == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton1[0]), "clicked",GTK_SIGNAL_FUNC(rasspdok_radio01),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton10),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton1[0]));

data.radiobutton1[1]=gtk_radio_button_new_with_label(group,gettext("Только неподтверждённые документы"));
if(data.rk->metka_dok == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton1[1]), "clicked",GTK_SIGNAL_FUNC(rasspdok_radio11),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[1], TRUE, TRUE, 0);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton1[1]));

data.radiobutton1[2]=gtk_radio_button_new_with_label(group,gettext("Все документы"));
if(data.rk->metka_dok == 2)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[2]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton1[2]), "clicked",GTK_SIGNAL_FUNC(rasspdok_radio21),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[2], TRUE, TRUE, 0);

GtkWidget *separator3=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator3);

//Вставляем радиокнопки приходов-расходов

data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("Все документы"));
if(data.rk->metka_pr == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[0]), "clicked",GTK_SIGNAL_FUNC(rasspdok_radio02),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton20),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));

data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Только приходные документы"));
if(data.rk->metka_pr == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[1]), "clicked",GTK_SIGNAL_FUNC(rasspdok_radio12),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton2[1]));

data.radiobutton2[2]=gtk_radio_button_new_with_label(group,gettext("Только расходные документы"));
if(data.rk->metka_pr == 2)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[2]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[2]), "clicked",GTK_SIGNAL_FUNC(rasspdok_radio22),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[2], TRUE, TRUE, 0);

GtkWidget *separator4=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator4);


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(rasspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(rasspdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(rasspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(rasspdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);


sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODOP]),"clicked",GTK_SIGNAL_FUNC(rasspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODOP]),(gpointer)E_KODOP);

data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODOP]), "activate",GTK_SIGNAL_FUNC(rasspdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODOP]),(gpointer)E_KODOP);

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KONTR]),"clicked",GTK_SIGNAL_FUNC(rasspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KONTR]),(gpointer)E_KONTR);

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(rasspdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk->kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);

sprintf(strsql,"%s (,,)",gettext("Склад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKLAD]),"clicked",GTK_SIGNAL_FUNC(rasspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKLAD]),(gpointer)E_SKLAD);

data.entry[E_SKLAD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKLAD]), "activate",GTK_SIGNAL_FUNC(rasspdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKLAD]),(gpointer)E_SKLAD);

sprintf(strsql,"%s (,,)",gettext("Код группи контрагента"));
data.knopka_enter[E_GRUPK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPK]), data.knopka_enter[E_GRUPK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUPK]),"clicked",GTK_SIGNAL_FUNC(rasspdok_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUPK]),(gpointer)E_GRUPK);

data.entry[E_GRUPK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPK]), data.entry[E_GRUPK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUPK]), "activate",GTK_SIGNAL_FUNC(rasspdok_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPK]),data.rk->grupk.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUPK]),(gpointer)E_GRUPK);



GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(rasspdok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

/*********************
sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр нужных реквизитов"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(rasspdok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);
**********************/

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(rasspdok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(rasspdok_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  rasspdok_v_e_knopka(GtkWidget *widget,class rasspdok_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
      
    return;  

  case E_KODOP:
    vib_kodop_mu(&data->rk->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno_toutf());
    return;  

  case E_KONTR:
    iceb_vibrek(0,"Kontragent",&data->rk->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk->kontr.ravno_toutf());
    return;

  case E_SKLAD:
    iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno_toutf());
    return;

  case E_GRUPK:
    iceb_vibrek(0,"Gkont",&data->rk->grupk,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPK]),data->rk->grupk.ravno_toutf());
    return;
 }
}
/******************************/
/*Обработчик нажатия радиокнопок сортировки*/
/**********************************/

void  rasspdok_radio0(GtkWidget *widget,class rasspdok_data *data)
{
//g_print("rasspdok_radio0\n");

if(data->rk->metka_sort == 0)
  return;
data->rk->metka_sort=0;

}
void  rasspdok_radio1(GtkWidget *widget,class rasspdok_data *data)
{
//g_print("rasspdok_radio1\n");
if(data->rk->metka_sort == 1)
  return;
data->rk->metka_sort=1;
}
/******************************/
/*Обработчик нажатия радиокнопок видов документов подтверждённых-неподтверждённых*/
/**********************************/

void  rasspdok_radio01(GtkWidget *widget,class rasspdok_data *data)
{
//g_print("rasspdok_radio0\n");

if(data->rk->metka_dok == 0)
  return;
data->rk->metka_dok=0;

}
void  rasspdok_radio11(GtkWidget *widget,class rasspdok_data *data)
{
//g_print("rasspdok_radio1\n");
if(data->rk->metka_dok == 1)
  return;
data->rk->metka_dok=1;
}
void  rasspdok_radio21(GtkWidget *widget,class rasspdok_data *data)
{
//g_print("rasspdok_radio1\n");
if(data->rk->metka_dok == 2)
  return;
data->rk->metka_dok=2;
}

/******************************/
/*Обработчик нажатия радиокнопок приходов/расходов*/
/**********************************/

void  rasspdok_radio02(GtkWidget *widget,class rasspdok_data *data)
{
//g_print("rasspdok_radio0\n");

if(data->rk->metka_pr == 0)
  return;
data->rk->metka_pr=0;

}
void  rasspdok_radio12(GtkWidget *widget,class rasspdok_data *data)
{
//g_print("rasspdok_radio1\n");
if(data->rk->metka_pr == 1)
  return;
data->rk->metka_pr=1;
}
void  rasspdok_radio22(GtkWidget *widget,class rasspdok_data *data)
{
//g_print("rasspdok_radio1\n");
if(data->rk->metka_pr == 2)
  return;
data->rk->metka_pr=2;
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasspdok_v_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdok_data *data)
{

//printf("rasspdok_v_key_press\n");
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
void  rasspdok_v_knopka(GtkWidget *widget,class rasspdok_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
//    if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
    if(iceb_rsdatp_str(&data->rk->datan,&data->rk->datak,data->window) != 0)
      return;

    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    rasspdok_v_vvod(GtkWidget *widget,class rasspdok_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("rasspdok_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODOP:
    data->rk->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KONTR:
    data->rk->kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SKLAD:
    data->rk->sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUPK:
    data->rk->grupk.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
