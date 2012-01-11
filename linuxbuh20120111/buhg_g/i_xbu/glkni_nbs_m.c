/*$Id: glkni_nbs_m.c,v 1.12 2011-02-21 07:35:51 sasa Exp $*/
/*08.02.2005	22.01.2004	Белых А.И.	glkni_nbs_m.c
Меню запуска расчета главной книги
*/

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "glkni_nbs.h"

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
  E_SHET,
  KOLENTER  
 };

class glkni_nbs_data
 {
  public:
  glkni_nbs_rr  *rek_r;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *radiobutton[2];
  
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  glkni_nbs_data() //Конструктор
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

iceb_u_str glkni_nbs_rr::datan;
iceb_u_str glkni_nbs_rr::datak;
iceb_u_str glkni_nbs_rr::shet;
short glkni_nbs_rr::metka_r;

void       glkni_nbs_radio0(GtkWidget *,class glkni_nbs_data *);
void       glkni_nbs_radio1(GtkWidget *,class glkni_nbs_data *);

gboolean   glkni_nbs_v_key_press(GtkWidget *widget,GdkEventKey *event,class glkni_nbs_data *data);
void    glkni_nbs_v_vvod(GtkWidget *widget,class glkni_nbs_data *data);
void  glkni_nbs_v_knopka(GtkWidget *widget,class glkni_nbs_data *data);
void glkni_nbs_r(class glkni_nbs_rr *datap);
void  glkni_nbs_v_e_knopka(GtkWidget *widget,class glkni_nbs_data *data);

extern char *name_system;

void  glkni_nbs_m()
{
glkni_nbs_rr  rek_ras;
glkni_nbs_data data;

data.rek_r=&rek_ras;
if(data.rek_r->datan.getdlinna() == 0)
  data.rek_r->clear_data();
  
char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Оборотный баланс"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(glkni_nbs_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Расчет оборотного баланса по забалансовым счетам"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);


GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);
GtkWidget *hboxradio = gtk_hbox_new (FALSE, 0);

//Вставляем радиокнопки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Расчет по субсчетам"));
if(data.rek_r->metka_r == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[0]), "clicked",GTK_SIGNAL_FUNC(glkni_nbs_radio0),&data);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

data.radiobutton[1]=gtk_radio_button_new_with_label(group,gettext("Расчет по счетам"));
if(data.rek_r->metka_r == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[1]), "clicked",GTK_SIGNAL_FUNC(glkni_nbs_radio1),&data);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[1], TRUE, TRUE, 0);


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxradio);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(glkni_nbs_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(glkni_nbs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rek_r->datan.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(glkni_nbs_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"),NULL);


data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(glkni_nbs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rek_r->datak.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

sprintf(strsql,"%s (,,)",gettext("Счет"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(glkni_nbs_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(glkni_nbs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rek_r->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);


GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(glkni_nbs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(glkni_nbs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(glkni_nbs_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {

  glkni_nbs_r(&rek_ras);
 }

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  glkni_nbs_v_e_knopka(GtkWidget *widget,class glkni_nbs_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rek_r->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rek_r->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rek_r->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rek_r->datak.ravno());
      
    return;  
   
  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->rek_r->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rek_r->shet.ravno_toutf());

    return;  

 }
}

/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  glkni_nbs_radio0(GtkWidget *widget,class glkni_nbs_data *data)
{
//g_print("glkni_nbs_radio0\n");

if(data->rek_r->metka_r == 0)
  return;
data->rek_r->metka_r=0;

}
void  glkni_nbs_radio1(GtkWidget *widget,class glkni_nbs_data *data)
{
//g_print("glkni_nbs_radio1\n");
if(data->rek_r->metka_r == 1)
  return;
data->rek_r->metka_r=1;
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   glkni_nbs_v_key_press(GtkWidget *widget,GdkEventKey *event,class glkni_nbs_data *data)
{

//printf("glkni_nbs_v_key_press\n");
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
void  glkni_nbs_v_knopka(GtkWidget *widget,class glkni_nbs_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(iceb_rsdatp_str(&data->rek_r->datan,&data->rek_r->datak,data->window) != 0)
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

void    glkni_nbs_v_vvod(GtkWidget *widget,class glkni_nbs_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("glkni_nbs_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rek_r->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->rek_r->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET:
    data->rek_r->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
