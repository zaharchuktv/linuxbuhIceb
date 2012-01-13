/*$Id: mumow_m.c,v 1.6 2011-02-21 07:35:55 sasa Exp $*/
/*17.12.2009	17.12.2009	Белых А.И.	mumow_m.c
Меню для получения отчета 
*/
#include <stdlib.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "mumow.h"

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
  E_SKLAD,
  E_GRUPM,
  E_KODMAT,
  E_SHET,
  KOLENTER  
 };

class mumow_data
 {
  public:
  mumow_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  mumow_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN]))));
    rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK]))));
    rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    rk->grupm.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GRUPM]))));
    rk->kodmat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODMAT]))));
    
//    for(int i=0; i < KOLENTER; i++)
//      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };



void  mumow_radio02(GtkWidget *widget,class mumow_data *data);
void  mumow_radio12(GtkWidget *widget,class mumow_data *data);
void  mumow_radio22(GtkWidget *widget,class mumow_data *data);

gboolean   mumow_v_key_press(GtkWidget *widget,GdkEventKey *event,class mumow_data *data);
void    mumow_v_vvod(GtkWidget *widget,class mumow_data *data);
void  mumow_v_knopka(GtkWidget *widget,class mumow_data *data);
void  mumow_v_e_knopka(GtkWidget *widget,class mumow_data *data);


extern char *name_system;
extern SQL_baza bd;

int   mumow_m(class mumow_rr *rek_ras)
{
mumow_data data;

data.rk=rek_ras;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Материальный отчёт"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(mumow_v_key_press),&data);


GtkWidget *label=gtk_label_new(gettext("Материальный отчёт"));

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


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(mumow_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(mumow_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(mumow_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(mumow_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);


sprintf(strsql,"%s (,,)",gettext("Код склада"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKLAD]),"clicked",GTK_SIGNAL_FUNC(mumow_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKLAD]),(gpointer)E_SKLAD);
tooltips_enter[E_SKLAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SKLAD],data.knopka_enter[E_SKLAD],gettext("Выбор склада"),NULL);

data.entry[E_SKLAD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKLAD]), "activate",GTK_SIGNAL_FUNC(mumow_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKLAD]),(gpointer)E_SKLAD);


sprintf(strsql,"%s (,,)",gettext("Код группы материалла"));
data.knopka_enter[E_GRUPM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPM]), data.knopka_enter[E_GRUPM], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUPM]),"clicked",GTK_SIGNAL_FUNC(mumow_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUPM]),(gpointer)E_GRUPM);
tooltips_enter[E_GRUPM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUPM],data.knopka_enter[E_GRUPM],gettext("Выбор группы"),NULL);

data.entry[E_GRUPM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPM]), data.entry[E_GRUPM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUPM]), "activate",GTK_SIGNAL_FUNC(mumow_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPM]),data.rk->grupm.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUPM]),(gpointer)E_GRUPM);

sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(mumow_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(mumow_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s (,,)",gettext("Код материалла"));
data.knopka_enter[E_KODMAT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.knopka_enter[E_KODMAT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODMAT]),"clicked",GTK_SIGNAL_FUNC(mumow_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODMAT]),(gpointer)E_KODMAT);
tooltips_enter[E_KODMAT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODMAT],data.knopka_enter[E_KODMAT],gettext("Выбор материалла"),NULL);

data.entry[E_KODMAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.entry[E_KODMAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODMAT]), "activate",GTK_SIGNAL_FUNC(mumow_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODMAT]),data.rk->kodmat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODMAT]),(gpointer)E_KODMAT);



GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(mumow_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(mumow_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(mumow_v_knopka),&data);
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
void  mumow_v_e_knopka(GtkWidget *widget,class mumow_data *data)
{
class iceb_u_str kod;
class iceb_u_str naim;
kod.plus("");
naim.plus("");

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



  case E_GRUPM:
    if(iceb_l_grupmat(1,&kod,&naim,data->window) == 0)
      data->rk->grupm.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPM]),data->rk->grupm.ravno_toutf());
    return;

  case E_KODMAT:
    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
      data->rk->kodmat.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno_toutf());
    return;

  case E_SKLAD:
    if(iceb_l_sklad(1,&kod,&naim,data->window) == 0)
      data->rk->sklad.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno_toutf());
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

gboolean   mumow_v_key_press(GtkWidget *widget,GdkEventKey *event,class mumow_data *data)
{

//printf("mumow_v_key_press\n");
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
void  mumow_v_knopka(GtkWidget *widget,class mumow_data *data)
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

void    mumow_v_vvod(GtkWidget *widget,class mumow_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("mumow_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SKLAD:
    data->rk->sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


  case E_GRUPM:
    data->rk->grupm.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET:
    data->rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
