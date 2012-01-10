/*$Id: kasothw_m.c,v 1.14 2011-04-14 16:09:35 sasa Exp $*/
/*09.04.2010	10.03.2006	Белых А.И.	kasothw_m.c
Меню для получения отчета 
*/
#include <stdlib.h>
#include "buhg_g.h"
#include "kasothw.h"

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
  E_KASSA,
  E_SHET,
  E_KOD_KONTR,
  KOLENTER  
 };

class kasothw_data
 {
  public:
  kasothw_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет
  GtkWidget *radiobutton2[3];

  kasothw_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN]))));
    rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK]))));
    rk->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP]))));
    rk->kassa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KASSA]))));
    rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    rk->kod_kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_KONTR]))));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };



void  kasothw_radio02(GtkWidget *widget,class kasothw_data *data);
void  kasothw_radio12(GtkWidget *widget,class kasothw_data *data);
void  kasothw_radio22(GtkWidget *widget,class kasothw_data *data);

gboolean   kasothw_v_key_press(GtkWidget *widget,GdkEventKey *event,class kasothw_data *data);
void    kasothw_v_vvod(GtkWidget *widget,class kasothw_data *data);
void  kasothw_v_knopka(GtkWidget *widget,class kasothw_data *data);
void  kasothw_v_e_knopka(GtkWidget *widget,class kasothw_data *data);


extern char *name_system;
extern SQL_baza bd;

int kasothw_m(class kasothw_rr *rek_ras,GtkWidget *wpredok)
{
kasothw_data data;

data.rk=rek_ras;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,data.rk->naim_oth.ravno_toutf());
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kasothw_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

class iceb_u_str repl;
repl.plus(gettext("Учёт кассовых ордеров"));

repl.ps_plus(data.rk->naim_oth.ravno());
GtkWidget *label=gtk_label_new(repl.ravno_toutf());

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

//Вставляем радиокнопки приходов-расходов

data.radiobutton2[0]=gtk_radio_button_new_with_label(NULL,gettext("Все документы"));
if(data.rk->metka_pr == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[0]), "clicked",GTK_SIGNAL_FUNC(kasothw_radio02),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton20),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton2[0]));

data.radiobutton2[1]=gtk_radio_button_new_with_label(group,gettext("Только приходные документы"));
if(data.rk->metka_pr == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[1]), "clicked",GTK_SIGNAL_FUNC(kasothw_radio12),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[1], TRUE, TRUE, 0);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton2[1]));

data.radiobutton2[2]=gtk_radio_button_new_with_label(group,gettext("Только расходные документы"));
if(data.rk->metka_pr == 2)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[2]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton2[2]), "clicked",GTK_SIGNAL_FUNC(kasothw_radio22),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton2[2], TRUE, TRUE, 0);

GtkWidget *separator4=gtk_hseparator_new();
gtk_container_add (GTK_CONTAINER (vbox), separator4);


gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(kasothw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(kasothw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);


sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODOP]),"clicked",GTK_SIGNAL_FUNC(kasothw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODOP]),(gpointer)E_KODOP);
tooltips_enter[E_KODOP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODOP],data.knopka_enter[E_KODOP],gettext("Выбор операции"),NULL);

data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODOP]), "activate",GTK_SIGNAL_FUNC(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODOP]),(gpointer)E_KODOP);


sprintf(strsql,"%s (,,)",gettext("Код кассы"));
data.knopka_enter[E_KASSA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.knopka_enter[E_KASSA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KASSA]),"clicked",GTK_SIGNAL_FUNC(kasothw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KASSA]),(gpointer)E_KASSA);
tooltips_enter[E_KASSA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KASSA],data.knopka_enter[E_KASSA],gettext("Выбор кассы"),NULL);

data.entry[E_KASSA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KASSA]), data.entry[E_KASSA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KASSA]), "activate",GTK_SIGNAL_FUNC(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KASSA]),data.rk->kassa.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KASSA]),(gpointer)E_KASSA);

sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(kasothw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KOD_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.knopka_enter[E_KOD_KONTR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_KONTR]),"clicked",GTK_SIGNAL_FUNC(kasothw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_KONTR]),(gpointer)E_KOD_KONTR);
tooltips_enter[E_KOD_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_KONTR],data.knopka_enter[E_KOD_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_KOD_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.entry[E_KOD_KONTR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_KONTR]), "activate",GTK_SIGNAL_FUNC(kasothw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_KONTR]),data.rk->kod_kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_KONTR]),(gpointer)E_KOD_KONTR);



GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(kasothw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(kasothw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(kasothw_v_knopka),&data);
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
void  kasothw_v_e_knopka(GtkWidget *widget,class kasothw_data *data)
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

  case E_KODOP:
    vib_kodop_kas(&data->rk->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno_toutf());
    return;  


  case E_KASSA:
    if(l_spis_kas(1,&kod,&naim,data->window) == 0)
      data->rk->kassa.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KASSA]),data->rk->kassa.ravno_toutf());
    return;

  case E_SHET:
    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno_toutf());
    return;

  case E_KOD_KONTR:
    iceb_vibrek(0,"Kontragent",&data->rk->kod_kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_KONTR]),data->rk->kod_kontr.ravno_toutf());
    return;
 }
}

/******************************/
/*Обработчик нажатия радиокнопок приходов/расходов*/
/**********************************/

void  kasothw_radio02(GtkWidget *widget,class kasothw_data *data)
{
//g_print("kasothw_radio0\n");

if(data->rk->metka_pr == 0)
  return;
data->rk->metka_pr=0;

}
void  kasothw_radio12(GtkWidget *widget,class kasothw_data *data)
{
//g_print("kasothw_radio1\n");
if(data->rk->metka_pr == 1)
  return;
data->rk->metka_pr=1;
}
void  kasothw_radio22(GtkWidget *widget,class kasothw_data *data)
{
//g_print("kasothw_radio1\n");
if(data->rk->metka_pr == 2)
  return;
data->rk->metka_pr=2;
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kasothw_v_key_press(GtkWidget *widget,GdkEventKey *event,class kasothw_data *data)
{

//printf("kasothw_v_key_press\n");
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
void  kasothw_v_knopka(GtkWidget *widget,class kasothw_data *data)
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

void    kasothw_v_vvod(GtkWidget *widget,class kasothw_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("kasothw_v_vvod enter=%d\n",enter);

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


  case E_KASSA:
    data->rk->kassa.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
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
