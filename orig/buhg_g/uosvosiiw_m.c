/*$Id: uosvosiiw_m.c,v 1.9 2011-02-21 07:35:58 sasa Exp $*/
/*07.01.2008	07.01.2008	Белых А.И.	uosvosiiw_m.c
Меню для ввода реквизитов
*/

#include "buhg_g.h"
#include "uosvosiiw.h"
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
  E_GRUPA_NU,
  E_GRUPA_BU,
  E_PODR,
  E_MAT_OT,
  E_SHETU,
  E_HAU,
  E_INNOM,
  E_HZT,
  E_KOD_OP,
  KOLENTER  
 };

class uosvosiiw_m_data
 {
  public:
  class uosvosiiw_data *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt1;
  GtkWidget *radiobutton1[2];
  GtkWidget *radiobutton2[3];
  GtkWidget *radiobutton3[2];
  short kl_shift;
  short voz;      //0-начать расчет 1 нет

  uosvosiiw_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[0])) == TRUE)
     rk->metka_mat_podr=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1[1])) == TRUE)
     rk->metka_mat_podr=1;


    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[0])) == TRUE)
     rk->metka_ost=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[1])) == TRUE)
     rk->metka_ost=1;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[2])) == TRUE)
     rk->metka_ost=2;

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton3[0])) == TRUE)
     rk->metka_sort=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton3[1])) == TRUE)
     rk->metka_sort=1;

   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    gtk_option_menu_set_history(GTK_OPTION_MENU(opt1),0);
    rk->clear();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton1[0]),TRUE); //Устанавливем активной кнопку
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2[0]),TRUE); //Устанавливем активной кнопку
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton3[0]),TRUE); //Устанавливем активной кнопку
   }
 };


gboolean   uosvosiiw_v_key_press(GtkWidget *widget,GdkEventKey *event,class uosvosiiw_m_data *data);
void    uosvosiiw_v_vvod(GtkWidget *widget,class uosvosiiw_m_data *data);
void  uosvosiiw_v_knopka(GtkWidget *widget,class uosvosiiw_m_data *data);
void  uosvosiiw_v_e_knopka(GtkWidget *widget,class uosvosiiw_m_data *data);

int uosvosiiw_m_provr(class uosvosiiw_m_data *data);

extern char *name_system;
extern SQL_baza bd;

int uosvosiiw_m(int metka_r, /*0-налоговый учёт 1-бух.учёт*/
class uosvosiiw_data *rek_ras)
{
char strsql[512];


class uosvosiiw_m_data data;

data.rk=rek_ras;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(metka_r == 0)
  sprintf(strsql,"%s %s",name_system,gettext("Распечатать ведомость износа и остаточной стоимости (налоговый учёт)"));//5
if(metka_r == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Распечатать ведомость износа и остаточной стоимости (бух. учёт)"));//6

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosvosiiw_v_key_press),&data);

GtkWidget *label=NULL;
if(metka_r == 0)
  label=gtk_label_new(gettext("Распечатать ведомость износа и остаточной стоимости (налоговый учёт)"));//5
if(metka_r == 1)
  label=gtk_label_new(gettext("Распечатать ведомость износа и остаточной стоимости (бух. учёт)"));//6

GtkWidget *vbox = gtk_vbox_new (FALSE, 1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE,1);

GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);
GtkWidget *hboxopt1 = gtk_hbox_new (FALSE,1);


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE,1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE,1);

gtk_box_pack_start (GTK_BOX (vbox), hboxopt1, FALSE, FALSE,1);

GtkWidget *separator[4];

separator[0]=gtk_hseparator_new();
separator[1]=gtk_hseparator_new();
separator[2]=gtk_hseparator_new();
separator[3]=gtk_hseparator_new();
gtk_box_pack_start (GTK_BOX (vbox),separator[0], TRUE, TRUE, 1);

//Вставляем радиокнопки

data.radiobutton1[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Расчет по подразделениям"));
data.radiobutton1[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton1[0]),gettext("Расчёт по материально-ответственным"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton1[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton1[1], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[data.rk->metka_mat_podr]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox),separator[1], TRUE, TRUE, 1);

//Вставляем радиокнопки

data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Все основные средства"));
data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),gettext("Основные средства с нулевой остаточной стоимостью"));
data.radiobutton2[2]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[1]),gettext("Основные средства с ненулевой остаточной стоимостью"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[1], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[2], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk->metka_ost]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox),separator[2], TRUE, TRUE, 1);

//Вставляем радиокнопки

