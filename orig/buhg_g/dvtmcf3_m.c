/*$Id: dvtmcf3_m.c,v 1.15 2011-02-21 07:35:51 sasa Exp $*/
/*19.11.2005	30.11.2004	Белых А.И.	dvtmcf3_m.c
Меню для воода реквизитов расчёта движения товарно-материальных ценностей форма 3
*/
#include "buhg_g.h"
#include "dvtmcf3.h"
enum
{
  FK2,
  FK3,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAN,
  E_DATAK,
  E_SHET,
  E_SKLAD,
  E_KODGR,
  E_KODMAT,
  E_GRUPA_KONTR,
  E_VCENA,
  KOLENTER  
 };

class dvtmcf3_data
 {
  public:
  dvtmcf3_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчет 1 нет
  GtkWidget *radiobutton[2];
  GtkWidget *radiobutton1[3];

  dvtmcf3_data() //Конструктор
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


void       dvtmcf3_radio0(GtkWidget *,class dvtmcf3_data *);
void       dvtmcf3_radio1(GtkWidget *,class dvtmcf3_data *);

void       dvtmcf3_radio01(GtkWidget *,class dvtmcf3_data *);
void       dvtmcf3_radio11(GtkWidget *,class dvtmcf3_data *);
void       dvtmcf3_radio12(GtkWidget *,class dvtmcf3_data *);

gboolean   dvtmcf3_v_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf3_data *data);
void    dvtmcf3_v_vvod(GtkWidget *widget,class dvtmcf3_data *data);
void  dvtmcf3_v_knopka(GtkWidget *widget,class dvtmcf3_data *data);
void   dvtmcf3_rekviz(class dvtmcf3_data *data);
void  dvtmcf3_v_e_knopka(GtkWidget *widget,class dvtmcf3_data *data);

extern char *name_system;
extern SQL_baza bd;
extern double nds1;

int   dvtmcf3_m(class dvtmcf3_rr *rek_ras)
{


class dvtmcf3_data data;

data.rk=rek_ras;
char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",name_system,gettext("Распечетать движение товарно-материальных ценностей (форма 3)"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(dvtmcf3_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Распечетать движение товарно-материальных ценностей (форма 3)"));

GtkWidget *vbox = gtk_vbox_new (FALSE, 1);
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
  hbox[i] = gtk_hbox_new (FALSE, 0);

GtkWidget *hboxknop = gtk_hbox_new (FALSE, 0);
GtkWidget *separator1=gtk_hseparator_new();
GtkWidget *separator2=gtk_hseparator_new();
GtkWidget *separator3=gtk_hseparator_new();


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), separator1);

//Вставляем радиокнопки
GSList *group;

data.radiobutton[0]=gtk_radio_button_new_with_label(NULL,gettext("Расчёт в ценах учёта"));
if(data.rk->metka_ceni == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton[0]), "clicked",GTK_SIGNAL_FUNC(dvtmcf3_radio0),&data);
//gtk_object_set_user_data(GTK_OBJECT(data.radiobutton0),(gpointer)"0");

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton[0]));

data.radiobutton[1]=gtk_radio_button_new_with_label(group,gettext("Расчёт в ценах реализации"));
if(data.rk->metka_ceni == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton[1]), "clicked",GTK_SIGNAL_FUNC(dvtmcf3_radio1),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton[1], TRUE, TRUE, 0);

/*************/

gtk_container_add (GTK_CONTAINER (vbox), separator2);

data.radiobutton1[0]=gtk_radio_button_new_with_label(NULL,gettext("Распечатка только таблиц"));
if(data.rk->metka_ras == 0)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[0]),TRUE); //Устанавливем активной кнопку
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[0], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.radiobutton1[0]), "clicked",GTK_SIGNAL_FUNC(dvtmcf3_radio01),&data);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton1[0]));

data.radiobutton1[1]=gtk_radio_button_new_with_label(group,gettext("Распечатка полного отчёта"));
if(data.rk->metka_ras == 1)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[1]),TRUE); //Устанавливем активной кнопку
gtk_signal_connect(GTK_OBJECT(data.radiobutton1[1]), "clicked",GTK_SIGNAL_FUNC(dvtmcf3_radio11),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[1], TRUE, TRUE, 0);

group=gtk_radio_button_group(GTK_RADIO_BUTTON(data.radiobutton1[1]));

data.radiobutton1[2]=gtk_radio_button_new_with_label(group,gettext("Распечатка таблиц с полученным доходом"));
if(data.rk->metka_ras == 3)
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton1[2]),TRUE); //Устанавливем активной кнопку

gtk_signal_connect(GTK_OBJECT(data.radiobutton1[2]), "clicked",GTK_SIGNAL_FUNC(dvtmcf3_radio12),&data);
gtk_box_pack_start (GTK_BOX (vbox),data.radiobutton1[2], TRUE, TRUE, 0);

