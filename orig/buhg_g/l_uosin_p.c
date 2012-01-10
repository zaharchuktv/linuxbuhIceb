/*$Id: l_uosin_p.c,v 1.7 2011-02-21 07:35:54 sasa Exp $*/
/*20.12.2007	20.12.2007	Белых А.И.	l_uosin_p.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_uosin.h"
enum
 {
  E_NAIM,
  E_ZAVOD_IZ,
  E_PASPORT,
  E_MODEL,
  E_GOD_IZ,
  E_ZAV_NOMER,
  E_DATA_VVODA,
  E_SHETU,
  E_HZT,
  E_HAU,
  E_HNA,
  E_HNABU,
  E_POPKF_NU,
  E_POPKF_BU,
  E_NOM_ZNAK,
  E_MAT_OT,
  E_PODR,
  KOLENTER  
 };

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

class l_uosin_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_PFK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uosin_poi *rk;
  
  
  l_uosin_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
     }
   }

  void clear_data()
   {
    rk->clear();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void l_uosin_p_clear(class l_uosin_p_data *data);
void    l_uosin_p_vvod(GtkWidget *widget,class l_uosin_p_data *data);
void  l_uosin_p_knopka(GtkWidget *widget,class l_uosin_p_data *data);
gboolean   l_uosin_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosin_p_data *data);
void  l_uosin_p_e_knopka(GtkWidget *widget,class l_uosin_p_data *data);


extern char      *name_system;

int l_uosin_p(class l_uosin_poi *rek_poi,GtkWidget *wpredok)
{
class l_uosin_p_data data;
char strsql[512];
data.rk=rek_poi;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_uosin_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }



GtkWidget *vbox = gtk_vbox_new (FALSE,1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE,1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE,1);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkTooltips *tooltips_enter[KOLENTER];

GtkWidget *label=gtk_label_new(gettext("Наименование"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE,1);



data.entry[E_NAIM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NAIM]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk->naim.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NAIM]),(gpointer)E_NAIM);

label=gtk_label_new(gettext("Завод изготовитель"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZAVOD_IZ]), label, FALSE, FALSE,1);

data.entry[E_ZAVOD_IZ] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_ZAVOD_IZ]), data.entry[E_ZAVOD_IZ], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZAVOD_IZ]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZAVOD_IZ]),data.rk->zavod_iz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZAVOD_IZ]),(gpointer)E_ZAVOD_IZ);


label=gtk_label_new(gettext("Паспорт"));
gtk_box_pack_start (GTK_BOX (hbox[E_PASPORT]), label, FALSE, FALSE,1);

data.entry[E_PASPORT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PASPORT]), data.entry[E_PASPORT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PASPORT]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PASPORT]),data.rk->pasport.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PASPORT]),(gpointer)E_PASPORT);

label=gtk_label_new(gettext("Модель"));
gtk_box_pack_start (GTK_BOX (hbox[E_MODEL]), label, FALSE, FALSE,1);

data.entry[E_MODEL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_MODEL]), data.entry[E_MODEL], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_MODEL]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MODEL]),data.rk->model.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_MODEL]),(gpointer)E_MODEL);

label=gtk_label_new(gettext("Год изготовления"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOD_IZ]), label, FALSE, FALSE,1);

data.entry[E_GOD_IZ] = gtk_entry_new_with_max_length (4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD_IZ]), data.entry[E_GOD_IZ], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GOD_IZ]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD_IZ]),data.rk->god_iz.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GOD_IZ]),(gpointer)E_GOD_IZ);

label=gtk_label_new(gettext("Заводской номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_NOMER]), label, FALSE, FALSE,1);

data.entry[E_ZAV_NOMER] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_NOMER]), data.entry[E_ZAV_NOMER], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_ZAV_NOMER]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZAV_NOMER]),data.rk->zav_nomer.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_ZAV_NOMER]),(gpointer)E_ZAV_NOMER);


sprintf(strsql,"%s",gettext("Дата ввода в эксплуатацию"));
data.knopka_enter[E_DATA_VVODA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VVODA]), data.knopka_enter[E_DATA_VVODA], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATA_VVODA]),"clicked",GTK_SIGNAL_FUNC(l_uosin_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATA_VVODA]),(gpointer)E_DATA_VVODA);
tooltips_enter[E_DATA_VVODA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATA_VVODA],data.knopka_enter[E_DATA_VVODA],gettext("Выбор даты"),NULL);

data.entry[E_DATA_VVODA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VVODA]), data.entry[E_DATA_VVODA], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_VVODA]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_VVODA]),data.rk->data_vvoda.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_VVODA]),(gpointer)E_DATA_VVODA);


sprintf(strsql,"%s (,,)",gettext("Счёт учёта"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHETU]),"clicked",GTK_SIGNAL_FUNC(l_uosin_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHETU]),(gpointer)E_SHETU);
tooltips_enter[E_SHETU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHETU],data.knopka_enter[E_SHETU],gettext("Выбор шифра затрат"),NULL);

data.entry[E_SHETU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHETU]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.rk->shetu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHETU]),(gpointer)E_SHETU);


sprintf(strsql,"%s (,,)",gettext("Шифр затрат аморт-отчислений"));
data.knopka_enter[E_HZT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_HZT]), data.knopka_enter[E_HZT], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_HZT]),"clicked",GTK_SIGNAL_FUNC(l_uosin_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_HZT]),(gpointer)E_HZT);
tooltips_enter[E_HZT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_HZT],data.knopka_enter[E_HZT],gettext("Выбор шифра затрат аморт-отчислений"),NULL);

data.entry[E_HZT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HZT]), data.entry[E_HZT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HZT]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HZT]),data.rk->hzt.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HZT]),(gpointer)E_HZT);



sprintf(strsql,"%s (,,)",gettext("Шифр аналитического учёта"));
data.knopka_enter[E_HAU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_HAU]), data.knopka_enter[E_HAU], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_HAU]),"clicked",GTK_SIGNAL_FUNC(l_uosin_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_HAU]),(gpointer)E_HAU);
tooltips_enter[E_HAU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_HAU],data.knopka_enter[E_HAU],gettext("Выбор шифра аналитического учёта"),NULL);

data.entry[E_HAU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HAU]), data.entry[E_HAU], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HAU]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HAU]),data.rk->hau.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HAU]),(gpointer)E_HAU);




sprintf(strsql,"%s (,,)",gettext("Группа налогового учёта"));
data.knopka_enter[E_HNA]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_HNA]), data.knopka_enter[E_HNA], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_HNA]),"clicked",GTK_SIGNAL_FUNC(l_uosin_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_HNA]),(gpointer)E_HNA);
tooltips_enter[E_HNA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_HNA],data.knopka_enter[E_HNA],gettext("Выбор группы"),NULL);

data.entry[E_HNA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HNA]), data.entry[E_HNA], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HNA]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HNA]),data.rk->hna.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HNA]),(gpointer)E_HNA);



sprintf(strsql,"%s (,,)",gettext("Группа бух. учёта"));
data.knopka_enter[E_HNABU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_HNABU]), data.knopka_enter[E_HNABU], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_HNABU]),"clicked",GTK_SIGNAL_FUNC(l_uosin_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_HNABU]),(gpointer)E_HNABU);
tooltips_enter[E_HNABU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_HNABU],data.knopka_enter[E_HNABU],gettext("Выбор группы"),NULL);

data.entry[E_HNABU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HNABU]), data.entry[E_HNABU], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_HNABU]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HNABU]),data.rk->hnabu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_HNABU]),(gpointer)E_HNABU);

label=gtk_label_new(gettext("Поправочный коэффициент для налогового учёта"));
gtk_box_pack_start (GTK_BOX (hbox[E_POPKF_NU]), label, FALSE, FALSE,1);

data.entry[E_POPKF_NU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_POPKF_NU]), data.entry[E_POPKF_NU], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_POPKF_NU]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_POPKF_NU]),data.rk->popkf_nu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_POPKF_NU]),(gpointer)E_POPKF_NU);

label=gtk_label_new(gettext("Поправочный коэффициент для бух. учёта"));
gtk_box_pack_start (GTK_BOX (hbox[E_POPKF_BU]), label, FALSE, FALSE,1);

data.entry[E_POPKF_BU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_POPKF_BU]), data.entry[E_POPKF_BU], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_POPKF_BU]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_POPKF_BU]),data.rk->popkf_bu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_POPKF_BU]),(gpointer)E_POPKF_BU);


label=gtk_label_new(gettext("Гос. номерной знак автотранспорта"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_ZNAK]), label, FALSE, FALSE,1);

data.entry[E_NOM_ZNAK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_ZNAK]), data.entry[E_NOM_ZNAK], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOM_ZNAK]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOM_ZNAK]),data.rk->nom_znak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOM_ZNAK]),(gpointer)E_NOM_ZNAK);


sprintf(strsql,"%s (,,)",gettext("Код материально-ответственного"));
data.knopka_enter[E_MAT_OT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.knopka_enter[E_MAT_OT], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_MAT_OT]),"clicked",GTK_SIGNAL_FUNC(l_uosin_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_MAT_OT]),(gpointer)E_MAT_OT);
tooltips_enter[E_MAT_OT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_MAT_OT],data.knopka_enter[E_MAT_OT],gettext("Выбор материально-ответственного"),NULL);

data.entry[E_MAT_OT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.entry[E_MAT_OT], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_MAT_OT]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MAT_OT]),data.rk->mat_ot.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_MAT_OT]),(gpointer)E_MAT_OT);



sprintf(strsql,"%s (,,)",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE,1);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(l_uosin_p_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE,1);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);



GtkTooltips *tooltips[KOL_PFK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[PFK2]=gtk_button_new_with_label(strsql);
tooltips[PFK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK2],data.knopka[PFK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK2]),"clicked",GTK_SIGNAL_FUNC(l_uosin_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK2]),(gpointer)PFK2);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[PFK4]=gtk_button_new_with_label(strsql);
tooltips[PFK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK4],data.knopka[PFK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK4]),"clicked",GTK_SIGNAL_FUNC(l_uosin_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK4]),(gpointer)PFK4);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[PFK10]=gtk_button_new_with_label(strsql);
tooltips[PFK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[PFK10],data.knopka[PFK10],gettext("Поиск не выполнять"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[PFK10]),"clicked",GTK_SIGNAL_FUNC(l_uosin_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PFK10]),(gpointer)PFK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK2], TRUE, TRUE,1);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK4], TRUE, TRUE,1);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[PFK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 rek_poi->metka_poi=1;
else
 rek_poi->metka_poi=0;
  
return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uosin_p_e_knopka(GtkWidget *widget,class l_uosin_p_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case E_DATA_VVODA:

    if(iceb_calendar(&data->rk->data_vvoda,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_VVODA]),data->rk->data_vvoda.ravno());
      
    return;  

   
  case E_SHETU:

    iceb_vibrek(0,"Plansh",&data->rk->shetu,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->rk->shetu.ravno_toutf());

    return;

  case E_HZT:

    if(l_uoshz(1,&kod,&naim,data->window) == 0)
     data->rk->hzt.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HZT]),data->rk->hzt.ravno_toutf());

    return;
  
  case E_HAU:

    if(l_uoshau(1,&kod,&naim,data->window) == 0)
     data->rk->hau.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HAU]),data->rk->hau.ravno_toutf());

    return;

  case E_HNA:

    if(l_uosgrnu(1,&kod,&naim,data->window) == 0)
     data->rk->hna.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HNA]),data->rk->hna.ravno_toutf());

    return;

  case E_HNABU:

    if(l_uosgrbu(1,&kod,&naim,data->window) == 0)
     data->rk->hnabu.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HNABU]),data->rk->hnabu.ravno_toutf());

    return;

  case E_MAT_OT:

    if(l_uosmo(1,&kod,&naim,data->window) == 0)
     data->rk->mat_ot.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_MAT_OT]),data->rk->mat_ot.ravno_toutf());

    return;
  case E_PODR:

    if(l_uospodr(1,&kod,&naim,data->window) == 0)
     data->rk->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno_toutf());

    return;
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosin_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosin_p_data *data)
{
//char  bros[512];

//printf("vl_uosin_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK2]),"clicked");
    return(FALSE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK4]),"clicked");
    return(TRUE);

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[PFK10]),"clicked");
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
void  l_uosin_p_knopka(GtkWidget *widget,class l_uosin_p_data *data)
{
//char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosin_p_knopka knop=%d\n",knop);

switch (knop)
 {
  case PFK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case PFK4:
    data->clear_data();
    return;  


  case PFK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_uosin_p_vvod(GtkWidget *widget,class l_uosin_p_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_uosin_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_ZAVOD_IZ:
    data->rk->zavod_iz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PASPORT:
    data->rk->pasport.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_MODEL:
    data->rk->model.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_ZAV_NOMER:
    data->rk->zav_nomer.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_DATA_VVODA:
    data->rk->data_vvoda.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHETU:
    data->rk->shetu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_HZT:
    data->rk->hzt.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_HAU:
    data->rk->hau.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_HNA:
    data->rk->hna.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
  case E_HNABU:
    data->rk->hnabu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_POPKF_NU:
    data->rk->popkf_nu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_POPKF_BU:
    data->rk->popkf_bu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NOM_ZNAK:
    data->rk->nom_znak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_MAT_OT:
    data->rk->mat_ot.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_PODR:
    data->rk->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_GOD_IZ:
    data->rk->god_iz.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
  case E_NAIM:
    data->rk->naim.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