data.radiobutton3[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Сортировка по группам"));
data.radiobutton3[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton3[0]),gettext("Сортировка по счетам учёта"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton3[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton3[1], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton3[data.rk->metka_sort]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox),separator[3], TRUE, TRUE, 1);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE,1);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(uosvosiiw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);


sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(uosvosiiw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);



sprintf(strsql,"%s (,,)",gettext("Код группы налогового учёта"));
data.knopka_enter[E_GRUPA_NU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA_NU]), data.knopka_enter[E_GRUPA_NU], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUPA_NU]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUPA_NU]),(gpointer)E_GRUPA_NU);
tooltips_enter[E_GRUPA_NU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUPA_NU],data.knopka_enter[E_GRUPA_NU],gettext("Выбор группы"),NULL);

data.entry[E_GRUPA_NU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA_NU]), data.entry[E_GRUPA_NU], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUPA_NU]), "activate",GTK_SIGNAL_FUNC(uosvosiiw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA_NU]),data.rk->grupa_nu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUPA_NU]),(gpointer)E_GRUPA_NU);


sprintf(strsql,"%s (,,)",gettext("Код группы бух. учёта"));
data.knopka_enter[E_GRUPA_BU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA_BU]), data.knopka_enter[E_GRUPA_BU], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUPA_BU]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUPA_BU]),(gpointer)E_GRUPA_BU);
tooltips_enter[E_GRUPA_BU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUPA_BU],data.knopka_enter[E_GRUPA_BU],gettext("Выбор группы"),NULL);

data.entry[E_GRUPA_BU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA_BU]), data.entry[E_GRUPA_BU], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUPA_BU]), "activate",GTK_SIGNAL_FUNC(uosvosiiw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA_BU]),data.rk->grupa_bu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUPA_BU]),(gpointer)E_GRUPA_BU);


sprintf(strsql,"%s (,,)",gettext("Код мат.ответственного"));
data.knopka_enter[E_MAT_OT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.knopka_enter[E_MAT_OT], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_MAT_OT]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_MAT_OT]),(gpointer)E_MAT_OT);
tooltips_enter[E_MAT_OT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_MAT_OT],data.knopka_enter[E_MAT_OT],gettext("Выбор материально-ответственного"),NULL);

data.entry[E_MAT_OT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.entry[E_MAT_OT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_MAT_OT]), "activate",GTK_SIGNAL_FUNC(uosvosiiw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MAT_OT]),data.rk->mat_ot.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_MAT_OT]),(gpointer)E_MAT_OT);


sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(uosvosiiw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);


sprintf(strsql,"%s (,,)",gettext("Счёт учёта"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHETU]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHETU]),(gpointer)E_SHETU);
tooltips_enter[E_SHETU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHETU],data.knopka_enter[E_SHETU],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHETU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHETU]), "activate",GTK_SIGNAL_FUNC(uosvosiiw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.rk->shetu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHETU]),(gpointer)E_SHETU);


sprintf(strsql,"%s (,,)",gettext("Шифр затрат аморт-отчислений"));
data.knopka_enter[E_HZT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_HZT]), data.knopka_enter[E_HZT], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_HZT]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_HZT]),(gpointer)E_HZT);
tooltips_enter[E_HZT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_HZT],data.knopka_enter[E_HZT],gettext("Выбор шифра затрат аморт-отчислений"),NULL);

data.entry[E_HZT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HZT]), data.entry[E_HZT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HZT]), "activate",GTK_SIGNAL_FUNC(uosvosiiw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HZT]),data.rk->hzt.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HZT]),(gpointer)E_HZT);

sprintf(strsql,"%s (,,)",gettext("Шифр аналитического учёта"));
data.knopka_enter[E_HAU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_HAU]), data.knopka_enter[E_HAU], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_HAU]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_HAU]),(gpointer)E_HAU);
tooltips_enter[E_HAU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_HAU],data.knopka_enter[E_HAU],gettext("Выбор шифра аналитического учёта"),NULL);

data.entry[E_HAU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HAU]), data.entry[E_HAU], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HAU]), "activate",GTK_SIGNAL_FUNC(uosvosiiw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HAU]),data.rk->hzt.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HAU]),(gpointer)E_HAU);


sprintf(strsql,"%s (,,)",gettext("Инвентарный номер"));
data.knopka_enter[E_INNOM]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_INNOM]), data.knopka_enter[E_INNOM], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_INNOM]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_INNOM]),(gpointer)E_INNOM);
tooltips_enter[E_INNOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_INNOM],data.knopka_enter[E_INNOM],gettext("Выбор инвентарного номера"),NULL);