gtk_container_add (GTK_CONTAINER (vbox), separator3);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkTooltips *tooltips_enter[KOLENTER];

sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAN]),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAN]),(gpointer)E_DATAN);
tooltips_enter[E_DATAN]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAN],data.knopka_enter[E_DATAN],gettext("Выбор даты"),NULL);

data.entry[E_DATAN] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAN]), "activate",GTK_SIGNAL_FUNC(dvtmcf3_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAN]),(gpointer)E_DATAN);

sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(iceb_u_toutf(strsql));
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_DATAK]),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_DATAK]),(gpointer)E_DATAK);
tooltips_enter[E_DATAK]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_DATAK],data.knopka_enter[E_DATAK],gettext("Выбор даты"),NULL);

data.entry[E_DATAK] = gtk_entry_new_with_max_length (10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_DATAK]), "activate",GTK_SIGNAL_FUNC(dvtmcf3_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_DATAK]),(gpointer)E_DATAK);

sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SHET]),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SHET]),(gpointer)E_SHET);
tooltips_enter[E_SHET]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SHET],data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"),NULL);

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SHET]), "activate",GTK_SIGNAL_FUNC(dvtmcf3_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SHET]),(gpointer)E_SHET);

sprintf(strsql,"%s (,,)",gettext("Склад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_SKLAD]),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_SKLAD]),(gpointer)E_SKLAD);
tooltips_enter[E_SKLAD]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_SKLAD],data.knopka_enter[E_SKLAD],gettext("Выбор склада"),NULL);

data.entry[E_SKLAD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_SKLAD]), "activate",GTK_SIGNAL_FUNC(dvtmcf3_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_SKLAD]),(gpointer)E_SKLAD);

sprintf(strsql,"%s (,,)",gettext("Код группы материалла"));
data.knopka_enter[E_KODGR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.knopka_enter[E_KODGR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODGR]),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODGR]),(gpointer)E_KODGR);
tooltips_enter[E_KODGR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODGR],data.knopka_enter[E_KODGR],gettext("Выбор группы"),NULL);

data.entry[E_KODGR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODGR]), data.entry[E_KODGR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODGR]), "activate",GTK_SIGNAL_FUNC(dvtmcf3_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODGR]),data.rk->grupa_mat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODGR]),(gpointer)E_KODGR);

sprintf(strsql,"%s (,,)",gettext("Код материалла"));
data.knopka_enter[E_KODMAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.knopka_enter[E_KODMAT], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_KODMAT]),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_KODMAT]),(gpointer)E_KODMAT);
tooltips_enter[E_KODMAT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_KODMAT],data.knopka_enter[E_KODMAT],gettext("Выбор материалла"),NULL);

data.entry[E_KODMAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.entry[E_KODMAT], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_KODMAT]), "activate",GTK_SIGNAL_FUNC(dvtmcf3_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODMAT]),data.rk->kodmat.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_KODMAT]),(gpointer)E_KODMAT);

sprintf(strsql,"%s (,,)",gettext("Код группы контрагента"));
data.knopka_enter[E_GRUPA_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA_KONTR]), data.knopka_enter[E_GRUPA_KONTR], FALSE, FALSE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka_enter[E_GRUPA_KONTR]),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_v_e_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka_enter[E_GRUPA_KONTR]),(gpointer)E_GRUPA_KONTR);
tooltips_enter[E_GRUPA_KONTR]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips_enter[E_GRUPA_KONTR],data.knopka_enter[E_GRUPA_KONTR],gettext("Выбор материалла"),NULL);

//sprintf(strsql,"%s (,,)",gettext("Код группы контрагента"));
//label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_GRUPA_KONTR] = gtk_entry_new();
//gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA_KONTR]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA_KONTR]), data.entry[E_GRUPA_KONTR], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_GRUPA_KONTR]), "activate",GTK_SIGNAL_FUNC(dvtmcf3_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA_KONTR]),data.rk->grupa_kontr.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_GRUPA_KONTR]),(gpointer)E_GRUPA_KONTR);

sprintf(strsql,"%s",gettext("Отпускная цена"));
label=gtk_label_new(iceb_u_toutf(strsql));
data.entry[E_VCENA] = gtk_entry_new_with_max_length (20);
gtk_box_pack_start (GTK_BOX (hbox[E_VCENA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_VCENA]), data.entry[E_VCENA], TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT (data.entry[E_VCENA]), "activate",GTK_SIGNAL_FUNC(dvtmcf3_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VCENA]),data.rk->vcena.ravno_toutf());
gtk_object_set_user_data(GTK_OBJECT(data.entry[E_VCENA]),(gpointer)E_VCENA);



