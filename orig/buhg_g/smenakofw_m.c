/*$Id: smenakofw_m.c,v 1.8 2011-02-21 07:35:57 sasa Exp $*/
/*19.10.2007	19.10.2007	Белых А.И.	smenfkofw_m.c
Меню для ввода данных для изменения коэффициента
*/
#include <stdlib.h>
#include "buhg_g.h"
#include "smenakofw.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAK,
  E_KOD_GR,
  E_KOF,
  KOLENTER  
 };

class smenakofw_m_data
 {
  public:
  smenakofw_data  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  smenakofw_m_data() //Конструктор
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
    rk->clear_data();
   }
 };



gboolean   smenakofw_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class smenakofw_m_data *data);
void    smenakofw_m_v_vvod(GtkWidget *widget,class smenakofw_m_data *data);
void  smenakofw_m_v_knopka(GtkWidget *widget,class smenakofw_m_data *data);
void   smenakofw_m_rekviz(class smenakofw_m_data *data);
void  smenakofw_v_e_knopka(GtkWidget *widget,class smenakofw_m_data *data);

extern char *name_system;
extern SQL_baza bd;

int   smenakofw_m(class smenakofw_data *rek_ras,GtkWidget *wpredok)
{
//smenakofw_m_rr  rek_ras;
smenakofw_m_data data;

data.rk=rek_ras;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Смена коэффициента"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(smenakofw_m_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=NULL;
if(data.rk->metka_uh == 0)
 label=gtk_label_new(gettext("Смена коэффициентов для налогового учёта"));
if(data.rk->metka_uh == 1)
 label=gtk_label_new(gettext("Смена коэффициентов для бухгалтерского учёта"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);
GtkTooltips *tooltips_enter[KOLENTER];
/*********
sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("м.г"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), label, FALSE, FALSE, 0);
**************/

sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(smenakofw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(smenakofw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);


sprintf(strsql,"%s",gettext("Код группы"));
data.knopka_enter[E_KOD_GR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR]), data.knopka_enter[E_KOD_GR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_GR]),"clicked",GTK_SIGNAL_FUNC(smenakofw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_GR]),(gpointer)E_KOD_GR);
tooltips_enter[E_KOD_GR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_GR],data.knopka_enter[E_KOD_GR],gettext("Выбор группы"),NULL);

data.entry[E_KOD_GR] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR]), data.entry[E_KOD_GR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_GR]), "activate",GTK_SIGNAL_FUNC(smenakofw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_GR]),data.rk->kod_gr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_GR]),(gpointer)E_KOD_GR);

label=gtk_label_new(gettext("Коэффициент"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOF]), label, FALSE, FALSE,1);

data.entry[E_KOF] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOF]), data.entry[E_KOF], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOF]), "activate",GTK_SIGNAL_FUNC(smenakofw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOF]),data.rk->kof.ravno());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOF]),(gpointer)E_KOF);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(smenakofw_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(smenakofw_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(smenakofw_m_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

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
void  smenakofw_v_e_knopka(GtkWidget *widget,class smenakofw_m_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATAK:

    if(iceb_calendar1(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
      
    return;  

  case E_KOD_GR:
    
    if(data->rk->metka_uh == 0)
     if(l_uosgrnu(1,&kod,&naim,data->window) == 0)
      data->rk->kod_gr.new_plus(kod.ravno());

    if(data->rk->metka_uh == 1)
     if(l_uosgrbu(1,&kod,&naim,data->window) == 0)
      data->rk->kod_gr.new_plus(kod.ravno());
  
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_GR]),data->rk->kod_gr.ravno());
      
    return;  

 }
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   smenakofw_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class smenakofw_m_data *data)
{

//printf("smenakofw_m_v_key_press\n");
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
void  smenakofw_m_v_knopka(GtkWidget *widget,class smenakofw_m_data *data)
{
char strsql[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("smenakofw_m_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->rk->datak.prov_dat1() != 0)
     {
      iceb_menu_soob(gettext("Не верно введена дата !"),data->window);
      return;       
     }

    if(data->rk->kof.ravno_atof() == 0.)
     {
      iceb_menu_soob(gettext("Не введено коэффициент !"),data->window);
      return;       
     }
    if(data->rk->metka_uh == 0)
     sprintf(strsql,"select * from Uosgrup where kod='%s'",data->rk->kod_gr.ravno());
    if(data->rk->metka_uh == 1)
     sprintf(strsql,"select * from Uosgrup1 where kod='%s'",data->rk->kod_gr.ravno());

    if(iceb_sql_readkey(strsql,data->window) <= 0)
     {
      sprintf(strsql,"%s %s !",gettext("Ненайден код группы"),data->rk->kod_gr.ravno());
      
      iceb_menu_soob(strsql,data->window);
      return;       
      
     }
     
    gtk_widget_destroy(data->window);
    data->voz=0;
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    smenakofw_m_v_vvod(GtkWidget *widget,class smenakofw_m_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("smenakofw_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAK:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOF:
    data->rk->kof.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOD_GR:
    data->rk->kod_gr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
