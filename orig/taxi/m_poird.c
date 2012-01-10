/*$Id: m_poird.c,v 1.8 2011-02-21 07:36:21 sasa Exp $*/
/*05.03.2004	05.03.2004	Белых А.И.	m_poird.c
Поиск документа по номеру документа
Если вернули 0-ищем
             1-нет
*/
#include <stdlib.h>
#include <unistd.h>
#include "i_rest.h"
enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_NOMD,
  E_GOD,
  KOLENTER
 };


class  m_poird_data
 {
  public:
  iceb_u_str nomd;
  iceb_u_str god;

  
  GtkWidget *entry[KOLENTER];
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       voz;  
  //Конструктор
  m_poird_data()
   {
    nomd.plus("");
    god.plus("");
    voz=0;
    kl_shift=0;
    window=NULL;
   }      
 };

gboolean   m_poird_key_press(GtkWidget *widget,GdkEventKey *event,class m_poird_data *data);
void  m_poird_knopka(GtkWidget *widget,class m_poird_data *data);
void    m_poird_vvod(GtkWidget *widget,class m_poird_data *data);
int restdok_pk(char *god,char *shet,GtkWidget*);

extern SQL_baza	bd;
extern char *name_system;


int  m_poird(iceb_u_str *nomd,short *god,GtkWidget *wpredok)
{
char strsql[300];
m_poird_data data;
iceb_u_str nadpis;
time_t vrem;
struct tm *bf;

time(&vrem);
bf=localtime(&vrem);

printf("l_m_poird\n");
    
data.kl_shift=0;
sprintf(strsql,"%d",bf->tm_year+1900);
data.god.new_plus(strsql);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
//gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_widget_destroyed),&data.window);
gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(m_poird_key_press),&data);

if(wpredok != NULL)
 {
 
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
nadpis.new_plus(gettext("Поиск документа по номеру"));

GtkWidget *label=gtk_label_new(nadpis.ravno_toutf());

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox),label);
for(int i=0; i < KOLENTER; i++) 
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

short nomer=0;
label=gtk_label_new(gettext("Номер документа"));
data.entry[E_NOMD] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[nomer]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[nomer++]), data.entry[E_NOMD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMD]), "activate",GTK_SIGNAL_FUNC(m_poird_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMD]),data.nomd.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMD]),(gpointer)E_NOMD);

label=gtk_label_new(gettext("Год"));
data.entry[E_GOD] = gtk_entry_new_with_max_length (5);
gtk_box_pack_start (GTK_BOX (hbox[nomer]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[nomer++]), data.entry[E_GOD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOD]), "activate",GTK_SIGNAL_FUNC(m_poird_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.god.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOD]),(gpointer)E_GOD);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(m_poird_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(m_poird_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
nomd->new_plus(data.nomd.ravno());
*god=data.god.ravno_atoi();
return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   m_poird_key_press(GtkWidget *widget,GdkEventKey *event,class m_poird_data *data)
{
//char  bros[300];

printf("m_poird_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
//    printf("Нажата клавиша F10\n");
    return(TRUE);

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
void  m_poird_knopka(GtkWidget *widget,class m_poird_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch ((gint)knop)
 {
  case FK2:
    for(int i=0; i < KOLENTER ; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(data->entry[i]),"activate");
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  


  case FK10:
    printf("Нажата кнопка F10\n");
    data->voz=1;
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    m_poird_vvod(GtkWidget *widget,class m_poird_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("m_poird_vvod enter=%d\n",enter);
switch (enter)
 {
  case E_NOMD:
    data->nomd.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GOD:
    data->god.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