GtkTooltips *tooltips[KOL_F_KL];

sprintf(strsql,"F2 %s",gettext("Расчет"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Начать расчет"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Просмотр нужных реквизитов"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Очистить меню от введенной информации"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  dvtmcf3_radio0(GtkWidget *widget,class dvtmcf3_data *data)
{
//g_print("dvtmcf3_radio0\n");

if(data->rk->metka_ceni == 0)
  return;
data->rk->metka_ceni=0;

}
void  dvtmcf3_radio1(GtkWidget *widget,class dvtmcf3_data *data)
{
//g_print("dvtmcf3_radio1\n");
if(data->rk->metka_ceni == 1)
  return;
data->rk->metka_ceni=1;
}

/**********************/

void  dvtmcf3_radio01(GtkWidget *widget,class dvtmcf3_data *data)
{
//g_print("dvtmcf3_radio0\n");

if(data->rk->metka_ras == 0)
  return;
data->rk->metka_ras=0;

}
void  dvtmcf3_radio11(GtkWidget *widget,class dvtmcf3_data *data)
{
//g_print("dvtmcf3_radio1\n");
if(data->rk->metka_ras == 1)
  return;
data->rk->metka_ras=1;
}
void  dvtmcf3_radio12(GtkWidget *widget,class dvtmcf3_data *data)
{
//g_print("dvtmcf3_radio1\n");
if(data->rk->metka_ras == 2)
  return;
data->rk->metka_ras=2;
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  dvtmcf3_v_e_knopka(GtkWidget *widget,class dvtmcf3_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

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

   
  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno_toutf());

    return;  

  case E_SKLAD:

    iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno_toutf());

    return;  


  case E_KODMAT:

    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->rk->kodmat.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno_toutf());
     }

    return;  

  case E_GRUPA_KONTR:

    iceb_vibrek(0,"Gkont",&data->rk->grupa_kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA_KONTR]),data->rk->grupa_kontr.ravno_toutf());


    return;  

  case E_KODGR:

    iceb_vibrek(0,"Grup",&data->rk->grupa_mat,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGR]),data->rk->grupa_mat.ravno_toutf());

    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dvtmcf3_v_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf3_data *data)
{

//printf("dvtmcf3_v_key_press\n");
switch(event->keyval)
 {

  case GDK_F2:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
    return(TRUE);

  case GDK_F3:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
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
void  dvtmcf3_v_knopka(GtkWidget *widget,class dvtmcf3_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(iceb_rsdatp_str(&data->rk->datan,&data->rk->datak,data->window) != 0)
     return;
     
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK3:
    dvtmcf3_rekviz(data);

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

void    dvtmcf3_v_vvod(GtkWidget *widget,class dvtmcf3_data *data)
{
//short d,m,g;

int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("dvtmcf3_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SHET:
    data->rk->shet.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_SKLAD:
    data->rk->sklad.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODGR:
    data->rk->grupa_mat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_KODMAT:
    data->rk->kodmat.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_GRUPA_KONTR:
    data->rk->grupa_kontr.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

  case E_VCENA:
    data->rk->vcena.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************/
/*Работа с реквизитами*/
/***********************/
void   dvtmcf3_rekviz(class dvtmcf3_data *data)
{
short nomer=0;

iceb_u_str titl;
iceb_u_str zamuinvlovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));

zamuinvlovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр списка счетов"));
punkt_m.plus(gettext("Просмотр списка материаллов"));
punkt_m.plus(gettext("Просмотр списка групп"));
punkt_m.plus(gettext("Просмотр списка складов"));
punkt_m.plus(gettext("Просмотр списка групп контрагентов"));

nomer=iceb_menu_mv(&titl,&zamuinvlovok,&punkt_m,nomer,data->window);



iceb_u_str kod;
iceb_u_str naikod;

switch (nomer)
 {
  case -1:
    return;
    
  case 0:
    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),iceb_u_toutf(data->rk->shet.ravno_toutf()));
    break;


  case 1:
     if(l_mater(1,&kod,&naikod,0,0,NULL) == 0)
      {
       data->rk->kodmat.z_plus(kod.ravno());
       gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),iceb_u_toutf(data->rk->kodmat.ravno_toutf()));
      }
    break;

  case 2:
    iceb_vibrek(0,"Grup",&data->rk->grupa_mat,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODGR]),iceb_u_toutf(data->rk->grupa_mat.ravno_toutf()));
    break;

  case 3:
    iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),iceb_u_toutf(data->rk->sklad.ravno_toutf()));
    break;

  case 4:
    iceb_vibrek(0,"Gkont",&data->rk->grupa_kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA_KONTR]),iceb_u_toutf(data->rk->grupa_kontr.ravno_toutf()));
    break;

 }
}
