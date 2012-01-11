/*$Id: spis_p_kontrw_m.c,v 1.10 2011-02-21 07:35:57 sasa Exp $*/
/*19.06.2006	19.06.2006	Белых А.И.	spis_p_kontrw_m.c
Меню для ввода реквизитов поиска для списания/получения по контрагентам
*/
#include "../headers/buhg_g.h"
#include "spis_p_kontrw.h"
enum
 {
  E_DATAN,
  E_DATAK,
  E_SKLAD,
  E_KONTR,
  E_GRUP_MAT,
  E_KODOP,
  E_SHETU,
  E_KODMAT,
  E_NOMDOK,
  KOLENTER
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 }; 

class spis_p_kontrw_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class spis_p_kontrw_rr *rk;
      
  spis_p_kontrw_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
/******************
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str sklad;
   class iceb_u_str kontr;
   class iceb_u_str grup_mat;
   class iceb_u_str kodop;
   class iceb_u_str kodmat;
   class iceb_u_str shetu;
   class iceb_u_str nomdok;
   class iceb_u_str prih_rash;
****************/
    rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN]))));
    rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK]))));
    rk->sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SKLAD]))));
    rk->kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR]))));
    rk->grup_mat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_GRUP_MAT]))));
    rk->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP]))));
    rk->kodmat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_KODMAT]))));
    rk->shetu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_SHETU]))));
    rk->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK]))));
//    for(int i=0; i < KOLENTER; i++)
//     {
//      gtk_signal_emit_by_name(GTK_OBJECT(entry[i]),"activate");
//     }
   }

  void clear_rek()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
    gtk_option_menu_set_history(GTK_OPTION_MENU(opt),0);
   }
 };

void    spis_p_kontrw_m_vvod(GtkWidget *widget,class spis_p_kontrw_m_data *data);
void  spis_p_kontrw_m_knopka(GtkWidget *widget,class spis_p_kontrw_m_data *data);
gboolean   spis_p_kontrw_m_key_press(GtkWidget *widget,GdkEventKey *event,class spis_p_kontrw_m_data *data);
void  spis_p_kontrw_m_e_knopka(GtkWidget *widget,class spis_p_kontrw_m_data *data);

extern char *name_system;


int spis_p_kontrw_m(class spis_p_kontrw_rr *rek_poi,
GtkWidget *wpredok)
{
//printf("spis_p_kontrw_m\n");

class spis_p_kontrw_m_data data;
char strsql[512];
data.rk=rek_poi;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",name_system,gettext("Расчёт списания/получения материаллов по контрагентам"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(spis_p_kontrw_m_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }



GtkWidget *vbox = gtk_vbox_new (FALSE, 0);

GtkWidget *label=gtk_label_new (gettext("Расчёт списания/получения материаллов по контрагентам"));
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);


GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 hbox[i] = gtk_hbox_new (FALSE, 1);
GtkWidget *hboxknop = gtk_hbox_new (FALSE, 1);
GtkWidget *hbox_opt = gtk_hbox_new (FALSE, 0);

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox), hbox[i], FALSE, FALSE, 0);

gtk_box_pack_start (GTK_BOX (vbox), hbox_opt, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE, 0);


GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(spis_p_kontrw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);


sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(spis_p_kontrw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);


sprintf(strsql,"%s (,,)",gettext("Склад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKLAD]),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKLAD]),(gpointer)E_SKLAD);
tooltips_enter[E_SKLAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SKLAD],data.knopka_enter[E_SKLAD],gettext("Выбор склада"),NULL);

data.entry[E_SKLAD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKLAD]), "activate",GTK_SIGNAL_FUNC(spis_p_kontrw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKLAD]),(gpointer)E_SKLAD);


sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KONTR]),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KONTR]),(gpointer)E_KONTR);
tooltips_enter[E_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KONTR],data.knopka_enter[E_KONTR],gettext("Выбор контрагента"),NULL);

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KONTR]), "activate",GTK_SIGNAL_FUNC(spis_p_kontrw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk->kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KONTR]),(gpointer)E_KONTR);


sprintf(strsql,"%s (,,)",gettext("Группа материалла"));
data.knopka_enter[E_GRUP_MAT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_MAT]), data.knopka_enter[E_GRUP_MAT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUP_MAT]),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUP_MAT]),(gpointer)E_GRUP_MAT);
tooltips_enter[E_GRUP_MAT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUP_MAT],data.knopka_enter[E_GRUP_MAT],gettext("Выбор группы"),NULL);

