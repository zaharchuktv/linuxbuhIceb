/*$Id: iceb_parol.c,v 1.24 2011-09-07 11:44:22 sasa Exp $*/
/*19.03.2010	11.10.2003	Белых А.И.	iceb_parol.c
Если вернули 0- был введен пароль и он правильный
             1- пароль не правильный
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "iceb_libbuh.h"


int iceb_parol(class iceb_u_str *parol,GtkWidget *wpredok)
{
class iceb_u_spisok text_menu;
return(iceb_parol(2,&text_menu,parol,wpredok));
}
/*************************/
int    iceb_parol(int metkap, //0-проверка по сумме 1-проверка по строке даты 2-не проверять
class iceb_u_spisok *text_menu,
GtkWidget *wpredok) 
{
int kod;
class iceb_u_str parol_voz("");

if((kod=iceb_parol(metkap,text_menu,&parol_voz,wpredok)) == 0)
  return(0);

if(kod == 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Пароль введен неправильно !"));
  iceb_menu_soob(&repl,wpredok);
 }
return(1);

}
/****************************/
int    iceb_parol(int metkap, //0-проверка по сумме 1-проверка по строке даты 2-не проверять
GtkWidget *wpredok) 
{
int kod;
class iceb_u_spisok text_menu;
class iceb_u_str parol("");
if((kod=iceb_parol(metkap,&text_menu,&parol,wpredok)) == 0)
  return(0);

if(kod == 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Пароль введен неправильно !"));
  iceb_menu_soob(&repl,wpredok);
 }
return(1);
}

/**************************************
Ввод и проверка пароля
Если вернули 0- был введен пароль и он правильный
             1- пароль не правильный
            -1- отказались вводить пароль
******************************/



struct iceb_parol_data
 {
  GtkWidget *window;
  GtkWidget *knopka[2];
  GtkWidget *entry;
  char  parol[56];
  int   metkap;  
  int   voz; //0-пароль введен правильно 1-неправильно
 };

void       iceb_parol_knopka(GtkWidget *,struct iceb_parol_data *);
gboolean   iceb_parol_key_press(GtkWidget *,GdkEventKey *,struct iceb_parol_data *);
int        iceb_parol_prov(struct iceb_parol_data *);
void       iceb_parol_get_text(GtkWidget *widget,struct iceb_parol_data *data);
gboolean parol_delete_event(GtkWidget *widget,GdkEvent *event,struct iceb_parol_data *data);

extern char             *name_system;

int iceb_parol(int metkap, //0-проверка по сумме 1-проверка по строке даты 2-не проверять
class iceb_u_spisok *text_menu,
class iceb_u_str *parol_voz,
GtkWidget *wpredok) 
{
char strsql[512];
iceb_parol_data data;

memset(&data,'\0',sizeof(data));
data.metkap=metkap;
data.voz=1;
printf("%s\n",__FUNCTION__);


data.window = gtk_window_new( GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),GTK_WIN_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

gtk_window_set_title(GTK_WINDOW (data.window),name_system);
gtk_signal_connect(GTK_OBJECT (data.window), "delete_event",(GtkSignalFunc) parol_delete_event,&data);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_parol_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);

int kolih_text=text_menu->kolih();

GtkWidget *label[kolih_text+1];

for(int ii=0; ii < kolih_text; ii++)
  label[ii]=gtk_label_new(text_menu->ravno_toutf(ii));
 
label[kolih_text]=gtk_label_new(iceb_u_toutf(gettext("Введите пароль")));

for(int ii=0; ii < kolih_text+1; ii++)
  gtk_box_pack_start (GTK_BOX (vbox), label[ii], TRUE, TRUE, 0);

gtk_box_pack_end(GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

data.entry = gtk_entry_new_with_max_length (sizeof(data.parol)-1);
gtk_signal_connect(GTK_OBJECT(data.entry), "activate",GTK_SIGNAL_FUNC(iceb_parol_get_text),&data);

gtk_box_pack_start (GTK_BOX (vbox), data.entry, TRUE, TRUE, 0);
gtk_entry_set_visibility(GTK_ENTRY(data.entry),FALSE);

sprintf(strsql,"F2 %s",gettext("Ввести"));
data.knopka[0] = gtk_button_new_with_label (iceb_u_toutf(strsql));
gtk_signal_connect(GTK_OBJECT (data.knopka[0]), "clicked",GTK_SIGNAL_FUNC(iceb_parol_knopka),&data);
gtk_box_pack_start (GTK_BOX (hbox), data.knopka[0], TRUE, TRUE, 0);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[0]),(gpointer)2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[1] = gtk_button_new_with_label (iceb_u_toutf(strsql));
gtk_signal_connect(GTK_OBJECT (data.knopka[1]), "clicked",GTK_SIGNAL_FUNC(iceb_parol_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[1]),(gpointer)10);
gtk_box_pack_start (GTK_BOX (hbox), data.knopka[1], TRUE, TRUE, 0);



gtk_widget_show_all(data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

if(data.voz == 0)
 parol_voz->new_plus(data.parol);

return(data.voz);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_parol_knopka(GtkWidget *widget,struct iceb_parol_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("lvoditel_knopka knop=%s\n",knop);

switch (knop)
 {
  case 2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry),"activate");
    return;

  case 10:
    data->voz=-1;
    gtk_widget_destroy(data->window);
    return;
 }
}
 
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_parol_key_press(GtkWidget *widget,GdkEventKey *event,struct iceb_parol_data *data)
{
//printf("lvoditel_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry),"activate");
    return(FALSE);

  case GDK_Escape:
  case GDK_F10:
    data->voz=-1;
    gtk_widget_destroy(widget);

    return(FALSE);

 }

return(TRUE);
}
/****************************/
/*Проверка пароля           */
/*****************************/

int    iceb_parol_prov(struct iceb_parol_data *data)
{
time_t vrem;
struct  tm      *bf;
time(&vrem);
bf=localtime(&vrem);

if(data->metkap == 0) //Проверка по сумме
 {
  if(atoi(data->parol) != bf->tm_mday + bf->tm_mon+1 + bf->tm_year+1900)
  {
   data->voz=1;
   return(1);  
  }
 }
if(data->metkap == 1) //Проверка по строке даты
 {
  char stroka[112];
  sprintf(stroka,"%d+%d+%d",bf->tm_year+1900,bf->tm_mon+1,bf->tm_mday);
  if(iceb_u_SRAV(data->parol,stroka,0) != 0)
   {
    data->voz=1;
    return(1);
   }
 }
data->voz=0;
return(0);
}
/******************************/
/*Чтение по Enter             */
/*******************************/
void iceb_parol_get_text(GtkWidget *widget,struct iceb_parol_data *data)
{

strcpy(data->parol,iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
gtk_entry_select_region(GTK_ENTRY(widget),0,strlen(data->parol));

iceb_parol_prov(data);
gtk_widget_destroy(data->window);

}
/***************************/
/*Обработчик сигнала delete event*/
/*********************************/
gboolean parol_delete_event(GtkWidget *widget,GdkEvent *event,struct iceb_parol_data *data)
{
data->voz=-1;
gtk_widget_destroy(widget);
return(FALSE);
}