data.entry[E_INNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_INNOM]), data.entry[E_INNOM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_INNOM]), "activate",GTK_SIGNAL_FUNC(uosvosiiw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INNOM]),data.rk->innom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_INNOM]),(gpointer)E_INNOM);


sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KOD_OP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.knopka_enter[E_KOD_OP], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KOD_OP]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KOD_OP]),(gpointer)E_KOD_OP);
tooltips_enter[E_KOD_OP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KOD_OP],data.knopka_enter[E_KOD_OP],gettext("Выбор операции"),NULL);

data.entry[E_KOD_OP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.entry[E_KOD_OP], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOD_OP]), "activate",GTK_SIGNAL_FUNC(uosvosiiw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_OP]),data.rk->kod_op.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOD_OP]),(gpointer)E_KOD_OP);





label=gtk_label_new(gettext("Состояние объекта"));
gtk_box_pack_start (GTK_BOX (hboxopt1), label, FALSE, FALSE, 1);

class iceb_u_spisok bal_st;
bal_st.plus(gettext("Все основные средства"));
bal_st.plus(gettext("Основные средства, амортизация для которых считается"));
bal_st.plus(gettext("Основные средства, амортизация для которых не считается"));

iceb_pm_vibor(&bal_st,&data.opt1,&data.rk->sost_ob);
gtk_box_pack_start (GTK_BOX (hboxopt1), data.opt1, TRUE, TRUE,1);




GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);


sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(uosvosiiw_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uosvosiiw_v_e_knopka(GtkWidget *widget,class uosvosiiw_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
/*g_print("uosvosiiw_v_e_knopka knop=%d\n",knop);*/

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

  case E_INNOM:

    if(l_uosin(1,&kod,&naim,data->window) == 0)
      data->rk->innom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_INNOM]),data->rk->innom.ravno());
      
    return;  

  case E_KOD_OP:
    vib_kodop_uos(&data->rk->kod_op,data->window);
    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_OP]),data->rk->kod_op.ravno_toutf());
      
    return;  

  case E_GRUPA_NU:

    if(l_uosgrnu(1,&kod,&naim,data->window) == 0)
      data->rk->grupa_nu.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA_NU]),data->rk->grupa_nu.ravno_toutf());
      
    return;  

  case E_GRUPA_BU:

    if(l_uosgrbu(1,&kod,&naim,data->window) == 0)
      data->rk->grupa_bu.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA_BU]),data->rk->grupa_bu.ravno_toutf());
      
    return;  


  case E_MAT_OT:

    if(l_uosmo(1,&kod,&naim,data->window) == 0)
      data->rk->mat_ot.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_MAT_OT]),data->rk->mat_ot.ravno());
      
    return;  

  case E_PODR:

    if(l_uospodr(1,&kod,&naim,data->window) == 0)
      data->rk->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
      
    return;  

  case E_SHETU:

    if(iceb_l_plansh(1,&kod,&naim,data->window) == 0)
      data->rk->shetu.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->rk->shetu.ravno_toutf());
      
    return;  

  case E_HAU:

    if(l_uoshau(1,&kod,&naim,data->window) == 0)
      data->rk->hau.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HAU]),data->rk->hau.ravno_toutf());
      
    return;  

  case E_HZT:

    if(l_uoshz(1,&kod,&naim,data->window) == 0)
      data->rk->hzt.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HZT]),data->rk->hzt.ravno_toutf());
      
    return;  


   

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosvosiiw_v_key_press(GtkWidget *widget,GdkEventKey *event,class uosvosiiw_m_data *data)
{

//printf("uosvosiiw_v_key_press\n");
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
void  uosvosiiw_v_knopka(GtkWidget *widget,class uosvosiiw_m_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(uosvosiiw_m_provr(data) != 0)
     return;
    
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    uosvosiiw_v_vvod(GtkWidget *widget,class uosvosiiw_m_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("uosvosiiw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_INNOM:
    data->rk->innom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOD_OP:
    data->rk->kod_op.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUPA_NU:
    data->rk->grupa_nu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_GRUPA_BU:
    data->rk->grupa_bu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_MAT_OT:
    data->rk->mat_ot.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_PODR:
    data->rk->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHETU:
    data->rk->shetu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_HAU:
    data->rk->hau.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_HZT:
    data->rk->hzt.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;



 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************************/
/*Проверка реквизитов*/
/*******************************/
int uosvosiiw_m_provr(class uosvosiiw_m_data *data)
{

if(iceb_rsdatp(data->rk->datan.ravno(),data->rk->datak.ravno(),data->window) != 0)
 return(1);

return(0);
}