data.entry[E_GRUP_MAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_MAT]), data.entry[E_GRUP_MAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUP_MAT]), "activate",GTK_SIGNAL_FUNC(spis_p_kontrw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP_MAT]),data.rk->grup_mat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUP_MAT]),(gpointer)E_GRUP_MAT);

sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODOP]),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODOP]),(gpointer)E_KODOP);
tooltips_enter[E_KODOP]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODOP],data.knopka_enter[E_KODOP],gettext("Выбор операции"),NULL);

data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODOP]), "activate",GTK_SIGNAL_FUNC(spis_p_kontrw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODOP]),(gpointer)E_KODOP);


sprintf(strsql,"%s (,,)",gettext("Счёт учёта"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHETU]),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHETU]),(gpointer)E_SHETU);
tooltips_enter[E_SHETU]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHETU],data.knopka_enter[E_SHETU],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHETU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHETU]), "activate",GTK_SIGNAL_FUNC(spis_p_kontrw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.rk->shetu.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHETU]),(gpointer)E_SHETU);


sprintf(strsql,"%s (,,)",gettext("Код материалла"));
data.knopka_enter[E_KODMAT]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.knopka_enter[E_KODMAT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODMAT]),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_m_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODMAT]),(gpointer)E_KODMAT);
tooltips_enter[E_KODMAT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODMAT],data.knopka_enter[E_KODMAT],gettext("Выбор материалла"),NULL);

data.entry[E_KODMAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.entry[E_KODMAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODMAT]), "activate",GTK_SIGNAL_FUNC(spis_p_kontrw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODMAT]),data.rk->kodmat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODMAT]),(gpointer)E_KODMAT);


sprintf(strsql,"%s (,,)",gettext("Номер документа"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE, 0);

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_NOMDOK]), "activate",GTK_SIGNAL_FUNC(spis_p_kontrw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdok.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_NOMDOK]),(gpointer)E_NOMDOK);


sprintf(strsql,"%s",gettext("Приход/расход"));
label=gtk_label_new(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox_opt), label, FALSE, FALSE, 0);

iceb_pm_pr(&data.opt,&data.rk->prih_rash);
gtk_box_pack_start (GTK_BOX (hbox_opt), data.opt, TRUE, TRUE, 0);

GtkTooltips *tooltips[KOL_FK];

sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать поиск нужных записей"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введеноой информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Поиск не выполнять"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(spis_p_kontrw_m_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

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
void  spis_p_kontrw_m_e_knopka(GtkWidget *widget,class spis_p_kontrw_m_data *data)
{
class iceb_u_str kod;
class iceb_u_str naim;
naim.plus("");
kod.plus("");

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("go_v_e_knopka knop=%d\n",knop);

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

  case E_SKLAD:

    iceb_vibrek(0,"Sklad",&data->rk->sklad,&naim,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno_toutf());
      
    return;  

  case E_KONTR:

    iceb_vibrek(0,"Kontragent",&data->rk->kontr,&naim,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk->kontr.ravno_toutf());
      
    return;  


  case E_GRUP_MAT:

    iceb_vibrek(0,"Grup",&data->rk->grup_mat,&naim,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP_MAT]),data->rk->grup_mat.ravno_toutf());
      
    return;  

  case E_KODOP:

    vib_kodop_mu(&data->rk->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno_toutf());
      
    return;  

  case E_SHETU:

    iceb_vibrek(0,"Plansh",&data->rk->shetu,&naim,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->rk->shetu.ravno_toutf());
      
    return;  

  case E_KODMAT:
    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     data->rk->kodmat.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno_toutf());
      
    return;  
   
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spis_p_kontrw_m_key_press(GtkWidget *widget,GdkEventKey *event,class spis_p_kontrw_m_data *data)
{
//char  bros[512];

//printf("vei_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(FALSE);

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
void  spis_p_kontrw_m_knopka(GtkWidget *widget,class spis_p_kontrw_m_data *data)
{
//char bros[512];

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("spis_p_kontrw_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(iceb_rsdatp(data->rk->datan.ravno(),data->rk->datak.ravno(),data->window) != 0)
     return;

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_rek();
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    spis_p_kontrw_m_vvod(GtkWidget *widget,class spis_p_kontrw_m_data *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("spis_p_kontrw_m_vvod enter=%d\n",enter);

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

  case E_KONTR:
    data->rk->kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUP_MAT:
    data->rk->grup_mat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODOP:
    data->rk->kodop.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHETU:
    data->rk->shetu.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODMAT:
    data->rk->kodmat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_NOMDOK:
    data->rk->nomdok.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
