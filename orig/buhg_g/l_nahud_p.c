/*$Id: l_nahud_p.c,v 1.15 2011-02-21 07:35:53 sasa Exp $*/
/*12.09.2009	28.09.2006	Белых А.И.	l_nahud_p.c
Ввод реквизитов поиска
*/
#include "buhg_g.h"
#include "l_nahud.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_PODR,
  E_TABNOM,
  E_FIO,
  E_DATA_PROS,
  E_DEN_N_PROS,
  E_DEN_K_PROS,
  E_KATEG,
  E_SHET,
  E_VKM,
  E_KOMENT,
  E_GRUP_PODR,
  E_MNKS,
  E_KODBANKA,
  KOLENTER  
 };

class l_nahud_p_data
 {
  public:
  class l_nahud_rek *poi;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  short kl_shift;
  int voz;  
  
  l_nahud_p_data() //Конструктор
   {
    kl_shift=0;
    voz=0;  
   }

  void read_rek()
   {
    poi->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_PODR]))));
    poi->mes_god_pros.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_PROS]))));
    poi->den_n_pros.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DEN_N_PROS]))));
    poi->den_k_pros.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DEN_K_PROS]))));
    poi->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM]))));
    poi->fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_FIO]))));
    poi->kateg.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KATEG]))));
    poi->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHET]))));
    poi->vkm.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_VKM]))));
    poi->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT]))));
    poi->grup_podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GRUP_PODR]))));
    poi->mnks.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_MNKS]))));
    poi->kodbanka.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODBANKA]))));
//    for(int i=0; i < KOLENTER; i++)
//      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    poi->clear_rek();
   }
 };

gboolean   l_nahud_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_p_data *data);
void    l_nahud_p_vvod(GtkWidget *widget,class l_nahud_p_data *data);
void  l_nahud_p_knopka(GtkWidget *widget,class l_nahud_p_data *data);

void  l_nahud_v_e_knopka(GtkWidget *widget,class l_nahud_p_data *data);

extern char *name_system;
extern SQL_baza bd;

int l_nahud_p(class l_nahud_rek *datap,GtkWidget *wpredok)
{
l_nahud_p_data data;
data.poi=datap;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Поиск"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(l_nahud_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Поиск"));

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

sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_PODR]),"clicked",GTK_SIGNAL_FUNC(l_nahud_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_PODR]),(gpointer)E_PODR);
tooltips_enter[E_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_PODR],data.knopka_enter[E_PODR],gettext("Выбор подразделения"),NULL);

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_PODR]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.poi->podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_PODR]),(gpointer)E_PODR);


sprintf(strsql,"%s (,,)",gettext("Табельный номер"));
data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_TABNOM]),"clicked",GTK_SIGNAL_FUNC(l_nahud_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_TABNOM]),(gpointer)E_TABNOM);
tooltips_enter[E_TABNOM]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_TABNOM],data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"),NULL);

data.entry[E_TABNOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_TABNOM]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.poi->tabnom.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_TABNOM]),(gpointer)E_TABNOM);


sprintf(strsql,"%s",gettext("Фамилия"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);

data.entry[E_FIO] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_FIO]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.poi->fio.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_FIO]),(gpointer)E_FIO);


sprintf(strsql,"%s (%s)",gettext("Дата просмотра"),gettext("м.г"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PROS]), label, FALSE, FALSE, 0);

data.entry[E_DATA_PROS] = gtk_entry_new_with_max_length(7);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PROS]), data.entry[E_DATA_PROS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATA_PROS]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_PROS]),data.poi->mes_god_pros.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATA_PROS]),(gpointer)E_DATA_PROS);


sprintf(strsql,"%s",gettext("День начала"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DEN_N_PROS]), label, FALSE, FALSE, 0);

data.entry[E_DEN_N_PROS] = gtk_entry_new_with_max_length(2);
gtk_box_pack_start (GTK_BOX (hbox[E_DEN_N_PROS]), data.entry[E_DEN_N_PROS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DEN_N_PROS]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEN_N_PROS]),data.poi->den_n_pros.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DEN_N_PROS]),(gpointer)E_DEN_N_PROS);


sprintf(strsql,"%s",gettext("День конца"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DEN_K_PROS]), label, FALSE, FALSE, 0);

data.entry[E_DEN_K_PROS] = gtk_entry_new_with_max_length(2);
gtk_box_pack_start (GTK_BOX (hbox[E_DEN_K_PROS]), data.entry[E_DEN_K_PROS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DEN_K_PROS]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEN_K_PROS]),data.poi->den_k_pros.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DEN_K_PROS]),(gpointer)E_DEN_K_PROS);


sprintf(strsql,"%s (,,)",gettext("Категория"));
data.knopka_enter[E_KATEG]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KATEG]), data.knopka_enter[E_KATEG], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KATEG]),"clicked",GTK_SIGNAL_FUNC(l_nahud_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KATEG]),(gpointer)E_KATEG);
tooltips_enter[E_KATEG]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KATEG],data.knopka_enter[E_KATEG],gettext("Выбор категории"),NULL);

data.entry[E_KATEG] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KATEG]), data.entry[E_KATEG], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KATEG]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KATEG]),data.poi->kateg.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KATEG]),(gpointer)E_KATEG);


sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(l_nahud_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.poi->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);


sprintf(strsql,"%s (%s)",gettext("В счёт какого месяца"),gettext("м.г"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_VKM]), label, FALSE, FALSE, 0);

data.entry[E_VKM] = gtk_entry_new_with_max_length(7);
gtk_box_pack_start (GTK_BOX (hbox[E_VKM]), data.entry[E_VKM], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VKM]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VKM]),data.poi->vkm.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VKM]),(gpointer)E_VKM);


sprintf(strsql,"%s",gettext("Коментарий"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KOMENT]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.poi->koment.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KOMENT]),(gpointer)E_KOMENT);


sprintf(strsql,"%s (,,)",gettext("Код группы подразделения"));
data.knopka_enter[E_GRUP_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_PODR]), data.knopka_enter[E_GRUP_PODR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUP_PODR]),"clicked",GTK_SIGNAL_FUNC(l_nahud_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUP_PODR]),(gpointer)E_GRUP_PODR);
tooltips_enter[E_GRUP_PODR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUP_PODR],data.knopka_enter[E_GRUP_PODR],gettext("Выбор группы подразделения"),NULL);

data.entry[E_GRUP_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_PODR]), data.entry[E_GRUP_PODR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUP_PODR]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP_PODR]),data.poi->grup_podr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUP_PODR]),(gpointer)E_GRUP_PODR);


sprintf(strsql,"%s (+/-)",gettext("Карт-счёт"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_MNKS]), label, FALSE, FALSE, 0);

data.entry[E_MNKS] = gtk_entry_new_with_max_length(1);
gtk_box_pack_start (GTK_BOX (hbox[E_MNKS]), data.entry[E_MNKS], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_MNKS]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MNKS]),data.poi->mnks.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_MNKS]),(gpointer)E_MNKS);


sprintf(strsql,"%s (,,)",gettext("Код банка"));
data.knopka_enter[E_KODBANKA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODBANKA]), data.knopka_enter[E_KODBANKA], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODBANKA]),"clicked",GTK_SIGNAL_FUNC(l_nahud_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODBANKA]),(gpointer)E_KODBANKA);
tooltips_enter[E_KODBANKA]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODBANKA],data.knopka_enter[E_KODBANKA],gettext("Выбор банка"),NULL);

data.entry[E_KODBANKA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODBANKA]), data.entry[E_KODBANKA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODBANKA]), "activate",GTK_SIGNAL_FUNC(l_nahud_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODBANKA]),data.poi->kodbanka.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODBANKA]),(gpointer)E_KODBANKA);

GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(l_nahud_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(l_nahud_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(l_nahud_p_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();
//printf("l_l_nahud_p-metka_poi=%d\n",data.poi->metka_poi);
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_nahud_v_e_knopka(GtkWidget *widget,class l_nahud_p_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_nahud_v_e_knopka knop=%d\n",knop);

switch (knop)
 {

  case E_PODR:

    if(l_zarpodr(1,&kod,&naim,data->window) == 0)
     data->poi->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->poi->podr.ravno_toutf());

    return;  

  case E_TABNOM:

    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->poi->tabnom.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->poi->tabnom.ravno_toutf());

    return;  

  case E_KATEG:

    if(l_zarkateg(1,&kod,&naim,data->window) == 0)
     data->poi->kateg.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KATEG]),data->poi->kateg.ravno_toutf());

    return;  

  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->poi->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->poi->shet.ravno_toutf());

    return;  

  case E_GRUP_PODR:

    if(l_gruppod(1,&kod,&naim,data->window) == 0)
     data->poi->grup_podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP_PODR]),data->poi->grup_podr.ravno_toutf());

    return;  

  case E_KODBANKA:

    if(l_zarsb(1,&kod,&naim,data->window) == 0)
     data->poi->kodbanka.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODBANKA]),data->poi->kodbanka.ravno_toutf());

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_nahud_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_p_data *data)
{

//printf("l_nahud_p_key_press\n");
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
void  l_nahud_p_knopka(GtkWidget *widget,class l_nahud_p_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->poi->mes_god_pros.prov_dat1() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата просмотра !"),data->window);
      return;     
     }
    if(data->poi->vkm.prov_dat1() != 0)
     {
      iceb_menu_soob(gettext("Неправильно ввели в счёт какого месяца !"),data->window);
      return;     
     }
    data->voz=0;
    data->poi->metka_poi=1;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->poi->metka_poi=0;
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_nahud_p_vvod(GtkWidget *widget,class l_nahud_p_data *data)
{

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("l_nahud_p_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_PODR:
    data->poi->podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_TABNOM:
    data->poi->tabnom.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KATEG:
    data->poi->kateg.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET:
    data->poi->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUP_PODR:
    data->poi->grup_podr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_FIO:
    data->poi->fio.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATA_PROS:
    data->poi->mes_god_pros.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DEN_N_PROS:
    data->poi->den_n_pros.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DEN_K_PROS:
    data->poi->den_k_pros.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VKM:
    data->poi->vkm.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KOMENT:
    data->poi->koment.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_MNKS:
    data->poi->mnks.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;


 }

enter+=1;

if(enter >= KOLENTER)
enter=0;


gtk_widget_grab_focus(data->entry[enter]);
 
}

